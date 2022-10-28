#pragma once

#include "common.h"
#include "ram.h"

#define VRAM_SIZE 0x2000
#define OAM_SIZE 0xA0

#define LINES_PER_FRAME 154
#define TICKS_PER_LINE 456
#define RES_Y 144
#define RES_X 160

typedef struct Ppu {
    Ram vram;
    Ram oam;

    u32 frame_count;
    u32 line_ticks;
    u8 *video_buffer;
} Ppu;

GbErr ppu_init(Ppu *ppu) nodiscard;
void ppu_deinit(Ppu *ppu);

u8 ppu_read(const Ppu *ppu, u16 address);
void ppu_write(Ppu *ppu, u16 address, u8 value);

void ppu_cycle(Ppu *ppu, Ram *hwreg);