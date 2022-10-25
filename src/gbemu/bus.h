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

    /// @brief Serial transfer data (0xFF01)
    u8 reg_sb;
    /// @brief Serial transfer control (0xFF02)
    u8 reg_sc;
    /// @brief Divider register (0xFF04)
    u8 reg_div;
    /// @brief Timer counter (0xFF05)
    u8 reg_tima;
    /// @brief Timer modulo (0xFF06)
    u8 reg_tma;
    /// @brief Timer control (0xFF07)
    u8 reg_tac;
    /// @brief Interrupt flags (0xFF0F)
    u8 reg_if;
    /// @brief Interrupt enables (0xFFFF)
    u8 reg_ie;

    u16 clocks;
    bool is_bootrom_disabled;
} Bus;

GbErr bus_init(Bus *bus) nodiscard;
void bus_deinit(Bus *bus);

/// @brief Copy data from buffer into bootrom
/// @param buffer non-NULL
/// @param size
/// @return GbErr code. 0 is success
GbErr bus_load_bootrom_from_buffer(Bus *bus, const u8 *buffer,
                                   size_t size) nodiscard;

/// @brief Copy data from buffer into cartridge
/// @param buffer non-NULL
/// @param size
/// @return GbErr code. 0 is success
GbErr bus_load_cart_from_buffer(Bus *bus, const u8 *buffer,
                                size_t size) nodiscard;

/// @brief Copy data from file into cartridge
/// @param bus non-NULL
/// @param cart_filename non-NULL
/// @return GbErr code. 0 is success
GbErr bus_load_cart_from_file(Bus *bus, const char *cart_filename) nodiscard;

/// @brief Read data from the bus
/// @param bus non-NULL
/// @param address
/// @return byte at the given address
u8 bus_read(const Bus *bus, u16 address);

/// @brief Read data from bus for debug purposes. i.e. No side-effects or
/// errors.
/// @param bus non-NULL
/// @param address
/// @return byte at the given address
u8 bus_debug_peek(const Bus *bus, u16 address);

/// @brief Write a byte to the bus
/// @param bus non-NULL
/// @param address
/// @param value
void bus_write(Bus *bus, u16 address, u8 value);

/// @brief Advance clock by 4 and update timers accordingly.
/// @param bus non-NULL
void bus_cycle(Bus *bus);

/// @brief Check if a byte is ready to be taken by bus_take_serial_byte()
/// @param bus non-NULL
/// @return true if ready
bool bus_is_serial_transfer_requested(Bus *bus);

/// @brief Take a byte from the serial buffer. Check that
/// bus_is_serial_transfer_requested() is true before calling this function.
/// @param bus non-NULL
/// @return
u8 bus_take_serial_byte(Bus *bus);
