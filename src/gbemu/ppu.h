#pragma once

#include "common.h"
#include "pixel_fifo.h"
#include "ram.h"

#define VRAM_SIZE 0x2000
#define OAM_SIZE 0xA0

#define LINES_PER_FRAME 154
#define TICKS_PER_LINE 456
#define RES_Y 144
#define RES_X 160
#define VIDEO_BUFFER_LEN (RES_X * RES_Y)

typedef enum FetcherState {
    FS_TILE,
    FS_DATA0,
    FS_DATA1,
    FS_IDLE,
    FS_PUSH,
} FetcherState;

typedef struct Ppu {
    Ram vram;
    Ram oam;

    u32 frame_count;
    u32 line_ticks;
    u8 *video_buffer;

    PixelFifo fifo;
    FetcherState fetcher_state;
    u8 line_x;
    u8 pushed_x;
    u8 fetch_x;
    u8 bgw_fetch_data[3];
    u8 oam_fetch_data[6];
    u8 map_y;
    u8 map_x;
    u8 tile_y;
    u8 fifo_x;
} Ppu;

GbErr ppu_init(Ppu *ppu) nodiscard;
void ppu_deinit(Ppu *ppu);

u8 ppu_read(const Ppu *ppu, u16 address);
void ppu_write(Ppu *ppu, u16 address, u8 value);
