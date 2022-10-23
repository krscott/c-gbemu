#include "gb.h"

GameBoy *gb_alloc_with_cart(const char *cart_filename, RomLoadErr *err) {
    GameBoy *gb = NULL;

    do {
        gb = malloc(sizeof(GameBoy));
        if (!gb) break;
        cpu_init_post_boot_dmg(&gb->cpu);
        bus_init_booted(&gb->bus);

        gb->bus.cart = cart_alloc_from_file(cart_filename, err);
        if (!gb->bus.cart) break;

        gb->bus.work_ram = ram_alloc_blank(WORK_RAM_SIZE);
        if (!gb->bus.work_ram) break;

        gb->bus.high_ram = ram_alloc_blank(HIGH_RAM_SIZE);
        if (!gb->bus.high_ram) break;

        return gb;
    } while (0);

    // Error cleanup
    gb_dealloc(&gb);

    return NULL;
}

void gb_dealloc(GameBoy **gb) {
    if (!*gb) return;

    cart_dealloc(&(*gb)->bus.cart);
    ram_dealloc(&(*gb)->bus.work_ram);
    ram_dealloc(&(*gb)->bus.high_ram);

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