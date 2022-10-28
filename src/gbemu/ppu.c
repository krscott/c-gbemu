#include "ppu.h"

#include "bus.h"

GbErr ppu_init(Ppu *ppu) {
    assert(ppu);

    ppu->frame_count = 0;
    ppu->line_ticks = 0;

    GbErr err;

    err = ram_init(&ppu->vram, VRAM_SIZE);
    if (err) return err;

    err = ram_init(&ppu->oam, OAM_SIZE);
    if (err) return err;

    ppu->video_buffer = malloc(RES_X * RES_Y * sizeof(*ppu->video_buffer));
    if (!ppu->video_buffer) return ERR_ALLOC;

    return OK;
}

void ppu_deinit(Ppu *ppu) {
    if (!ppu) return;
    ram_deinit(&ppu->vram);
    ram_deinit(&ppu->oam);
    free(ppu->video_buffer);
}

u8 ppu_read(const Ppu *ppu, u16 address) {
    assert(ppu);

    if (address >= 0x8000 && address <= 0x9FFF) {
        return ram_read(&ppu->vram, address - 0x8000);
    }

    if (address >= 0xFE00 && address <= 0xFE9F) {
        return ram_read(&ppu->oam, address - 0xFE00);
    }

    // Not valid PPU address
    assert(0);
    return 0xFF;
}

void ppu_write(Ppu *ppu, u16 address, u8 value) {
    assert(ppu);

    if (address >= 0x8000 && address <= 0x9FFF) {
        ram_write(&ppu->vram, address - 0x8000, value);
        return;
    }

    if (address >= 0xFE00 && address <= 0xFE9F) {
        ram_write(&ppu->oam, address - 0xFE00, value);
        return;
    }

    // Not valid PPU address
    assert(0);
}

/// Set STAT Mode flag (bits 0 and 1)
static void set_stat_mode(u8 *reg, LcdMode mode) {
    reg[FF_STAT] &= ~3;
    reg[FF_STAT] |= mode & 3;
}

static void inc_ly_and_compare(u8 *reg) {
    ++reg[FF_LY];

    if (reg[FF_LY] == reg[FF_LYC]) {
        // Set STAT LYC=LY flag (bit 2)
        reg[FF_STAT] |= 4;

        // If LYC interrupt is enabled, request STAT interrupt
        if (reg[FF_STAT] & SS_LYC) {
            reg[FF_IF] |= INTR_LCD_STAT_MASK;
        }
    } else {
        // Clear STAT LYC=LY flag (bit 2)
        reg[FF_STAT] &= ~4;
    }
}

static void ppu_tick(Ppu *ppu, Ram *hwreg) {
    assert(hwreg->size == 0x100);
    u8 *reg = hwreg->data;

    // Pixel FIFO state machine

    ++ppu->line_ticks;

    switch (reg[FF_STAT] & 3) {
        case MODE_OAM:
            if (ppu->line_ticks >= 80) {
                set_stat_mode(reg, MODE_XFER);
            }
            break;

        case MODE_XFER:
            if (ppu->line_ticks >= 80 + 172) {
                set_stat_mode(reg, MODE_HBLANK);
            }
            break;

        case MODE_HBLANK:
            if (ppu->line_ticks >= TICKS_PER_LINE) {
                inc_ly_and_compare(reg);

                if (reg[FF_LY] >= RES_Y) {
                    // Request VBLANK interrupt
                    reg[FF_IF] |= INTR_VBLANK_MASK;

                    // If VBlank Interrupt is enabled, request STAT interrupt
                    if (reg[FF_STAT] & SS_VBLANK) {
                        reg[FF_IF] |= INTR_LCD_STAT_MASK;
                    }

                    // Next frame
                    ++ppu->frame_count;

                    set_stat_mode(reg, MODE_VBLANK);
                } else {
                    set_stat_mode(reg, MODE_OAM);
                }
            }
            break;

        case MODE_VBLANK:
            if (ppu->line_ticks >= TICKS_PER_LINE) {
                inc_ly_and_compare(reg);

                if (reg[FF_LY] >= LINES_PER_FRAME) {
                    reg[FF_LY] = 0;
                    set_stat_mode(reg, MODE_OAM);
                }
            }
            break;

        default:
            assert(0);
    }
}

void ppu_cycle(Ppu *ppu, Ram *hwreg) {
    // 4 clocks per cycle
    ppu_tick(ppu, hwreg);
    ppu_tick(ppu, hwreg);
    ppu_tick(ppu, hwreg);
    ppu_tick(ppu, hwreg);
}
