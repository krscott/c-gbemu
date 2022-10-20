#include "bus.h"

#include "cart.h"

struct Bus {
    const CartRom *cart;
};

u8 bus_read(const Bus *bus, u16 address) {
    if (address < 0x8000) {
        return cart_read(bus->cart, address);
    }

    panicf("Unmapped addr read: 0x%04X", address);
    return 0;
}

void bus_write(Bus *bus, u16 address, u8 value) {
    if (address < 0x8000) {
        panic("TODO: cart_write");
        return;
    }

    panicf("Unmapped addr write: 0x%04X", address);
}