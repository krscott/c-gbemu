#include "ram.h"

#include <string.h>  // memset

GbErr ram_init(Ram *ram, size_t size) {
    assert(ram);

    if (size > 0) {
        ram->data = malloc(sizeof(*ram->data) * size);
        if (!ram->data) return ERR_ALLOC;

        // An emulated program may access uninitilzed memory, so we need to
        // initialize it. However, it is undefined behavior for them, so we can
        // use any value.
        memset(ram->data, 0, size);
    } else {
        ram->data = NULL;
    }

    ram->size = size;
    return OK;
}

void ram_deinit(Ram *ram) {
    assert(ram);
    free(ram->data);
}

u8 ram_read(const Ram *ram, size_t address) {
    assert(ram);
    assert(ram->data);
    assert(address < ram->size);

    // infof("RAM $%04X R $%02X", address, ram->data[address]);
    return ram->data[address % ram->size];
}

void ram_write(Ram *ram, size_t address, u8 value) {
    assert(ram);
    assert(ram->data);
    assert(address < ram->size);
    // infof("RAM $%04X W $%02X (was $%02X)", address, value,
    //       ram->data[address]);
    ram->data[address % ram->size] = value;
}