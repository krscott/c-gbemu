#include "rom.h"

#include <string.h>  // memset

void rom_deinit(Rom *rom) {
    assert(rom);
    free((void *)rom->data);
}

GbemuError rom_init_from_buffer(Rom *rom, const u8 *buffer, size_t size) {
    assert(rom);
    assert(buffer);

    u8 *rom_data = malloc(sizeof(u8) * size);
    if (!rom_data) return ERR_ALLOC;

    memcpy(rom_data, buffer, size);

    rom->size = size;
    rom->data = rom_data;
    return OK;
}

void close_file(FILE **fp) {
    fclose(*fp);
    *fp = NULL;
}

GbemuError rom_init_from_file(Rom *rom, const char *filename) {
    assert(rom);
    assert(filename);

    FILE *file defer(close_file) = fopen(filename, "r");
    if (!file) return ERR_FILE_OPEN;

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);

    u8 *rom_data = malloc(sizeof(u8) * size);
    if (!rom_data) return ERR_ALLOC;

    rewind(file);
    fread(rom_data, size, 1, file);

    rom->size = size;
    rom->data = rom_data;
    return OK;
}

void rom_init_none(Rom *rom) {
    rom->size = 0;
    rom->data = NULL;
}

u8 rom_read(const Rom *rom, size_t address) {
    assert(rom);
    assert(rom->data);
    assert(address < rom->size);
    return rom->data[address % rom->size];
}