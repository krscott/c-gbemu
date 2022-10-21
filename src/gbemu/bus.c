#include "bus.h"

#include "cart.h"
#include "rom.h"

u8 _bus_read(const Bus *bus, u16 address) {
    assert(bus);

    if (address < 0x4000 && !bus->is_bootrom_disabled) {
        assert(bus->boot);
        return rom_read(bus->boot, address);
    }

    if (address < 0x8000) {
        if (!bus->cart) {
            errorf("Attempting to read cart at $%04X but no cart inserted",
                   address);
            return 0;
        }

        return cart_read(bus->cart, address);
    }

    // 0x8000..=0x9FFF VRAM
    if (address < 0xA000) {
        panicf("TODO: VRAM $%04X", address);
        return 0;
    }

    // 0xA000..=0xBFFF External RAM
    if (address < 0xC000) {
        panicf("TODO: ExRAM $%04X", address);
        return 0;
    }

    // 0xC000..=0xCFFF Work RAM
    // 0xD000..=0xDFFF Work RAM
    // 0xE000..=0xFDFF Mirror RAM
    if (address < 0xFE00) {
        if (!bus->work_ram) {
            panicf("Attempting to read work RAM at $%04X but it is NULL",
                   address);
            return 0;
        }

        return ram_read(bus->work_ram, (address - 0xC000) % 0x2000);
    }

    panicf("Unmapped addr read: $%04X", address);
    return 0;
}

u8 bus_read(const Bus *bus, u16 address) {
    u8 out = _bus_read(bus, address);
    infof("$%04X R $%02X", address, out);
    return out;
}

void bus_write(Bus *bus, u16 address, u8 value) {
    assert(bus);
    infof("$%04X W $%02X", address, value);

    if (address < 0x8000) {
        panicf("TODO: cart_write $%04X", address);
        return;
    }

    // 0x8000..=0x9FFF VRAM
    if (address < 0xA000) {
        panicf("TODO: VRAM $%04X", address);
        return;
    }

    // 0xA000..=0xBFFF External RAM
    if (address < 0xC000) {
        panicf("TODO: ExRAM $%04X", address);
        return;
    }

    // 0xC000..=0xCFFF Work RAM
    // 0xD000..=0xDFFF Work RAM
    // 0xE000..=0xFDFF Mirror RAM
    if (address < 0xFE00) {
        if (!bus->work_ram) {
            panicf("Attempting to read work RAM at $%04X but it is NULL",
                   address);
            return;
        }

        ram_write(bus->work_ram, (address - 0xC000) % 0x2000, value);
        return;
    }

    panicf("Unmapped addr write: $%04X", address);
}
