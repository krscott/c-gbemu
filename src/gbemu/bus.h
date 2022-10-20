#pragma once

#include "cart.h"
#include "common.h"
#include "rom.h"

typedef struct {
    const Rom *boot;
    const CartRom *cart;

    bool is_bootrom_disabled;
} Bus;

/// @brief Read data from the bus
/// @param bus Must not be NULL
/// @param address
/// @return
u8 bus_read(const Bus *bus, u16 address);

/// @brief Write data to the bus
/// @param bus Must not be NULL
/// @param address
/// @param value
void bus_write(Bus *bus, u16 address, u8 value);
