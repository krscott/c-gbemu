#include "bus.h"

#include <string.h>

#include "cart.h"
#include "rom.h"

GbErr bus_init(Bus *bus) {
    assert(bus);
    memset(bus, 0, sizeof(*bus));

    GbErr err;

    do {
        // Already booted, so don't need to allocate ROM
        rom_init_none(&bus->boot);

        err = cart_init_none(&bus->cart);
        if (err) break;

        err = ppu_init(&bus->ppu);
        if (err) break;

        err = ram_init(&bus->work_ram, WORK_RAM_SIZE);
        if (err) break;

        err = ram_init(&bus->high_byte_ram, HIGH_BYTE_RAM_SIZE);
        if (err) break;

        return OK;
    } while (0);

    return err;
}

void bus_deinit(Bus *bus) {
    rom_deinit(&bus->boot);
    cart_deinit(&bus->cart);
    ppu_deinit(&bus->ppu);
    ram_deinit(&bus->work_ram);
    ram_deinit(&bus->high_byte_ram);
}

GbErr bus_load_bootrom_from_buffer(Bus *bus, const u8 *buffer, size_t size) {
    rom_deinit(&bus->boot);
    return rom_init_from_buffer(&bus->boot, buffer, size);
}

GbErr bus_load_cart_from_buffer(Bus *bus, const u8 *buffer, size_t size) {
    cart_deinit(&bus->cart);
    return cart_init_from_buffer(&bus->cart, buffer, size);
}

GbErr bus_load_cart_from_file(Bus *bus, const char *cart_filename) {
    cart_deinit(&bus->cart);
    return cart_init(&bus->cart, cart_filename);
}

static u8 bus_read_helper(const Bus *bus, u16 address, bool debug_peek) {
    assert(bus);

    // 0x0000..=0x3FFF Boot ROM bank 0
    if (address < 0x4000 && !bus->is_bootrom_disabled) {
        return rom_read(&bus->boot, address);
    }

    // 0x0000..=0x3FFF Cart ROM bank 0
    // 0x4000..=0x7FFF Cart ROM bank N
    else if (address < 0x8000) {
        return cart_read(&bus->cart, address);
    }

    // 0x8000..=0x9FFF VRAM
    else if (address < 0xA000) {
        return ppu_read(&bus->ppu, address);
    }

    // 0xA000..=0xBFFF External RAM
    else if (address < 0xC000) {
        u16 internal_address = address - 0xA000;
        assert(internal_address < CART_RAM_BANK_SIZE);

        return ram_read(&bus->work_ram, internal_address);
    }

    // 0xC000..=0xCFFF Work RAM
    // 0xD000..=0xDFFF Work RAM
    // 0xE000..=0xFDFF Mirror RAM
    else if (address < 0xFE00) {
        if (!debug_peek && address >= 0xE000) {
            errorf("Attempting to read Mirror RAM at $%04X", address);
        }

        // Modulus to "reflect" mirror RAM
        return ram_read(&bus->work_ram, (address - 0xC000) % WORK_RAM_SIZE);
    }

    // 0xFE00..=FE9F Sprite attribute table (OAM)
    else if (address < 0xFEA0) {
        return ppu_read(&bus->ppu, address);
    }

    // 0xFEA0..=0xFEFF Not Usable
    else if (address < 0xFF00) {
        if (!debug_peek) panicf("Prohibitied memory access $%04X", address);
        return 0;
    }

    // 0xFF00..=0xFFFF Hardware registers and High RAM
    assert(address >= 0xFF00);

    // Treat the whole block as RAM, and handle behaviors as needed.
    u8 ff_address = address & 0xFF;

    switch (ff_address) {
        case FF_DIV_LO:
            if (debug_peek) return bus->internal_timer << 2;
            return 0xFF;

        case FF_DIV:
            return bus->internal_timer >> 6;
    }

    return ram_read(&bus->high_byte_ram, ff_address);
}

u8 bus_read(const Bus *bus, u16 address) {
    u8 out = bus_read_helper(bus, address, false);
    // printf("  %04X R $%02X\n", address, out);
    return out;
}

u8 bus_debug_peek(const Bus *bus, u16 address) {
    return bus_read_helper(bus, address, true);
}

