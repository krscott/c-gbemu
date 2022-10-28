#include "lcd.h"

void lcd_set_palette(Lcd *lcd, PaletteSelect select, u8 data) {
    u8 *colors;

    switch (select) {
        case PLT_BG:
            colors = lcd->bg_colors;
            break;
        case PLT_SP1:
            colors = lcd->sp1_colors;
            // Remove transparency bits
            data &= ~3;
            break;
        case PLT_SP2:
            colors = lcd->sp2_colors;
            // Remove transparency bits
            data &= ~3;
            break;

        default:
            panicf("Unhandled PaletteSelect: %d", select);
            return;
    }

    colors[0] = data & 3;
    colors[1] = (data >> 2) & 3;
    colors[2] = (data >> 4) & 3;
    colors[3] = (data >> 6) & 3;
}