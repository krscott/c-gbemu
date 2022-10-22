#pragma once

#include "cart.h"
#include "common.h"
#include "ram.h"
#include "rom.h"

#define WORK_RAM_SIZE 0x2000
#define HIGH_RAM_SIZE 0x007F

typedef struct Bus {
    const Rom *boot;
    const CartRom *cart;
    Ram *work_ram;
    Ram *high_ram;
    u8 ie;

    bool is_bootrom_disabled;
} Bus;

void bus_init_booted(Bus *bus);

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
