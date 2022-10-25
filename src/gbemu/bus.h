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

typedef enum HighByteAddress {
    FF_JOY = 0x00,
    FF_SB = 0x01,
    FF_SC = 0x02,

    FF_DIV = 0x04,
    FF_TIMA = 0x05,
    FF_TMA = 0x06,
    FF_TAC = 0x07,

    FF_IE = 0x0F,
    FF_NR10 = 0x10,
    FF_NR11 = 0x11,
    FF_NR12 = 0x12,
    FF_NR13 = 0x13,
    FF_NR14 = 0x14,

    FF_NR21 = 0x16,
    FF_NR22 = 0x17,
    FF_NR23 = 0x18,
    FF_NR24 = 0x19,
    FF_NR30 = 0x1A,
    FF_NR31 = 0x1B,
    FF_NR32 = 0x1C,
    FF_NR33 = 0x1D,
    FF_NR34 = 0x1E,

    FF_NR41 = 0x20,
    FF_NR42 = 0x21,
    FF_NR43 = 0x22,
    FF_NR44 = 0x23,
    FF_NR50 = 0x24,
    FF_NR51 = 0x25,
    FF_NR52 = 0x26,

    FF_WAV_0 = 0x30,
    FF_WAV_1 = 0x31,
    FF_WAV_2 = 0x32,
    FF_WAV_3 = 0x33,
    FF_WAV_4 = 0x34,
    FF_WAV_5 = 0x35,
    FF_WAV_6 = 0x36,
    FF_WAV_7 = 0x37,
    FF_WAV_8 = 0x38,
    FF_WAV_9 = 0x39,
    FF_WAV_A = 0x3A,
    FF_WAV_B = 0x3B,
    FF_WAV_C = 0x3C,
    FF_WAV_D = 0x3D,
    FF_WAV_E = 0x3E,
    FF_WAV_F = 0x3F,
    FF_LCDC = 0x40,
    FF_STAT = 0x41,
    FF_SCY = 0x42,
    FF_SCX = 0x43,
    FF_LY = 0x44,
    FF_LYC = 0x45,

    FF_BGP = 0x47,
    FF_OBP0 = 0x48,
    FF_OBP1 = 0x49,
    FF_WY = 0x4A,
    FF_WX = 0x4B,

    FF_KEY1 = 0x4D,  // CGB Only

    FF_VBK = 0x4F,  // CGB Only
    FF_BOOTDIS = 0x50,
    FF_HDMA1 = 0x51,  // CGB Only
    FF_HDMA2 = 0x52,  // CGB Only
    FF_HDMA3 = 0x53,  // CGB Only
    FF_HDMA4 = 0x54,  // CGB Only
    FF_HDMA5 = 0x55,  // CGB Only
    FF_RP = 0x56,     // CGB Only

    FF_BCPS = 0x68,  // CGB Only
    FF_BCPD = 0x69,  // CGB Only
    FF_OCPS = 0x6A,  // CGB Only
    FF_OCPD = 0x6B,  // CGB Only
    FF_OPRI = 0x6C,  // CGB Only

    FF_SVBK = 0x70,  // CGB Only

    FF_72 = 0x72,     // CGB Only (undocumented)
    FF_73 = 0x73,     // CGB Only (undocumented)
    FF_75 = 0x75,     // CGB Only (undocumented)
    FF_PCM12 = 0x76,  // CGB Only (undocumented)
    FF_PCM34 = 0x77,  // CGB Only (undocumented)

    FF_IE = 0xFF,
} HighByteAddress;

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
