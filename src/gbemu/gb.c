#include "gb.h"

GameBoy *gb_alloc_with_cart(const char *cart_filename, RomLoadErr *err) {
    const CartRom *cart = NULL;
    GameBoy *gb = NULL;
    Ram *work_ram = NULL;
    Ram *high_ram = NULL;

    do {
        gb = malloc(sizeof(GameBoy));
        if (!gb) break;
        cpu_init_post_boot_dmg(&gb->cpu);
        bus_init_booted(&gb->bus);

        cart = cart_alloc_from_file(cart_filename, err);
        if (!cart) break;
        gb->bus.cart = cart;

        work_ram = ram_alloc_blank(WORK_RAM_SIZE);
        if (!work_ram) break;
        gb->bus.work_ram = work_ram;

        high_ram = ram_alloc_blank(HIGH_RAM_SIZE);
        if (!high_ram) break;
        gb->bus.high_ram = high_ram;

        return gb;
    } while (0);

    // Error cleanup
    // Ok to pass NULL to these functions
    ram_dealloc(&high_ram);
    ram_dealloc(&work_ram);
    gb_dealloc(&gb);
    cart_dealloc(&cart);

    return NULL;
}

void gb_dealloc(GameBoy **gb) {
    if (!*gb) return;

    cart_dealloc(&(*gb)->bus.cart);
    ram_dealloc(&(*gb)->bus.work_ram);

    free(*gb);
    *gb = NULL;
}

void gb_run_until_halt(GameBoy *gb) {
    gb->cpu.halted = false;
    while (!gb->cpu.halted) {
        cpu_print_trace(&gb->cpu, &gb->bus);
        cpu_cycle(&gb->cpu, &gb->bus);
    }
}