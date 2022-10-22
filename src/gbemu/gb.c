#include "gb.h"

GameBoy *gb_alloc_with_cart(const char *cart_filename, RomLoadErr *err) {
    const CartRom *cart = cart_alloc_from_file(cart_filename, err);
    if (!cart) goto alloc_cart_failed;

    GameBoy *gb = malloc(sizeof(GameBoy));
    if (!gb) goto alloc_gb_failed;

    cpu_init_post_boot_dmg(&gb->cpu);
    bus_init_booted(&gb->bus);

    Ram *work_ram = ram_alloc_blank(WORK_RAM_SIZE);
    if (!work_ram) goto alloc_ram_failed;

    gb->bus.cart = cart;
    gb->bus.work_ram = work_ram;

    return gb;

alloc_ram_failed:
    gb_dealloc(&gb);

alloc_gb_failed:
    cart_dealloc(&cart);

alloc_cart_failed:
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
        cpu_cycle(&gb->cpu, &gb->bus);
    }
}