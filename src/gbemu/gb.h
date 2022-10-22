#pragma once

#include "bus.h"
#include "cart.h"
#include "cpu.h"
#include "instructions.h"
#include "rom.h"

typedef struct GameBoy {
    Cpu cpu;
    Bus bus;
} GameBoy;

GameBoy *gb_alloc_with_cart(const char *filename, RomLoadErr *err);

void gb_dealloc(GameBoy **gb);

void gb_run_until_halt(GameBoy *gb);