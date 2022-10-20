#include "rom.h"

const Rom *rom_alloc_from_file(const char *filename, RomLoadErr *err) {
    assert(filename);

    FILE *file = fopen(filename, "r");

    if (!file) {
        errorf("Failed to open file: %s", filename);
        if (err) *err = ROM_FILE_ERR;
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t rom_size = ftell(file);

    // Allocate and read data from file
    Rom *cart = malloc(sizeof(Rom) + sizeof(u8) * rom_size);

    if (!cart) {
        fclose(file);
        error("Allocation failed");
        if (err) *err = ROM_FILE_ERR;
        return NULL;
    }

    cart->size = rom_size;

    rewind(file);
    fread(cart->data, rom_size, 1, file);
    fclose(file);

    if (err) *err = ROM_OK;
    return cart;
}

void rom_dealloc(const Rom *cart) { free((void *)cart); }

u8 rom_read(const Rom *cart, u16 address) {
    assert(cart);

    if (address >= cart->size) {
        return 0;
    }

    return cart->data[address];
}