#pragma once

#include "common.h"
#include "rom.h"

// https://gbdev.io/pandocs/The_Cartridge_Header.html

typedef struct {
    const u8 entry[4];
    const u8 logo[0x30];
    const char title[16];
    const char new_licensee_code[2];
    const u8 sgb;
    const u8 cart_type;
    const u8 rom_size;
    const u8 ram_size;
    const u8 destination_code;
    const u8 old_licensee_code;
    const u8 mask_rom_version;
    const u8 checksum;
    const u8 global_checksum[2];
} CartHeaderView;
static_assert(sizeof(CartHeaderView) == 0x50);

/// @brief Byte-compatible wrapper of Rom struct.
typedef struct {
    Rom rom;
} CartRom;

/// @brief Get a view of the cart's header data.
/// @param cart Must not be NULL.
/// @return A pointer to CartHeaderView, which has the same lifetime as the
/// given cart data.
const CartHeaderView *cart_header(const CartRom *cart);

/// @brief Validate cart header checksum.
/// @param cart Must not be null.
/// @return
bool cart_is_valid_header(const CartRom *cart);

/// @brief Load and validate a cart from file and store on the heap. Calls
/// malloc() internally.
/// @param filename The name of the file. Must not be NULL.
/// @param err Pointer to the error code storage. May be NULL.
/// @return Pointer to a CartRom. NULL if cart could not be loaded. Resutling
/// pointer must be called with cart_dealloc to free its memory.
const CartRom *cart_alloc_from_file(const char *filename, RomLoadErr *err);

/// @brief De-allocate CartRom memory. Calls free() internally.
/// @param cart May be null
void cart_dealloc(const CartRom *cart);

/// @brief Read data at the given address of the cart ROM.
/// @param cart Must not be NULL
/// @param address
/// @return Cart ROM data if address is in bounds, 0 otherwise.
u8 cart_read(const CartRom *cart, u16 address);
