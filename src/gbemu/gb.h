#pragma once

#include "bus.h"
#include "cart.h"
#include "cpu.h"
#include "instructions.h"
#include "rom.h"

typedef struct GameBoy {
    Cpu cpu;
    Bus bus;
    bool trace_cpu_en;
} GameBoy;

GbemuError gb_init(GameBoy *gb) nodiscard;
void gb_deinit(GameBoy *gb);

GbemuError gb_load_rom_file(GameBoy *gb, const char *cart_filename) nodiscard;
GbemuError gb_load_rom_buffer(GameBoy *gb, const u8 *buffer,
                              size_t size) nodiscard;
GbemuError gb_load_bootrom_buffer(GameBoy *gb, const u8 *buffer,
                                  size_t size) nodiscard;
void gb_boot_dmg(GameBoy *gb);
void gb_cycle(GameBoy *gb);
void gb_run_until_halt(GameBoy *gb);
