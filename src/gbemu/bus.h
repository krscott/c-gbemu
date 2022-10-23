#pragma once

#include "cart.h"
#include "common.h"
#include "ram.h"
#include "rom.h"

#define WORK_RAM_SIZE 0x2000
#define HIGH_RAM_SIZE 0x007F

typedef struct Bus {
    Rom boot;
    Cartridge cart;
    Ram work_ram;
    Ram high_ram;

    u8 reg_if;
    u8 reg_ie;

    bool is_bootrom_disabled;
} Bus;

GbErr bus_init(Bus *bus) nodiscard;
GbErr bus_load_cart_from_file(Bus *bus, const char *cart_filename) nodiscard;
GbErr bus_load_cart_from_buffer(Bus *bus, const u8 *buffer,
                                size_t size) nodiscard;
GbErr bus_load_bootrom_from_buffer(Bus *bus, const u8 *buffer,
                                   size_t size) nodiscard;

void bus_deinit(Bus *bus);

/// @brief Read data from the bus
/// @param bus Must not be NULL
/// @param address
/// @return
u8 bus_read(const Bus *bus, u16 address);

u8 bus_debug_peek(const Bus *bus, u16 address);

/// @brief Write data to the bus
/// @param bus Must not be NULL
/// @param address
/// @param value
void bus_write(Bus *bus, u16 address, u8 value);
