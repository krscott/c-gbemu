#pragma once

#include "common.h"

/// @brief Container of variable-size read-only memory
typedef struct Rom {
    size_t size;
    const u8 *data;
} Rom;

GbemuError rom_init_from_buffer(Rom *rom, const u8 *buffer, size_t n) nodiscard;
GbemuError rom_init_from_file(Rom *rom, const char *filename) nodiscard;
void rom_init_none(Rom *rom);

void rom_deinit(Rom *rom);

/// @brief Read data at the given address of the ROM.
/// @param rom Must not be NULL
/// @param address
/// @return Cart ROM data if address is in bounds, 0 otherwise.
u8 rom_read(const Rom *rom, size_t address);
