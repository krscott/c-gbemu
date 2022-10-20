#include "bus.h"

#include "cart.h"
#include "rom.h"

u8 _bus_read(const Bus *bus, u16 address) {
    assert(bus);

    if (!bus->is_bootrom_disabled) {
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

    panicf("Unmapped addr write: $%04X", address);
}
