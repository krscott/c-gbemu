#pragma once

#include "cart.h"
#include "common.h"
#include "ram.h"
#include "rom.h"

#define WORK_RAM_SIZE 0x2000
#define HIGH_RAM_SIZE 0x007F

typedef enum InterruptMask {
    INTR_VBLANK_MASK = 0x01,
    INTR_LCD_STAT_MASK = 0x02,
    INTR_TIMER_MASK = 0x04,
    INTR_SERIAL_MASK = 0x08,
    INTR_JOYPAD_MASK = 0x10,
} InterruptMask;

typedef struct Bus {
    Rom boot;
    Cartridge cart;
    Ram work_ram;
    Ram high_ram;

    /// @brief Serial transfer data
    u8 reg_sb;
    /// @brief Serial transfer control
    u8 reg_sc;
    /// @brief Interrupt flags
    u8 reg_if;
    /// @brief Interrupt enables
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

bool bus_is_serial_transfer_requested(Bus *bus);
u8 bus_take_serial_byte(Bus *bus);