
#include <string.h>

#include "../src/gbemu/gb.h"

// Make sure NDEBUG was not set during build step to ensure all asserts are
// enabled.
#ifdef NDEBUG
#error "NDEBUG must not be set for testing"
#endif

#define BLARGG_MAX_CYCLES 10000000

#define assert_eq_u8(a, b) log_assert_eq(a, b, "0x%02X")

static const char *blargg_roms[] = {
    "roms/01-special.gb",
    "roms/02-interrupts.gb",
    "roms/03-op sp,hl.gb",
    "roms/04-op r,imm.gb",
    "roms/05-op rp.gb",
    "roms/06-ld r,r.gb",
    "roms/07-jr,jp,call,ret,rst.gb",
    "roms/08-misc instrs.gb",
    "roms/09-op r,r.gb",
    "roms/10-bit ops.gb",
    "roms/11-op a,(hl).gb",
};

static void test_cart(void) {
    const char *filename = blargg_roms[0];

    GameBoy gb defer(gb_deinit);
    err_exit(gb_init(&gb));
    err_exit(gb_load_rom_file(&gb, filename));

    Cartridge *cart = &gb.bus.cart;
    log_assert(cart);
    log_assert(cart_is_valid_header(cart));

    // Peek some arbitrary memory location
    assert_eq_u8(cart_read(cart, 0x200), 0x47);
    assert_eq_u8(cart_read(cart, 0x210), 0xC3);
}

static void test_microcode_is_valid(void) {
    for (size_t opcode = 0; opcode <= 0xFF; ++opcode) {
        for (size_t ustep = 0; ustep < MICRO_INSTRUCTION_SIZE; ++ustep) {
            const MicroInstr *uinst = instructions_get_uinst(opcode, ustep);

            // Can't do multiple bus interactions on the same step,
            // and step 0 has an implicit fetch.
            if (ustep == 0 && uinst->io != IO_NONE) {
                panicf("Illegal IO on step 0 in opcode $%02X", (u8)opcode);
            }

            // If there is a rhs, there must be an lhs
            if (uinst->rhs && !uinst->lhs) {
                panicf("Opcode $%02X step %d has RHS, but no LHS", (u8)opcode,
                       (u8)ustep);
            }

            if (uinst->end) {
                break;
            }
        }
    }
}

static void test_cpu_jp(void) {
    const u8 prog[] = {
        0xC3, 0xAA, 0xBB,  // JP $BBAA
    };
    GameBoy gb defer(gb_deinit);
    err_exit(gb_init(&gb));
    err_exit(gb_load_bootrom_buffer(&gb, prog, sizeof(prog)));

    // Read opcode
    cpu_cycle(&gb.cpu, &gb.bus);
    assert_eq_u8(gb.cpu.opcode, 0xC3);
    assert_eq_u8(gb.cpu.pc, 1);

    // Read address
    cpu_cycle(&gb.cpu, &gb.bus);
    cpu_cycle(&gb.cpu, &gb.bus);
    assert_eq_u8(gb.cpu.pc, 3);

    // Jump
    cpu_cycle(&gb.cpu, &gb.bus);
    assert_eq_u8(gb.cpu.pc, 0xBBAA);
}

static void test_cpu_halt(void) {
    const u8 prog[] = {
        0x76,  // HALT
    };

    GameBoy gb defer(gb_deinit);
    err_exit(gb_init(&gb));
    err_exit(gb_load_bootrom_buffer(&gb, prog, sizeof(prog)));

    log_assert(!gb.cpu.halted);
    gb_step(&gb);
    log_assert(gb.cpu.halted);
}

