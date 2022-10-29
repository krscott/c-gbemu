#include "ppu.h"

#include <string.h>

#include "bus.h"

GbErr ppu_init(Ppu *ppu) {
    assert(ppu);
    memset(ppu, 0, sizeof(*ppu));

    GbErr err;

    err = ram_init(&ppu->vram, VRAM_SIZE);
    if (err) return err;

    err = ram_init(&ppu->oam, OAM_SIZE);
    if (err) return err;

    ppu->video_buffer = malloc(VIDEO_BUFFER_LEN * sizeof(*ppu->video_buffer));
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
