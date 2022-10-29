#pragma once

#include <stdatomic.h>

#include "bus.h"
#include "cart.h"
#include "cpu.h"
#include "instructions.h"
#include "rom.h"

/// @brief Emulated GameBoy system
typedef struct GameBoy {
    Cpu cpu;
    Bus bus;

    /// @brief Flag indicating if user closed UI to tell emu thread to shutdown.
    /// Does not impact emulation itself.
    _Atomic bool shutdown;

    // Serial message is for debugging Blargg tests.
    // TODO: Move debug_serial* to separate module

    /// @brief enable printing debug serial message
    bool enable_print_debug_serial_message;

    /// @brief Index into debug_serial_message.
    u16 debug_serial_message_index;
    /// @brief Record of transmitted serial data ASCII characters.
    char debug_serial_message[0x100];

} GameBoy;

GbErr gb_init(GameBoy *gb) nodiscard;
void gb_deinit(GameBoy *gb);

/// @brief Copy data from buffer into bootrom
/// @param gb non-NULL
/// @param buffer non-NULL
/// @param size
/// @return GbErr code. 0 is success
GbErr gb_load_bootrom_buffer(GameBoy *gb, const u8 *buffer,
                             size_t size) nodiscard;

/// @brief Copy data from buffer into cartridge
/// @param gb non-NULL
/// @param buffer non-NULL
/// @param size
/// @return GbErr code. 0 is success
GbErr gb_load_rom_buffer(GameBoy *gb, const u8 *buffer, size_t size) nodiscard;

/// @brief Copy data from file into cartridge
/// @param gb non-NULL
/// @param cart_filename non-NULL
/// @return GbErr code. 0 is success
GbErr gb_load_rom_file(GameBoy *gb, const char *cart_filename) nodiscard;

/// @brief Reset the system to the state it would be in after just exited the
/// bootrom
/// @param gb non-NULL
void gb_boot_dmg(GameBoy *gb);

/// @brief Advance the system by one CPU instruction, which is 1-6 cycles or
/// 4-24 clocks.
/// @param gb non-NULL
void gb_step(GameBoy *gb);

/// @brief Run the system until halting. Useful for debugging/testing.
/// @param gb non-NULL
void gb_run_until_halt(GameBoy *gb);

/// @brief Print part of the current state of the system
/// @param gb non-NULL
void gb_print_trace(const GameBoy *gb);

u32 gb_get_frame_count(const GameBoy *gb);
u32 gb_get_frame_count(const GameBoy *gb);