#include "ppu_pipeline.h"

// Note: Variable `hwr` stands for "Hardware Registers", which is shorthand for
// `bus->high_byte_ram.data`

/// Set STAT Mode flag (bits 0 and 1)
static void set_stat_mode(u8 *hwr, LcdMode mode) {
    hwr[FF_STAT] &= ~3;
    hwr[FF_STAT] |= mode & 3;
}

static void inc_ly_and_compare(u8 *hwr) {
    ++hwr[FF_LY];

    if (hwr[FF_LY] == hwr[FF_LYC]) {
        // Set STAT LYC=LY flag (bit 2)
        hwr[FF_STAT] |= 4;

        // If LYC interrupt is enabled, request STAT interrupt
        if (hwr[FF_STAT] & SS_LYC) {
            hwr[FF_IF] |= INTR_LCD_STAT_MASK;
        }
    } else {
        // Clear STAT LYC=LY flag (bit 2)
        hwr[FF_STAT] &= ~4;
    }
}

static u8 read_bg_map_area(Bus *bus, u16 offset) {
    u8 *hwr = bus->high_byte_ram.data;

    // BG tile map area is determined by LCDC.3
    u16 start = getbit(hwr[FF_LCDC], 3) ? 0x9C00 : 0x9800;

    return bus_peek(bus, start + offset);
}

static u8 read_bgw_map_area(Bus *bus, u16 offset) {
    u8 *hwr = bus->high_byte_ram.data;

    // BGW tile map area is determined by LCDC.4
    u16 start = getbit(hwr[FF_LCDC], 4) ? 0x8000 : 0x8800;

    return bus_peek(bus, start + offset);
}

static bool pipeline_fifo_add(Bus *bus) {
    Ppu *ppu = &bus->ppu;
    u8 *hwr = bus->high_byte_ram.data;

    // Check if FIFO is full
    if (ppu->fifo.size > 8) return false;

    // Offset fetch x by the scroll x position
    int x = ppu->fetch_x - (8 - (hwr[FF_SCX] % 8));

    for (int i = 0; i < 8; ++i) {
        int bit = 7 - i;
        u8 lo = getbit(ppu->bgw_fetch_data[1], bit) ? maskbit(0) : 0;
        u8 hi = getbit(ppu->bgw_fetch_data[2], bit) ? maskbit(1) : 0;
        u8 color_index = hi | lo;
        assert(color_index < 4);

        if (x >= 0) {
            pixelfifo_push(&ppu->fifo, color_index);
            ++ppu->fifo_x;
        }
    }

    // Pixel was added successfully
    return true;
}

static void pipeline_fetch(Bus *bus) {
    Ppu *ppu = &bus->ppu;
    u8 *hwr = bus->high_byte_ram.data;

    switch (ppu->fetcher_state) {
        case FS_TILE: {
            // Check if BGW is enabled LCDC.0
            if (getbit(hwr[FF_LCDC], 0)) {
                ppu->bgw_fetch_data[0] = read_bg_map_area(
                    bus, ppu->map_x / 8 + (ppu->map_y / 8) * 32);

                // Check BGW data area LCDC.4
                if (!getbit(hwr[FF_LCDC], 4)) {
                    // Increment ID for tile
                    ppu->bgw_fetch_data[0] += 128;
                }
            }

            ppu->fetcher_state = FS_DATA0;
            ppu->fetch_x += 8;
            break;
        }
        case FS_DATA0: {
            ppu->bgw_fetch_data[1] = read_bgw_map_area(
                bus, ppu->bgw_fetch_data[0] * 16 + ppu->tile_y);

            ppu->fetcher_state = FS_DATA1;
            break;
        }
        case FS_DATA1: {
            ppu->bgw_fetch_data[2] = read_bgw_map_area(
                bus, ppu->bgw_fetch_data[0] * 16 + ppu->tile_y + 1);

            ppu->fetcher_state = FS_IDLE;
            break;
        }
        case FS_IDLE: {
            ppu->fetcher_state = FS_PUSH;
            break;
        }
        case FS_PUSH: {
            if (pipeline_fifo_add(bus)) {
                ppu->fetcher_state = FS_TILE;
            }
            break;
        }
        default:
            assert(0);
    }
}

