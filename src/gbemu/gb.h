#pragma once

#include <stdatomic.h>

#include "bus.h"
#include "cart.h"
#include "cpu.h"
#include "instructions.h"
#include "rom.h"

typedef struct GameBoy {
    Cpu cpu;
    Bus bus;

    /// @brief If true, CPU registers are printed before every instruction
    _Atomic bool trace_cpu_en;

    /// @brief Flag indicating if user closed UI to tell emu thread to shutdown.
    /// Does not impact emulation itself.
    _Atomic bool shutdown;

    // Serial message is for debugging Blargg tests.
    // TODO: Move debug_serial* to separate module

    /// @brief Index into debug_serial_message.
    u16 debug_serial_message_index;
    /// @brief Record of transmitted serial data ASCII characters.
    char debug_serial_message[0x100];
} GameBoy;

GbErr gb_init(GameBoy *gb) nodiscard;
void gb_deinit(GameBoy *gb);

GbErr gb_load_rom_file(GameBoy *gb, const char *cart_filename) nodiscard;
GbErr gb_load_rom_buffer(GameBoy *gb, const u8 *buffer, size_t size) nodiscard;
GbErr gb_load_bootrom_buffer(GameBoy *gb, const u8 *buffer,
                             size_t size) nodiscard;
void gb_boot_dmg(GameBoy *gb);
void gb_step(GameBoy *gb);
void gb_run_until_halt(GameBoy *gb);

void gb_print_trace(const GameBoy *gb);
