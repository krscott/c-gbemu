#pragma once

#include "common.h"

/// @brief Container of variable-size read/write memory
typedef struct Ram {
    size_t size;
    u8 *data;
} Ram;

GbErr ram_init(Ram *ram, size_t size) nodiscard;
void ram_deinit(Ram *ram);

/// @brief Read data at the given address of the RAM.
/// @param ram Must not be NULL
/// @param address
/// @return Cart RAM data if address is in bounds, 0 otherwise.
u8 ram_read(const Ram *ram, size_t address);

/// @brief Write data to the given address of the RAM
/// @param ram Must not be NULL
/// @param address Must be less than RAM size
/// @param value
void ram_write(Ram *ram, size_t address, u8 value);