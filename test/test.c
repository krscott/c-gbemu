
#include "../src/gbemu/gb.h"

// Make sure NDEBUG was not set during build step to ensure all asserts are
// enabled.
#ifdef NDEBUG
#error "NDEBUG must not be set for testing"
#endif

#define ASSERT(cond)                               \
    do {                                           \
        if (!cond) panic("Assert failed: " #cond); \
    } while (0)
#define ASSERT_EQ(a, b, fmt)                                                 \
    do {                                                                     \
        if (a != b)                                                          \
            panicf("Assert failed: " #a " (" fmt ") == " #b " (" fmt ")", a, \
                   b);                                                       \
    } while (0)
#define ASSERT_EQ_U8(a, b) ASSERT_EQ(a, b, "0x%02X")

void test_cart(void) {
    const char *filename = "roms/01-special.gb";

    RomLoadErr err;
    GameBoy *gb defer(gb_dealloc) = gb_alloc_with_cart(filename, &err);
    if (err) panicf("Error loading cart: %s", filename);
    ASSERT_EQ_U8(err, ROM_OK);

    const CartRom *cart = gb->bus.cart;
    ASSERT(cart);
    ASSERT(cart_is_valid_header(cart));

    // Peek some arbitrary memory location
    ASSERT_EQ_U8(cart_read(cart, 0x200), 0x47);
    ASSERT_EQ_U8(cart_read(cart, 0x210), 0xC3);
}

void test_microcode_is_valid(void) {
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

            if (instructions_is_last_ustep(opcode, ustep)) {
                break;
            }
        }
    }
}

void test_cpu_jp(void) {
    const u8 prog[] = {
        0xC3, 0xAA, 0xBB,  // JP $BBAA
    };
    const Rom *rom defer(rom_dealloc) = rom_alloc_from_buf(prog, sizeof(prog));
    Bus bus = {.boot = rom};
    Cpu cpu = {0};

    // Read opcode
    cpu_cycle(&cpu, &bus);
    ASSERT_EQ_U8(cpu.opcode, 0xC3);
    ASSERT_EQ_U8(cpu.cycle, 1);
    ASSERT_EQ_U8(cpu.pc, 1);

    // Read address
    cpu_cycle(&cpu, &bus);
    cpu_cycle(&cpu, &bus);
    ASSERT_EQ_U8(cpu.pc, 3);

    // Jump
    cpu_cycle(&cpu, &bus);
    // cpu_print_info(&cpu);
    ASSERT_EQ_U8(cpu.pc, 0xBBAA);
}

void test_cpu_halt(void) {
    const u8 prog[] = {
        0x76,  // HALT
    };
    const Rom *rom defer(rom_dealloc) = rom_alloc_from_buf(prog, sizeof(prog));
    Bus bus = {.boot = rom};
    Cpu cpu = {0};

    ASSERT(!cpu.halted);
    cpu_cycle(&cpu, &bus);
    ASSERT(cpu.halted);
}

void test_cpu_ld_xor(void) {
    const u8 prog[] = {
        0x3E, 0x55,  // LD A,$55
        0x06, 0xF0,  // LD B,$F0
        0xA8,        // XOR B     ; expect A == 0xA5, F == 0x00
        0x76,        // HALT
        0xA8,        // XOR B     ; expect A == 0x55, F == 0x00
        0xAF,        // XOR A     ; expect A == 0x00, F == 0x80
        0x76,        // HALT
    };
    const Rom *rom defer(rom_dealloc) = rom_alloc_from_buf(prog, sizeof(prog));
    Bus bus = {.boot = rom};
    Cpu cpu = {0};

    while (!cpu.halted) {
        cpu_cycle(&cpu, &bus);
    }

    ASSERT_EQ_U8(cpu.a, 0xA5);
    ASSERT_EQ_U8(cpu.f, 0x00);

    cpu.halted = false;
    while (!cpu.halted) {
        cpu_cycle(&cpu, &bus);
    }
    ASSERT_EQ_U8(cpu.a, 0x00);
    ASSERT_EQ_U8(cpu.f, 0x80);
}

