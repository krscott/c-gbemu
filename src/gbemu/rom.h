#pragma once

#include "common.h"

typedef enum {
    ROM_OK,
    ROM_FILE_ERR,
    ROM_ALLOC_ERR,
} RomLoadErr;

/// @brief Dynamically-sized container of read-only memory
typedef struct {
    size_t size;
    const u8 data[];
} Rom;

/// @brief Load and validate a cart from file and store on the heap. Calls
/// malloc() internally.
/// @param filename The name of the file. Must not be NULL.
/// @return Pointer to a Rom. NULL if cart could not be loaded. Resutling
/// non-NULL pointer must be called with rom_dealloc to free its memory.
const Rom *rom_alloc_from_file(const char *filename, RomLoadErr *err);

/// @brief De-allocate Rom memory. Calls free() internally.
/// @param cart May be null
void rom_dealloc(const Rom *cart);

/// @brief Read data at the given address of the cart ROM.
/// @param cart Must not be NULL
/// @param address
/// @return Cart ROM data if address is in bounds, 0 otherwise.
u8 rom_read(const Rom *cart, u16 address);
