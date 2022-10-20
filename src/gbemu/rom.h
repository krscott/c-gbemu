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
    u8 data[];
} Rom;

const Rom *rom_alloc_blank(size_t size);
const Rom *rom_from_buf(const u8 *data, size_t n);

/// @brief Load and validate a ROM from file and store on the heap. Calls
/// malloc() internally.
/// @param filename The name of the file. Must not be NULL.
/// @return Pointer to a Rom. NULL if ROM could not be loaded. Resutling
/// non-NULL pointer must be called with rom_dealloc to free its memory.
const Rom *rom_alloc_from_file(const char *filename, RomLoadErr *err);

/// @brief De-allocate Rom memory. Calls free() internally.
/// @param rom May be null
void rom_dealloc(const Rom *rom);

/// @brief Read data at the given address of the ROM.
/// @param rom Must not be NULL
/// @param address
/// @return Cart ROM data if address is in bounds, 0 otherwise.
u8 rom_read(const Rom *rom, u16 address);
