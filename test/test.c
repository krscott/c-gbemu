
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

void test_cpu_jp(void) {
    const u8 jp_prog[] = {0xC3, 0xAA, 0xBB};
    const Rom *rom defer(rom_dealloc) = rom_from_buf(jp_prog, sizeof(jp_prog));

    Bus bus = {
        .boot = rom,
    };

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

int main(void) {
    test_cart();
    test_cpu_jp();

    info("All tests passed!");
}