void bus_write(Bus *bus, u16 address, u8 value) {
    assert(bus);
    // printf("  %04X W $%02X\n", address, value);

    // 0x0000..=0x3FFF Cart ROM bank 0
    // 0x4000..=0x7FFF Cart ROM bank N
    if (address < 0x8000) {
        cart_write(&bus->cart, address, value);
        return;
    }

    // 0x8000..=0x9FFF VRAM
    else if (address < 0xA000) {
        ppu_write(&bus->ppu, address, value);
        return;
    }

    // 0xA000..=0xBFFF External RAM
    else if (address < 0xC000) {
        u16 internal_address = address - 0xA000;
        assert(internal_address < CART_RAM_BANK_SIZE);

        ram_write(&bus->work_ram, internal_address, value);
        return;
    }

    // 0xC000..=0xCFFF Work RAM
    // 0xD000..=0xDFFF Work RAM
    // 0xE000..=0xFDFF Mirror RAM
    else if (address < 0xFE00) {
        if (address >= 0xE000) {
            errorf("Attempting to write Mirror RAM at $%04X", address);
        }

        // Modulus to "reflect" mirror RAM
        ram_write(&bus->work_ram, (address - 0xC000) % 0x2000, value);
        return;
    }

    // 0xFE00..=FE9F Sprite attribute table (OAM)
    else if (address < 0xFEA0) {
        ppu_write(&bus->ppu, address, value);
        return;
    }

    // 0xFEA0..=0xFEFF Not Usable
    else if (address < 0xFF00) {
        panicf("Prohibitied memory access $%04X", address);
        return;
    }

    // 0xFF00..=0xFFFF Hardware registers and High RAM
    assert(address >= 0xFF00);

    // Treat the whole block as RAM, and handle behaviors as needed.
    u8 ff_address = address & 0xFF;

    switch (ff_address) {
        case FF_IF:
            value |= 0xE0;
            break;

        case FF_DIV_LO:
            return;  // Skip write--DIV is handled by Bus.internal_timer

        case FF_DIV:
            // Any value resets DIV to 0.
            bus->internal_timer = 0;
            return;  // Skip write--DIV is handled by Bus.internal_timer

        case FF_TAC:
            value |= 0xF8;
            break;

        case FF_BOOTDIS:
            // Non-zero value disables bootrom
            if (value) bus->is_bootrom_disabled = true;
            break;
    }

    ram_write(&bus->high_byte_ram, ff_address, value);
    return;
}

void bus_cycle(Bus *bus) {
    assert(bus);

    u8 *const ram = bus->high_byte_ram.data;

    if (bus->load_tma_scheduled) {
        bus->load_tma_scheduled = false;
        ram[FF_IF] |= INTR_TIMER_MASK;
        ram[FF_TIMA] = ram[FF_TMA];
    }

    // Increment timer 1 cycle
    ++bus->internal_timer;

    // DIV counts clocks, and there are 4 clocks per cycle
    u16 div = bus->internal_timer * 4;

    // TAC bit 2 enables TIMA
    if ((ram[FF_TAC] & 4) != 0) {
        // Increment TIMA
        bool inc_tima;
        switch (ram[FF_TAC] % 3) {
            case 0:
                inc_tima = div % 1024 == 0;
                break;
            case 1:
                inc_tima = div % 16 == 0;
                break;
            case 2:
                inc_tima = div % 64 == 0;
                break;
            case 3:
                inc_tima = div % 256 == 0;
                break;
            default:
                assert(0);
        }

        if (inc_tima) {
            ++ram[FF_TIMA];

            // If TIMA overflows, request interrupt and copy TMA to TIMA
            if (ram[FF_TIMA] == 0) {
                bus->load_tma_scheduled = true;
            }
        }
    }
}

bool bus_is_serial_transfer_requested(Bus *bus) {
    return (bus->high_byte_ram.data[FF_SC] & 0x80) != 0;
}

u8 bus_take_serial_byte(Bus *bus) {
    assert(bus_is_serial_transfer_requested(bus));

    // Clear transfer flag
    bus->high_byte_ram.data[FF_SC] &= ~0x80;

    // Set serial interrupt request
    bus->high_byte_ram.data[FF_IF] |= INTR_SERIAL_MASK;

    // Return transfered data
    return bus->high_byte_ram.data[FF_SB];
}