static void test_cpu_ld_xor(void) {
    const u8 prog[] = {
        0x3E, 0x55,  // LD A,$55
        0x06, 0xF0,  // LD B,$F0
        0xA8,        // XOR B     ; expect A == 0xA5, F == 0x00
        0x76,        // HALT
        0xA8,        // XOR B     ; expect A == 0x55, F == 0x00
        0xAF,        // XOR A     ; expect A == 0x00, F == 0x80
        0x76,        // HALT
    };
    GameBoy gb defer(gb_deinit);
    err_exit(gb_init(&gb));
    err_exit(gb_load_bootrom_buffer(&gb, prog, sizeof(prog)));

    gb_run_until_halt(&gb);

    assert_eq_u8(gb.cpu.a, 0xA5);
    assert_eq_u8(gb.cpu.f, 0x00);

    gb_run_until_halt(&gb);
    assert_eq_u8(gb.cpu.a, 0x00);
    assert_eq_u8(gb.cpu.f, 0x80);
}

static void test_cpu_inc_dec(void) {
    const u8 prog[] = {
        0x05,  // DEC B ; expect B == 0xFF, F == 0x60
        0x05,  // DEC B ; expect B == 0xFE, F == 0x40
        0x04,  // INC B ; expect B == 0xFF, F == 0x00
        0x04,  // INC B ; expect B == 0x00, F == 0xA0
    };

    GameBoy gb defer(gb_deinit);
    err_exit(gb_init(&gb));
    err_exit(gb_load_bootrom_buffer(&gb, prog, sizeof(prog)));

    gb_step(&gb);
    assert_eq_u8(gb.cpu.b, 0xFF);
    assert_eq_u8(gb.cpu.f, 0x60);

    gb_step(&gb);
    assert_eq_u8(gb.cpu.b, 0xFE);
    assert_eq_u8(gb.cpu.f, 0x40);

    gb_step(&gb);
    assert_eq_u8(gb.cpu.b, 0xFF);
    assert_eq_u8(gb.cpu.f, 0x00);

    gb_step(&gb);
    assert_eq_u8(gb.cpu.b, 0x00);
    assert_eq_u8(gb.cpu.f, 0xA0);
}

static void test_cpu_hl(void) {
    const u8 prog[] = {
        0x2E, 0x00,  // LD L,$00
        0x26, 0xC0,  // LD H,$C0
        0x3E, 0x55,  // LD A,$55
        0x77,        // LD (HL),A
        0x76,        // HALT
    };

    GameBoy gb defer(gb_deinit);
    err_exit(gb_init(&gb));
    err_exit(gb_load_bootrom_buffer(&gb, prog, sizeof(prog)));

    gb_run_until_halt(&gb);
    assert_eq_u8(bus_read(&gb.bus, 0xC000), 0x55);
}

static void test_cpu_arith(void) {
    const u8 prog[] = {
        0x3E, 0x01,  // LD A,$01
        0x06, 0x02,  // LD B,$02
        0x80,        // ADD A,B  ; A: $03, F: 0000
        0x76,        // HALT     ; #1
                     //
        0x3E, 0xFF,  // LD A,$FF
        0x06, 0x01,  // LD B,$01
        0x80,        // ADD A,B  ; A: $00, F: 1011
        0x76,        // HALT     ; #2
                     //
        0x88,        // ADC A,B  ; A: $02, F: 0000
        0x76,        // HALT     ; #3
                     //
        0x3E, 0x01,  // LD A,$01
        0x06, 0x02,  // LD B,$02
        0x90,        // SUB A,B  ; A: $FF, F: 0111
        0x76,        // HALT     ; #4
                     //
        0x98,        // SBC A,B  ; A: $FC, F: 0100
        0x76,        // HALT     ; #5
                     //
        0x98,        // SBC A,B  ; A: $FA, F: 0100
        0x76,        // HALT     ; #6
    };

    GameBoy gb defer(gb_deinit);
    err_exit(gb_init(&gb));
    err_exit(gb_load_bootrom_buffer(&gb, prog, sizeof(prog)));

    gb_run_until_halt(&gb);
    // Halt 1
    assert_eq_u8(gb.cpu.a, 0x03);
    assert_eq_u8(gb.cpu.f, 0x00);

    gb_run_until_halt(&gb);
    // Halt 2
    assert_eq_u8(gb.cpu.a, 0x00);
    assert_eq_u8(gb.cpu.f, 0xB0);

    gb_run_until_halt(&gb);
    // Halt 3
    assert_eq_u8(gb.cpu.a, 0x02);
    assert_eq_u8(gb.cpu.f, 0x00);

    gb_run_until_halt(&gb);
    // Halt 4
    assert_eq_u8(gb.cpu.a, 0xFF);
    assert_eq_u8(gb.cpu.f, 0x70);

    gb_run_until_halt(&gb);
    // Halt 5
    assert_eq_u8(gb.cpu.a, 0xFC);
    assert_eq_u8(gb.cpu.f, 0x40);

    gb_run_until_halt(&gb);
    // Halt 6
    assert_eq_u8(gb.cpu.a, 0xFA);
    assert_eq_u8(gb.cpu.f, 0x40);
}

