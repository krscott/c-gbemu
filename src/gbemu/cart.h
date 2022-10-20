#pragma once

#include "common.h"

// https://gbdev.io/pandocs/The_Cartridge_Header.html

typedef struct
{
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
    const u16 global_checksum;
} CartHeaderView;

typedef enum
{
    CART_OK,
    CART_FILE_ERR,
    CART_ALLOC_ERR,
    CART_TOO_SMALL,
} CartLoadErr;

typedef struct CartRom CartRom;

const CartHeaderView *cart_header(const CartRom *cart);
bool cart_is_valid_header(const CartRom *cart);

const CartRom *cart_alloc_from_file(const char *filename, CartLoadErr *err);
void cart_dealloc(const CartRom *cart);

u8 cart_read(const CartRom *cart, u16 address);
