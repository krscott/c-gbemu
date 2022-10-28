#pragma once

#include "common.h"

typedef enum LcdMode {
    MODE_HBLANK = 0,
    MODE_VBLANK = 1,
    MODE_OAM = 2,
    MODE_XFER = 3,
} LcdMode;

typedef enum StatSrc {
    SS_HBLANK = 1 << 3,
    SS_VBLANK = 1 << 4,
    SS_OAM = 1 << 5,
    SS_LYC = 1 << 6,
} StatSrc;

typedef enum PaletteSelect {
    PLT_BG,
    PLT_SP1,
    PLT_SP2,
} PaletteSelect;

typedef struct Lcd {
    u8 bg_colors[4];
    u8 sp1_colors[4];
    u8 sp2_colors[4];
} Lcd;

void lcd_set_palette(Lcd *lcd, PaletteSelect select, u8 data);
