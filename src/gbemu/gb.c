#include "gb.h"

GbemuError gb_init(GameBoy *gb) {
    assert(gb);

    GbemuError err;

    do {
        cpu_reset(&gb->cpu);

        err = bus_init(&gb->bus);
        if (err) break;

        gb->trace_cpu_en = false;

        return OK;
    } while (0);

    gb_deinit(gb);
    return err;
}

void gb_deinit(GameBoy *gb) { bus_deinit(&gb->bus); }

GbemuError gb_load_rom_file(GameBoy *gb, const char *cart_filename) {
    return bus_load_cart_from_file(&gb->bus, cart_filename);
}

GbemuError gb_load_rom_buffer(GameBoy *gb, const u8 *buffer, size_t size) {
    return bus_load_cart_from_buffer(&gb->bus, buffer, size);
}

GbemuError gb_load_bootrom_buffer(GameBoy *gb, const u8 *buffer, size_t size) {
    return bus_load_bootrom_from_buffer(&gb->bus, buffer, size);
}

void gb_boot_dmg(GameBoy *gb) {
    cpu_reset_boot_dmg(&gb->cpu);
    gb->bus.is_bootrom_disabled = true;
}

void gb_cycle(GameBoy *gb) { cpu_cycle(&gb->cpu, &gb->bus); }

void gb_run_until_halt(GameBoy *gb) {
    gb->cpu.halted = false;
    while (!gb->cpu.halted) {
        if (gb->trace_cpu_en) cpu_print_trace(&gb->cpu, &gb->bus);
        cpu_cycle(&gb->cpu, &gb->bus);
    }
}