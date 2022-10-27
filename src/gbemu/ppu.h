#pragma once

#include "common.h"
#include "ram.h"

#define VRAM_SIZE 0x2000
#define OAM_SIZE 0xA0

typedef struct Ppu {
    Ram vram;
    Ram oam;
} Ppu;

GbErr ppu_init(Ppu *ppu);
void ppu_deinit(Ppu *ppu);

u8 ppu_read(const Ppu *ppu, u16 address);
void ppu_write(Ppu *ppu, u16 address, u8 value);