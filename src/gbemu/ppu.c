#include "ppu.h"

GbErr ppu_init(Ppu *ppu) {
    assert(ppu);

    GbErr err;
    do {
        err = ram_init(&ppu->vram, VRAM_SIZE);
        if (err) break;

        err = ram_init(&ppu->oam, OAM_SIZE);
        if (err) break;

        return OK;
    } while (0);

    return err;
}

void ppu_deinit(Ppu *ppu) {
    if (!ppu) return;
    ram_deinit(&ppu->vram);
    ram_deinit(&ppu->oam);
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