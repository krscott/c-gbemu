#include "bus.h"

#include <string.h>

#include "cart.h"
#include "rom.h"

GbErr bus_init(Bus *bus) {
    assert(bus);

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

        bus->reg_if = 0;
        bus->reg_ie = 0;
        bus->is_bootrom_disabled = false;

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

GbErr bus_load_cart_from_file(Bus *bus, const char *cart_filename) {
    cart_deinit(&bus->cart);
    return cart_init(&bus->cart, cart_filename);
}

GbErr bus_load_cart_from_buffer(Bus *bus, const u8 *buffer, size_t size) {
    cart_deinit(&bus->cart);
    return cart_init_from_buffer(&bus->cart, buffer, size);
}

GbErr bus_load_bootrom_from_buffer(Bus *bus, const u8 *buffer, size_t size) {
    rom_deinit(&bus->boot);
    return rom_init_from_buffer(&bus->boot, buffer, size);
}

u8 bus_read_helper(const Bus *bus, u16 address, bool debug_peek) {
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
        if (!debug_peek) errorf("TODO: VRAM $%04X", address);
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
        if (!debug_peek) errorf("TODO: OAM $%04X", address);
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
            case 0xFF0F:  // Interrupt Flag
                return bus->reg_if;
        }
        if (!debug_peek) errorf("TODO: I/O $%04X", address);
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
        errorf("TODO: VRAM $%04X", address);
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
        errorf("TODO: OAM $%04X", address);
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
            case 0xFF0F:  // Interrupt Flag
                bus->reg_if = value & 0x1F;
                return;
        }
        errorf("TODO: I/O $%04X", address);
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
