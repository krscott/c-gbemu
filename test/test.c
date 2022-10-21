
#include "../src/gbemu/bus.h"
#include "../src/gbemu/cart.h"
#include "../src/gbemu/cpu.h"
#include "../src/gbemu/instructions.h"
#include "../src/gbemu/rom.h"

// Make sure NDEBUG was not set during build step to ensure all asserts are
// enabled.
#ifdef NDEBUG
#error "NDEBUG must not be set for testing"
#endif

void test_cart(void) {
    const char *filename = "roms/01-special.gb";

    RomLoadErr err;
    const CartRom *cart defer(cart_dealloc) =
        cart_alloc_from_file(filename, &err);

    assert(err == ROM_OK);
    assert(cart_is_valid_header(cart));

    // Peek some arbitrary memory location
    assert(cart_read(cart, 0x200) == 0x47);
    assert(cart_read(cart, 0x210) == 0xC3);
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
    assert(cpu.opcode == 0xC3);
    assert(cpu.cycle == 1);
    assert(cpu.pc == 1);

    // Read address
    cpu_cycle(&cpu, &bus);
    cpu_cycle(&cpu, &bus);
    assert(cpu.pc == 3);

    // Jump
    cpu_cycle(&cpu, &bus);
    // cpu_print_info(&cpu);
    assert(cpu.pc == 0xBBAA);
}

void test_cpu_halt(void) {
    const u8 prog[] = {
        0x76,  // HALT
    };
    const Rom *rom defer(rom_dealloc) = rom_alloc_from_buf(prog, sizeof(prog));
    Bus bus = {.boot = rom};
    Cpu cpu = {0};

    assert(!cpu.halted);
    cpu_cycle(&cpu, &bus);
    assert(cpu.halted);
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

    assert(cpu.a == 0xA5);
    assert(cpu.f == 0x00);

    cpu.halted = false;
    while (!cpu.halted) {
        cpu_cycle(&cpu, &bus);
    }
    assert(cpu.a == 0x00);
    assert(cpu.f == 0x80);
}

void test_cpu_inc_dec(void) {
    const u8 prog[] = {
        0x05,  // DEC B ; expect B == 0xFF, F == 0x40
        0x05,  // DEC B ; expect B == 0xFE, F == 0x60
        0x04,  // INC B ; expect B == 0xFF, F == 0x00
        0x04,  // INC B ; expect B == 0x00, F == 0xA0
    };
    const Rom *rom defer(rom_dealloc) = rom_alloc_from_buf(prog, sizeof(prog));
    Bus bus = {.boot = rom};
    Cpu cpu = {0};

    cpu_cycle(&cpu, &bus);
    assert(cpu.b == 0xFF);
    assert(cpu.f == 0x40);

    cpu_cycle(&cpu, &bus);
    assert(cpu.b == 0xFE);
    assert(cpu.f == 0x60);

    cpu_cycle(&cpu, &bus);
    assert(cpu.b == 0xFF);
    assert(cpu.f == 0x00);

    cpu_cycle(&cpu, &bus);
    // cpu_print_info(&cpu);
    assert(cpu.b == 0x00);
    assert(cpu.f == 0xA0);
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
    assert(bus_read(&bus, 0xC000) == 0x55);
}

int main(void) {
    test_cart();
    test_microcode_is_valid();
    test_cpu_jp();
    test_cpu_halt();
    test_cpu_ld_xor();
    test_cpu_inc_dec();
    test_cpu_hl();

    printf("\nAll tests passed!\n");
}
