#pragma once

#include "common.h"
#include "ram.h"
#include "rom.h"

#define CART_ROM_BANK_SIZE 0x4000
#define CART_RAM_BANK_SIZE 0x2000

// 1 MiB
#define CART_MBC1_RAM_SIZE 0x100000

/// @brief A view into header data of the cartridge.
/// Details: https://gbdev.io/pandocs/The_Cartridge_Header.html
typedef struct CartHeaderView {
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

/// @brief A game cartridge. Contains ROM, RAM, and memory bank control
/// registers.
typedef struct Cartridge {
    Rom rom;
    Ram ram;
    bool ram_en;
    u8 bank_sel_lower;
    u8 bank_sel_upper;
    u8 bank_mode;
} Cartridge;

/// @brief Initialize cartridge and allocate ROM data from file
/// @param cart non-NULL
/// @param filename The name of the file. Must not be NULL.
/// @return Error code
GbErr cart_init(Cartridge *cart, const char *filename) nodiscard;

/// @brief Initialize cartridge without ROM data
/// @param cart non-NULL
/// @return Error code
GbErr cart_init_none(Cartridge *cart) nodiscard;

/// @brief Initialize cartridge by copying data from given buffer
/// @param cart non-NULL
/// @param buffer
/// @param size
/// @return Error code
GbErr cart_init_from_buffer(Cartridge *cart, const u8 *buffer,
                            size_t size) nodiscard;

/// @brief Deinitialize cartridge and free child data
/// @param cart non-NULL
void cart_deinit(Cartridge *cart);

/// @brief Read data at the given address of the cartridge.
/// @param cart non-NULL
/// @param address
/// @return Cart ROM data if address is in bounds, 0 otherwise.
u8 cart_read(const Cartridge *cart, u16 address);

/// @brief Write data at the given address of the cartridge
/// @param cart non-NULL
/// @param address
/// @param value
void cart_write(Cartridge *cart, u16 address, u8 value);

/// @brief Print cart info.
/// @param cart non-NULL
/// @param filename Optional. If NULL, filename will not be printed
void cart_print_info(const Cartridge *cart, const char *filename);

/// @brief Get a view of the cart's header data.
/// @param cart non-NULL
/// @return A pointer to CartHeaderView, which has the same lifetime as the
/// given cart data.
const CartHeaderView *cart_header(const Cartridge *cart);

/// @brief Validate cart header checksum.
/// @param cart non-NULL
/// @return
bool cart_is_valid_header(const Cartridge *cart);
