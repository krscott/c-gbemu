#pragma once

#include "common.h"
#include "ram.h"
#include "rom.h"

#define CART_ROM_BANK_SIZE 0x4000
#define CART_RAM_BANK_SIZE 0x2000

// 1 MiB
#define CART_MBC1_RAM_SIZE 0x100000

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

typedef struct Cartridge {
    const Rom *rom;
    Ram *ram;
    bool ram_en;
    u8 bank_sel_lower;
    u8 bank_sel_upper;
    u8 bank_mode;
} Cartridge;

/// @brief Set cartridge to default initialized state
/// @param cart
void cart_init(Cartridge *cart);

/// @brief Load and validate a cart from file and store on the heap. Calls
/// malloc() internally.
/// @param filename The name of the file. Must not be NULL.
/// @param err Pointer to the error code storage. May be NULL.
/// @return Pointer to a Cartridge. NULL if cart could not be loaded. Resutling
/// pointer must be called with cart_dealloc to free its memory.
Cartridge *cart_alloc_from_file(const char *filename, RomLoadErr *err);

/// @brief De-allocate Cartridge memory. Calls free() internally.
/// @param cart May be null
void cart_dealloc(Cartridge **cart);

/// @brief Read data at the given address of the cartridge.
/// @param cart Must not be NULL
/// @param address
/// @return Cart ROM data if address is in bounds, 0 otherwise.
u8 cart_read(const Cartridge *cart, u16 address);

/// @brief Write data at the given address of the cartridge
/// @param cart Must not be NULL
/// @param address
/// @param value
void cart_write(Cartridge *cart, u16 address, u8 value);

/// @brief Print cart info.
/// @param cart The cart ROM data. Must not be null.
/// @param filename Optional. If NULL, filename will not be printed
void cart_print_info(const Cartridge *cart, const char *filename);

/// @brief Get a view of the cart's header data.
/// @param cart Must not be NULL.
/// @return A pointer to CartHeaderView, which has the same lifetime as the
/// given cart data.
const CartHeaderView *cart_header(const Cartridge *cart);

/// @brief Validate cart header checksum.
/// @param cart Must not be null.
/// @return
bool cart_is_valid_header(const Cartridge *cart);
