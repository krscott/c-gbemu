#include "rom.h"

#include <string.h>  // memset

Rom *rom_alloc_uninit(size_t size) {
    Rom *rom = malloc(sizeof(Rom) + sizeof(u8) * size);
    if (rom) rom->size = size;
    return rom;
}

const Rom *rom_alloc_blank(size_t size) {
    Rom *rom = rom_alloc_uninit(size);
    if (rom) memset(rom->data, 0, size);
    return rom;
}

const Rom *rom_alloc_from_buf(const u8 *data, size_t n) {
    Rom *rom = rom_alloc_uninit(n);
    if (rom) memcpy(rom->data, data, n / sizeof(data[0]));
    return rom;
}

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

    Rom *rom = rom_alloc_uninit(rom_size);

    if (!rom) {
        fclose(file);
        error("Allocation failed");
        if (err) *err = ROM_FILE_ERR;
        return NULL;
    }

    rewind(file);
    fread(rom->data, rom_size, 1, file);
    fclose(file);

    if (err) *err = ROM_OK;
    return rom;
}

void rom_dealloc(const Rom **rom) {
    free((Rom *)*rom);
    *rom = NULL;
}

u8 rom_read(const Rom *rom, u16 address) {
    assert(rom);

    if (address >= rom->size) {
        return 0;
    }

    return rom->data[address];
}