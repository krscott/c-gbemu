#include "gb.h"

GameBoy *gb_alloc_with_cart(const char *cart_filename, RomLoadErr *err) {
    const CartRom *cart = cart_alloc_from_file(cart_filename, err);
    if (!cart) return NULL;

    GameBoy *gb = malloc(sizeof(GameBoy));
    cpu_init_post_boot_dmg(&gb->cpu);
    bus_init_booted(&gb->bus);

    Ram *work_ram = ram_alloc_blank(WORK_RAM_SIZE);

    gb->bus.cart = cart;
    gb->bus.work_ram = work_ram;

    return gb;
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
        cpu_cycle(&gb->cpu, &gb->bus);
    }
}