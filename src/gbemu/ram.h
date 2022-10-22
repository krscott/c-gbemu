#pragma once

#include "common.h"

/// @brief Dynamically-sized container of read/write memory
typedef struct Ram {
    size_t size;
    u8 data[];
} Ram;

Ram *ram_alloc_blank(size_t size);

/// @brief De-allocate Ram memory. Calls free() internally.
/// @param ram May be null
void ram_dealloc(Ram **ram);

/// @brief Read data at the given address of the RAM.
/// @param ram Must not be NULL
/// @param address
/// @return Cart RAM data if address is in bounds, 0 otherwise.
u8 ram_read(const Ram *ram, u16 address);

/// @brief Write data to the given address of the RAM
/// @param ram Must not be NULL
/// @param address Must be less than RAM size
/// @param value
void ram_write(Ram *ram, u16 address, u8 value);