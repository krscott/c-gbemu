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

        err = ram_init(&bus->work_ram, WORK_RAM_SIZE);
        if (err) break;

        err = ram_init(&bus->high_ram, HIGH_RAM_SIZE);
        if (err) break;

        return OK;
    } while (0);

    return err;
}

void bus_deinit(Bus *bus) {
    rom_deinit(&bus->boot);
    cart_deinit(&bus->cart);
    ram_deinit(&bus->work_ram);
    ram_deinit(&bus->high_ram);
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
        if (!debug_peek) panicf("TODO: VRAM $%04X", address);
        return 0;
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
        if (!debug_peek) panicf("TODO: OAM $%04X", address);
        return 0;
    }

    // 0xFEA0..=0xFEFF Not Usable
    else if (address < 0xFF00) {
        if (!debug_peek) panicf("Prohibitied memory access $%04X", address);
        return 0;
    }

    // 0xFF00..=0xFF7F I/O Registers
    else if (address < 0xFF80) {
        switch (address) {
            case 0xFF01:
                return bus->reg_sb;
            case 0xFF02:
                return bus->reg_sc;
            case 0xFF04:
                return bus->reg_div;
            case 0xFF05:
                return bus->reg_tima;
            case 0xFF06:
                return bus->reg_tma;
            case 0xFF07:
                return bus->reg_tac;
            case 0xFF0F:
                return bus->reg_if;
        }

        if (address >= 0xFF10 && address <= 0xFF26) {
            // Sound
            // panicf("TODO: Sound $%04X", address);
            return 0;
        }

        if (address >= 0xFF40 && address <= 0xFF4B) {
            // LCD
            // panicf("TODO: LCD $%04X", address);
            return 0;
        }

        if (!debug_peek) panicf("TODO: I/O $%04X", address);
        return 0;
    }

    // 0xFF80..=0xFFFE High RAM
    else if (address < 0xFFFF) {
        u16 internal_address = address - 0xFF80;
        assert(internal_address < HIGH_RAM_SIZE);

        return ram_read(&bus->high_ram, internal_address);
    }

    assert(address == 0xFFFF);
    return bus->reg_ie;
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
        panicf("TODO: cart_write $%04X", address);
        return;
    }

    // 0x8000..=0x9FFF VRAM
    else if (address < 0xA000) {
        // panicf("TODO: VRAM $%04X", address);
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
        panicf("TODO: OAM $%04X", address);
        return;
    }

    // 0xFEA0..=0xFEFF Not Usable
    else if (address < 0xFF00) {
        panicf("Prohibitied memory access $%04X", address);
        return;
    }

    // 0xFF00..=0xFF7F I/O Registers
    else if (address < 0xFF80) {
        switch (address) {
            case 0xFF01:
                bus->reg_sb = value;
                return;
            case 0xFF02:
                bus->reg_sc = value;
                return;
            case 0xFF04:
                // Writing any value to 0xFF04 resets DIV to 0.
                bus->reg_div = 0;
                return;
            case 0xFF05:
                bus->reg_tima = value;
                return;
            case 0xFF06:
                bus->reg_tma = value;
                return;
            case 0xFF07:
                bus->reg_tac = value;
                return;
            case 0xFF0F:
                bus->reg_if = value & 0x1F;
                return;
        }

        if (address >= 0xFF10 && address <= 0xFF26) {
            // Sound
            // panicf("TODO: Sound $%04X", address);
            return;
        }

        if (address >= 0xFF40 && address <= 0xFF4B) {
            // LCD
            // panicf("TODO: LCD $%04X", address);
            return;
        }

        panicf("TODO: I/O $%04X", address);
        return;
    }

    // 0xFF80..=0xFFFE High RAM
    else if (address < 0xFFFF) {
        u16 internal_address = address - 0xFF80;
        assert(internal_address < HIGH_RAM_SIZE);

        ram_write(&bus->high_ram, internal_address, value);
        return;
    }

    assert(address == 0xFFFF);
    bus->reg_ie = value;
}

void bus_cycle(Bus *bus) {
    assert(bus);
    assert(bus->clocks % 4 == 0);

    bus->clocks += 4;

    // Increment DIV
    if (bus->clocks % 256 == 0) ++bus->reg_div;

    bool inc_tima = false;

    // Increment TIMA
    switch (bus->reg_tac % 3) {
        case 0:
            inc_tima = bus->clocks % 1024 == 0;
            break;
        case 1:
            inc_tima = bus->clocks % 16 == 0;
            break;
        case 2:
            inc_tima = bus->clocks % 64 == 0;
            break;
        case 3:
            inc_tima = bus->clocks % 256 == 0;
            break;
        default:
            assert(0);
    }

    if (inc_tima) {
        ++bus->reg_tima;

        // If TIMA overflows, request interrupt and copy TMA to TIMA
        if (bus->reg_tima == 0) {
            bus->reg_if |= INTR_TIMER_MASK;
            bus->reg_tima = bus->reg_tma;
        }
    }
}

bool bus_is_serial_transfer_requested(Bus *bus) {
    return (bus->reg_sc & 0x80) != 0;
}

u8 bus_take_serial_byte(Bus *bus) {
    assert(bus_is_serial_transfer_requested(bus));

    // Clear transfer flag
    bus->reg_sc &= ~0x80;

    // Set serial interrupt request
    bus->reg_if |= INTR_SERIAL_MASK;

    // Return transfered data
    return bus->reg_sb;
}