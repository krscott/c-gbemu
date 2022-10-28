#include "gb.h"

#include <string.h>

// TODO: Move this to another file
// TODO: use HighByteAddress enum values for addresses
// See table: https://gbdev.io/pandocs/Power_Up_Sequence.html
static const u8 FFXX_DMG[] = {
    [0x00] = 0xCF,  // P1
    [0x01] = 0x00,  // SB
    [0x02] = 0x7E,  // SC

    // Skip DIV--this is derived from bus.internal_timer
    // [0x03] = 0x68,  // DIV_LO (guess based on mooneye)
    // [0x04] = 0xAA,  // DIV (Pandocs: 0xAB, Mooneye: 0xAA)

    [0x05] = 0x00,  // TIMA
    [0x06] = 0x00,  // TMA
    [0x07] = 0xF8,  // TAC
    [0x0F] = 0xE1,  // IF
    [0x10] = 0x80,  // NR10
    [0x11] = 0xBF,  // NR11
    [0x12] = 0xF3,  // NR12
    [0x13] = 0xFF,  // NR13
    [0x14] = 0xBF,  // NR14
    [0x16] = 0x3F,  // NR21
    [0x17] = 0x00,  // NR22
    [0x18] = 0xFF,  // NR23
    [0x19] = 0xBF,  // NR24
    [0x1A] = 0x7F,  // NR30
    [0x1B] = 0xFF,  // NR31
    [0x1C] = 0x9F,  // NR32
    [0x1D] = 0xFF,  // NR33
    [0x1E] = 0xBF,  // NR34
    [0x20] = 0xFF,  // NR41
    [0x21] = 0x00,  // NR42
    [0x22] = 0x00,  // NR43
    [0x23] = 0xBF,  // NR44
    [0x24] = 0x77,  // NR50
    [0x25] = 0xF3,  // NR51
    [0x26] = 0xF1,  // NR52
    [0x40] = 0x91,  // LCDC
    [0x41] = 0x85,  // STAT
    [0x42] = 0x00,  // SCY
    [0x43] = 0x00,  // SCX
    [0x44] = 0x00,  // LY
    [0x45] = 0x00,  // LYC
    [0x46] = 0xFF,  // DMA
    [0x47] = 0xFC,  // BGP
    [0x48] = 0xFF,  // OBP0 *
    [0x49] = 0xFF,  // OBP1 *
    [0x4A] = 0x00,  // WY
    [0x4B] = 0x00,  // WX
    [0x4D] = 0xFF,  // KEY1
    [0x4F] = 0xFF,  // VBK
    [0x51] = 0xFF,  // HDMA1
    [0x52] = 0xFF,  // HDMA2
    [0x53] = 0xFF,  // HDMA3
    [0x54] = 0xFF,  // HDMA4
    [0x55] = 0xFF,  // HDMA5
    [0x56] = 0xFF,  // RP
    [0x68] = 0xFF,  // BCPS
    [0x69] = 0xFF,  // BCPD
    [0x6A] = 0xFF,  // OCPS
    [0x6B] = 0xFF,  // OCPD
    [0x70] = 0xFF,  // SVBK
};

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

    gb->bus.load_tma_scheduled = false;
    // Full DIV counter. Matches mooneye, but different than pandocs?
    gb->bus.internal_timer = 0x2A9A;

    memcpy_s(gb->bus.high_byte_ram.data, gb->bus.high_byte_ram.size, FFXX_DMG,
             sizeof(FFXX_DMG));
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
        bus_cycle(&gb->bus);
        ppu_cycle(&gb->bus.ppu, &gb->bus.high_byte_ram);

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

u32 gb_get_frame_count(const GameBoy *gb) { return gb->bus.ppu.frame_count; }