void test_cpu_inc_dec(void) {
    const u8 prog[] = {
        0x05,  // DEC B ; expect B == 0xFF, F == 0x60
        0x05,  // DEC B ; expect B == 0xFE, F == 0x60
        0x04,  // INC B ; expect B == 0xFF, F == 0x00
        0x04,  // INC B ; expect B == 0x00, F == 0xA0
    };
    const Rom *rom defer(rom_dealloc) = rom_alloc_from_buf(prog, sizeof(prog));
    Bus bus = {.boot = rom};
    Cpu cpu = {0};

    cpu_cycle(&cpu, &bus);
    ASSERT_EQ_U8(cpu.b, 0xFF);
    ASSERT_EQ_U8(cpu.f, 0x60);

    cpu_cycle(&cpu, &bus);
    ASSERT_EQ_U8(cpu.b, 0xFE);
    ASSERT_EQ_U8(cpu.f, 0x60);

    cpu_cycle(&cpu, &bus);
    ASSERT_EQ_U8(cpu.b, 0xFF);
    ASSERT_EQ_U8(cpu.f, 0x00);

    cpu_cycle(&cpu, &bus);
    // cpu_print_info(&cpu);
    ASSERT_EQ_U8(cpu.b, 0x00);
    ASSERT_EQ_U8(cpu.f, 0xA0);
}

void test_cpu_hl(void) {
    const u8 prog[] = {
        0x2E, 0x00,  // LD L,$00
        0x26, 0xC0,  // LD H,$C0
        0x3E, 0x55,  // LD A,$55
        0x77,        // LD (HL),A
        0x76,        // HALT
    };
    const Rom *rom defer(rom_dealloc) = rom_alloc_from_buf(prog, sizeof(prog));
    Ram *work_ram defer(ram_dealloc) = ram_alloc_blank(WORK_RAM_SIZE);
    Bus bus = {.boot = rom, .work_ram = work_ram};
    Cpu cpu = {0};

    while (!cpu.halted) {
        cpu_cycle(&cpu, &bus);
    }
    ASSERT_EQ_U8(bus_read(&bus, 0xC000), 0x55);
}

void test_cpu_arith(void) {
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
        0x90,        // SUB A,B  ; A: $FF, F: 0100
        0x76,        // HALT     ; #4
                     //
        0x98,        // SBC A,B  ; A: $FD, F: 0111
        0x76,        // HALT     ; #5
                     //
        0x98,        // SBC A,B  ; A: $FA, F: 0111
        0x76,        // HALT     ; #6
    };
    const Rom *rom defer(rom_dealloc) = rom_alloc_from_buf(prog, sizeof(prog));
    Ram *work_ram defer(ram_dealloc) = ram_alloc_blank(WORK_RAM_SIZE);
    Bus bus = {.boot = rom, .work_ram = work_ram};
    Cpu cpu = {0};

    while (!cpu.halted) {
        cpu_cycle(&cpu, &bus);
    }
    // Halt 1
    ASSERT_EQ_U8(cpu.a, 0x03);
    ASSERT_EQ_U8(cpu.f, 0x00);

    cpu.halted = false;
    while (!cpu.halted) {
        cpu_cycle(&cpu, &bus);
    }
    // Halt 2
    ASSERT_EQ_U8(cpu.a, 0x00);
    ASSERT_EQ_U8(cpu.f, 0xB0);

    cpu.halted = false;
    while (!cpu.halted) {
        cpu_cycle(&cpu, &bus);
    }
    // Halt 3
    ASSERT_EQ_U8(cpu.a, 0x02);
    ASSERT_EQ_U8(cpu.f, 0x00);

    cpu.halted = false;
    while (!cpu.halted) {
        cpu_cycle(&cpu, &bus);
    }
    // Halt 4
    ASSERT_EQ_U8(cpu.a, 0xFF);
    ASSERT_EQ_U8(cpu.f, 0x40);

    cpu.halted = false;
    while (!cpu.halted) {
        cpu_cycle(&cpu, &bus);
    }
    // Halt 5
    ASSERT_EQ_U8(cpu.a, 0xFD);
    ASSERT_EQ_U8(cpu.f, 0x70);

    cpu.halted = false;
    while (!cpu.halted) {
        cpu_cycle(&cpu, &bus);
    }
    // Halt 6
    ASSERT_EQ_U8(cpu.a, 0xFA);
    ASSERT_EQ_U8(cpu.f, 0x70);
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

    printf("\nAll tests passed!\n");
}
