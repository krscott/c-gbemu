#include "bus.h"

#include "cart.h"
#include "rom.h"

struct Bus {
    const Rom *boot;
    const CartRom *cart;
};

u8 bus_read(const Bus *bus, u16 address) {
    assert(bus);

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

void bus_write(Bus *bus, u16 address, u8 value) {
    assert(bus);

    if (address < 0x8000) {
        panicf("TODO: cart_write $%04X", address);
        return;
    }

    panicf("Unmapped addr write: $%04X", address);
}