static void test_interrupt(void) {
    const u8 prog[] = {
        0x3E, 0x02,        // LD A,$02
        0xE0, 0xFF,        // LD $FFFF,A  ; Enable LCD_STAT
        0x31, 0xFF, 0xDF,  // LD SP,$DFFF ; Set SP to end of work RAM
        0xFB,              // EI
        0x00,              // NOP
        0x00,              // NOP
        0x00,              // NOP
        0x76,              // HALT
        0x00,              // NOP
        0x00,              // NOP
        0x00,              // NOP
    };

    GameBoy gb defer(gb_deinit);
    err_exit(gb_init(&gb));
    err_exit(gb_load_bootrom_buffer(&gb, prog, sizeof(prog)));

    gb_run_until_halt(&gb);
    assert_eq_u8(gb.bus.high_byte_ram.data[FF_IE], 0x02);
    log_assert(gb.cpu.ime);

    gb.cpu.halted = false;
    // Trigger interrupt
    gb.bus.high_byte_ram.data[FF_IF] = gb.bus.high_byte_ram.data[FF_IE];

    // Interrupt call is 3 cycles, then check that PC is
    // LCD_STAT -> INT $48
    cpu_cycle(&gb.cpu, &gb.bus);
    cpu_cycle(&gb.cpu, &gb.bus);
    log_assert(gb.cpu.pc != 0x0048);
    cpu_cycle(&gb.cpu, &gb.bus);
    assert_eq_u8(gb.cpu.pc, 0x0048);
}

static void test_blargg(void) {
    GameBoy gb defer(gb_deinit);
    err_exit(gb_init(&gb));

    for (size_t i = 0; i < array_len(blargg_roms); ++i) {
        const char *filename = blargg_roms[i];

        err_exit(gb_load_rom_file(&gb, filename));

        if (!cart_is_valid_header(&gb.bus.cart)) {
            errorf("Invalid ROM header: %s", filename);
        }

        gb_boot_dmg(&gb);

        for (size_t cycle = 0; cycle < BLARGG_MAX_CYCLES; ++cycle) {
            gb_step(&gb);
            if (0 == strcmp("Passed\n", gb.debug_serial_message)) {
                goto test_blarg__next;
            }
            if (0 == strcmp("Failed\n", gb.debug_serial_message)) {
                panicf("Test ROM failed: %s", filename);
            }
        }

        panicf("Test ROM timed out: %s", filename);

    test_blarg__next:
        continue;
    }

    // cart_print_info(&gb.bus.cart, filename);
}

int main(void) {
    test_cart();
    test_microcode_is_valid();
    test_cpu_jp();
    test_cpu_halt();
    test_cpu_ld_xor();
    test_cpu_inc_dec();
    test_cpu_hl();
    test_cpu_arith();
    test_interrupt();

    test_blargg();

    printf("\nAll tests passed!\n");
}
