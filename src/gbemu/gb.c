#include "gb.h"

#include <string.h>

GbErr gb_init(GameBoy *gb) {
    assert(gb);

    GbErr err;

    do {
        cpu_reset(&gb->cpu);

        err = bus_init(&gb->bus);
        if (err) break;

        gb->shutdown = false;

        gb->debug_serial_message_index = 0;
        memset(gb->debug_serial_message, 0, sizeof(gb->debug_serial_message));

        return OK;
    } while (0);

    gb_deinit(gb);
    return err;
}

void gb_deinit(GameBoy *gb) {
    assert(gb);
    bus_deinit(&gb->bus);
}

GbErr gb_load_rom_file(GameBoy *gb, const char *cart_filename) {
    assert(gb);
    assert(cart_filename);
    return bus_load_cart_from_file(&gb->bus, cart_filename);
}

GbErr gb_load_rom_buffer(GameBoy *gb, const u8 *buffer, size_t size) {
    assert(gb);
    return bus_load_cart_from_buffer(&gb->bus, buffer, size);
}

GbErr gb_load_bootrom_buffer(GameBoy *gb, const u8 *buffer, size_t size) {
    assert(gb);
    return bus_load_bootrom_from_buffer(&gb->bus, buffer, size);
}

void gb_boot_dmg(GameBoy *gb) {
    assert(gb);
    cpu_reset(&gb->cpu);

    gb->debug_serial_message_index = 0;
    gb->debug_serial_message[0] = '\0';

    const CartHeaderView *header = cart_header(&gb->bus.cart);

    // If checksum is 0, H and C are both clear. Otherwise, both set.
    // (Z is always set)
    gb->cpu.f = header->checksum == 0 ? 0x80 : 0xB0;

    gb->cpu.a = 0x01;
    gb->cpu.b = 0x00;
    gb->cpu.c = 0x13;
    gb->cpu.d = 0x00;
    gb->cpu.e = 0xD8;
    gb->cpu.h = 0x01;
    gb->cpu.l = 0x4D;
    gb->cpu.pc = 0x0100;
    gb->cpu.sp = 0xFFFE;

    gb->bus.is_bootrom_disabled = true;
}

static void gb_update_serial_message_buffer(GameBoy *gb) {
    if (bus_is_serial_transfer_requested(&gb->bus)) {
        char c = bus_take_serial_byte(&gb->bus);

        if (c) {
            // Make sure we have room for null terminator
            assert(gb->debug_serial_message_index <
                   array_len(gb->debug_serial_message) - 1);

            // Add character to message buffer
            gb->debug_serial_message[gb->debug_serial_message_index++] = c;
            gb->debug_serial_message[gb->debug_serial_message_index] = '\0';

            // If newline, or buffer is full, then flush buffer
            if (c == '\n' || gb->debug_serial_message_index ==
                                 array_len(gb->debug_serial_message) - 1) {
                // Skip empty lines
                if (gb->debug_serial_message_index > 1) {
                    // Print message
                    printf("SB> %s", gb->debug_serial_message);
                }

                // Reset index
                gb->debug_serial_message_index = 0;
            }

            // Check we didn't violate in-range invariant
            assert(gb->debug_serial_message_index <
                   array_len(gb->debug_serial_message) - 1);
        }
    }
}

void gb_step(GameBoy *gb) {
    assert(gb);
    do {
        cpu_cycle(&gb->cpu, &gb->bus);

        gb_update_serial_message_buffer(gb);
    } while (gb->cpu.ucode_step != 0);
}

void gb_run_until_halt(GameBoy *gb) {
    assert(gb);
    gb->cpu.halted = false;
    while (!gb->cpu.halted) {
        gb_step(gb);
    }
}

void gb_print_trace(const GameBoy *gb) {
    assert(gb);
    cpu_print_trace(&gb->cpu, &gb->bus);
}