static void pipeline_push_pixel(Bus *bus) {
    Ppu *ppu = &bus->ppu;
    u8 *hwr = bus->high_byte_ram.data;

    if (ppu->fifo.size > 8) {
        // FIFO is full

        u8 pixel = pixelfifo_pop(&ppu->fifo);

        if (ppu->line_x > (hwr[FF_SCX] % 8)) {
            ppu->video_buffer[ppu->pushed_x + (hwr[FF_LY] * RES_X)] = pixel;

            ++ppu->pushed_x;
        }

        ++ppu->line_x;
    }
}

static void pipeline_process(Bus *bus) {
    Ppu *ppu = &bus->ppu;
    u8 *hwr = bus->high_byte_ram.data;

    ppu->map_y = hwr[FF_LY] + hwr[FF_SCY];
    ppu->map_x = ppu->fetch_x + hwr[FF_SCX];
    ppu->tile_y = ((hwr[FF_LY] + hwr[FF_SCY]) % 8) * 2;

    // Update fetcher state on even ticks
    if ((ppu->line_ticks & 1) == 0) {
        pipeline_fetch(bus);
    }

    pipeline_push_pixel(bus);
}

static void pipeline_reset(Bus *bus) {
    Ppu *ppu = &bus->ppu;

    pixelfifo_zero(&ppu->fifo);
}

static void bus_ppu_tick(Bus *bus) {
    Ppu *ppu = &bus->ppu;
    u8 *hwr = bus->high_byte_ram.data;

    // Pixel FIFO state machine

    ++ppu->line_ticks;

    switch (hwr[FF_STAT] & 3) {
        case MODE_OAM:
            if (ppu->line_ticks >= 80) {
                ppu->fetcher_state = FS_TILE;
                ppu->line_x = 0;
                ppu->fetch_x = 0;
                ppu->pushed_x = 0;
                ppu->fifo_x = 0;

                set_stat_mode(hwr, MODE_XFER);
            }
            break;

        case MODE_XFER:
            pipeline_process(bus);

            if (ppu->pushed_x >= RES_X) {
                pipeline_reset(bus);

                // IF HBlank Interrupt is enabled, request STAT interrupt
                if (hwr[FF_STAT] & SS_HBLANK) {
                    hwr[FF_IF] |= INTR_LCD_STAT_MASK;
                }

                set_stat_mode(hwr, MODE_HBLANK);
            }
            break;

        case MODE_HBLANK:
            if (ppu->line_ticks >= TICKS_PER_LINE) {
                inc_ly_and_compare(hwr);

                if (hwr[FF_LY] >= RES_Y) {
                    // Request VBLANK interrupt
                    hwr[FF_IF] |= INTR_VBLANK_MASK;

                    // If VBlank Interrupt is enabled, request STAT interrupt
                    if (hwr[FF_STAT] & SS_VBLANK) {
                        hwr[FF_IF] |= INTR_LCD_STAT_MASK;
                    }

                    // Next frame
                    ++ppu->frame_count;

                    set_stat_mode(hwr, MODE_VBLANK);
                } else {
                    set_stat_mode(hwr, MODE_OAM);
                }

                ppu->line_ticks = 0;
            }
            break;

        case MODE_VBLANK:
            if (ppu->line_ticks >= TICKS_PER_LINE) {
                inc_ly_and_compare(hwr);

                if (hwr[FF_LY] >= LINES_PER_FRAME) {
                    hwr[FF_LY] = 0;

                    set_stat_mode(hwr, MODE_OAM);
                }
            }
            break;

        default:
            assert(0);
    }
}

void bus_ppu_cycle(Bus *bus) {
    // 4 clocks per cycle
    bus_ppu_tick(bus);
    bus_ppu_tick(bus);
    bus_ppu_tick(bus);
    bus_ppu_tick(bus);
}