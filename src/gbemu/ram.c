#include "ram.h"

#include <string.h>  // memset

Ram *ram_alloc_uninit(size_t size) {
    Ram *ram = malloc(sizeof(Ram) + sizeof(u8) * size);
    if (ram) ram->size = size;
    return ram;
}

Ram *ram_alloc_blank(size_t size) {
    Ram *ram = ram_alloc_uninit(size);
    if (ram) memset(ram->data, 0, size);
    return ram;
}

void ram_dealloc(Ram **ram) {
    free(*ram);
    *ram = NULL;
}

u8 ram_read(const Ram *ram, size_t address) {
    assert(ram);
    assert(address < ram->size);

    // infof("RAM $%04X R $%02X", address, ram->data[address]);
    return ram->data[address % ram->size];
}

void ram_write(Ram *ram, size_t address, u8 value) {
    assert(ram);
    assert(address < ram->size);
    // infof("RAM $%04X W $%02X (was $%02X)", address, value,
    //       ram->data[address]);
    ram->data[address % ram->size] = value;
}