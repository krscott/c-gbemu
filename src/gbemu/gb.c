#include "gb.h"

GameBoy *gb_alloc_with_cart(const char *cart_filename, RomLoadErr *err) {
    const CartRom *cart = NULL;
    GameBoy *gb = NULL;
    Ram *work_ram = NULL;

    cart = cart_alloc_from_file(cart_filename, err);
    if (!cart) goto gb_alloc_with_cart__error;

    gb = malloc(sizeof(GameBoy));
    if (!gb) goto gb_alloc_with_cart__error;

    cpu_init_post_boot_dmg(&gb->cpu);
    bus_init_booted(&gb->bus);

    work_ram = ram_alloc_blank(WORK_RAM_SIZE);
    if (!work_ram) goto gb_alloc_with_cart__error;

    gb->bus.cart = cart;
    gb->bus.work_ram = work_ram;

    return gb;

gb_alloc_with_cart__error:
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