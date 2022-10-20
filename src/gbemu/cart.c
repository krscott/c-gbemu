#include "cart.h"

#define HEADER_ADDR 0x100
#define MIN_CART_SIZE (HEADER_ADDR + sizeof(CartHeaderView))

struct CartRom {
    u32 size;
    u8 data[];
};

const CartHeaderView *cart_header(const CartRom *cart) {
    assert(cart);
    if (cart->size == 0) {
        return NULL;
    }
    assert(cart->size >= MIN_CART_SIZE);
    return (CartHeaderView *)&cart->data[HEADER_ADDR];
}

static const char *ROM_TYPES[] = {
    "ROM ONLY",
    "MBC1",
    "MBC1+RAM",
    "MBC1+RAM+BATTERY",
    "0x04 ???",
    "MBC2",
    "MBC2+BATTERY",
    "0x07 ???",
    "ROM+RAM 1",
    "ROM+RAM+BATTERY 1",
    "0x0A ???",
    "MMM01",
    "MMM01+RAM",
    "MMM01+RAM+BATTERY",
    "0x0E ???",
    "MBC3+TIMER+BATTERY",
    "MBC3+TIMER+RAM+BATTERY 2",
    "MBC3",
    "MBC3+RAM 2",
    "MBC3+RAM+BATTERY 2",
    "0x14 ???",
    "0x15 ???",
    "0x16 ???",
    "0x17 ???",
    "0x18 ???",
    "MBC5",
    "MBC5+RAM",
    "MBC5+RAM+BATTERY",
    "MBC5+RUMBLE",
    "MBC5+RUMBLE+RAM",
    "MBC5+RUMBLE+RAM+BATTERY",
    "0x1F ???",
    "MBC6",
    "0x21 ???",
    "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
};

const char *cart_type_name(const CartHeaderView *header) {
    if (header->cart_type <= 0x22) {
        return ROM_TYPES[header->cart_type];
    }
    return "UNKNOWN";
}

// TODO: Add old codes
static const char *LICENSEE_CODE[0x100] = {
    [0x00] = "None",
    [0x01] = "Nintendo",
    [0x08] = "Capcom",
    [0x09] = "Hot-B",
    [0x0A] = "Jaleco",
    [0x0B] = "Coconuts Japan",
    [0x0C] = "Elite Systems",
    [0x13] = "EA (Electronic Arts)",
    [0x18] = "Hudsonsoft",
    [0x19] = "ITC Entertainment",
    [0x1A] = "Yanoman",
    [0x1D] = "Japan Clary",
    [0x1F] = "Virgin Interactive",
    [0x24] = "PCM Complete",
    [0x25] = "San-X",
    [0x28] = "Kotobuki Systems",
    [0x29] = "Seta",
    [0x30] = "Infogrames",
    [0x31] = "Nintendo",
    [0x32] = "Bandai",
    [0x33] = "Indicates that the New licensee code should be used instead.",
    [0x34] = "Konami",
    [0x35] = "HectorSoft",
    [0x38] = "Capcom",
    [0x39] = "Banpresto",
    [0x3C] = ".Entertainment i",
    [0x3E] = "Gremlin",
    [0x41] = "Ubisoft",
    [0x42] = "Atlus",
    [0x44] = "Malibu",
    [0x46] = "Angel",
    [0x47] = "Spectrum Holoby",
    [0x49] = "Irem",
    [0x4A] = "Virgin Interactive",
    [0x4D] = "Malibu",
    [0x4F] = "U.S. Gold",
    [0x50] = "Absolute",
    [0x51] = "Acclaim",
    [0x52] = "Activision",
    [0x53] = "American Sammy",
    [0x54] = "GameTek",
    [0x55] = "Park Place",
    [0x56] = "LJN",
    [0x57] = "Matchbox",
    [0x59] = "Milton Bradley",
    [0x5A] = "Mindscape",
    [0x5B] = "Romstar",
    [0x5C] = "Naxat Soft",
    [0x5D] = "Tradewest",
    [0x60] = "Titus",
    [0x61] = "Virgin Interactive",
    [0x67] = "Ocean Interactive",
    [0x69] = "EA (Electronic Arts)",
    [0x6E] = "Elite Systems",
    [0x6F] = "Electro Brain",
    [0x70] = "Infogrames",
    [0x71] = "Interplay",
    [0x72] = "Broderbund",
    [0x73] = "Sculptered Soft",
    [0x75] = "The Sales Curve",
    [0x78] = "t.hq",
    [0x79] = "Accolade",
    [0x7A] = "Triffix Entertainment",
    [0x7C] = "Microprose",
    [0x7F] = "Kemco",
    [0x80] = "Misawa Entertainment",
    [0x83] = "Lozc",
    [0x86] = "Tokuma Shoten Intermedia",
    [0x8B] = "Bullet-Proof Software",
    [0x8C] = "Vic Tokai",
    [0x8E] = "Ape",
    [0x8F] = "I’Max",
    [0x91] = "Chunsoft Co.",
    [0x92] = "Video System",
    [0x93] = "Tsubaraya Productions Co.",
    [0x95] = "Varie Corporation",
    [0x96] = "Yonezawa/S’Pal",
    [0x97] = "Kaneko",
    [0x99] = "Arc",
    [0x9A] = "Nihon Bussan",
    [0x9B] = "Tecmo",
    [0x9C] = "Imagineer",
    [0x9D] = "Banpresto",
    [0x9F] = "Nova",
    [0xA1] = "Hori Electric",
    [0xA2] = "Bandai",
    [0xA4] = "Konami",
    [0xA6] = "Kawada",
    [0xA7] = "Takara",
    [0xA9] = "Technos Japan",
    [0xAA] = "Broderbund",
    [0xAC] = "Toei Animation",
    [0xAD] = "Toho",
    [0xAF] = "Namco",
    [0xB0] = "acclaim",
    [0xB1] = "ASCII or Nexsoft",
    [0xB2] = "Bandai",
    [0xB4] = "Square Enix",
    [0xB6] = "HAL Laboratory",
    [0xB7] = "SNK",
    [0xB9] = "Pony Canyon",
    [0xBA] = "Culture Brain",
    [0xBB] = "Sunsoft",
    [0xBD] = "Sony Imagesoft",
    [0xBF] = "Sammy",
    [0xC0] = "Taito",
    [0xC2] = "Kemco",
    [0xC3] = "Squaresoft",
    [0xC4] = "Tokuma Shoten Intermedia",
    [0xC5] = "Data East",
    [0xC6] = "Tonkinhouse",
    [0xC8] = "Koei",
    [0xC9] = "UFL",
    [0xCA] = "Ultra",
    [0xCB] = "Vap",
    [0xCC] = "Use Corporation",
    [0xCD] = "Meldac",
    [0xCE] = ".Pony Canyon or",
    [0xCF] = "Angel",
    [0xD0] = "Taito",
    [0xD1] = "Sofel",
    [0xD2] = "Quest",
    [0xD3] = "Sigma Enterprises",
    [0xD4] = "ASK Kodansha Co.",
    [0xD6] = "Naxat Soft",
    [0xD7] = "Copya System",
    [0xD9] = "Banpresto",
    [0xDA] = "Tomy",
    [0xDB] = "LJN",
    [0xDD] = "NCS",
    [0xDE] = "Human",
    [0xDF] = "Altron",
    [0xE0] = "Jaleco",
    [0xE1] = "Towa Chiki",
    [0xE2] = "Yutaka",
    [0xE3] = "Varie",
    [0xE5] = "Epcoh",
    [0xE7] = "Athena",
    [0xE8] = "Asmik ACE Entertainment",
    [0xE9] = "Natsume",
    [0xEA] = "King Records",
    [0xEB] = "Atlus",
    [0xEC] = "Epic/Sony Records",
    [0xEE] = "IGS",
    [0xF0] = "A Wave",
    [0xF3] = "Extreme Entertainment",
    [0xFF] = "LJN",
};

u8 cart_licensee_code(const CartHeaderView *header) {
    if (header->old_licensee_code == 0x33) {
        const char lic_ascii[] = {header->new_licensee_code[0],
                                  header->new_licensee_code[1], '\0'};

        return atoi(lic_ascii);
    }

    return header->old_licensee_code;
}

const char *cart_licensee_name(const CartHeaderView *header) {
    u8 code = cart_licensee_code(header);
    return LICENSEE_CODE[code];
}

const CartRom *cart_alloc_from_file(const char *filename, CartLoadErr *err) {
    assert(filename);

    FILE *file = fopen(filename, "r");

    if (!file) {
        errorf("Failed to open file: %s", filename);
        if (err) *err = CART_FILE_ERR;
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    u32 cart_size = ftell(file);

    if (cart_size < MIN_CART_SIZE) {
        fclose(file);
        error("File too small to be valid cart ROM");
        if (err) *err = CART_TOO_SMALL;
        return NULL;
    }

    // Allocate and read data from file
    CartRom *cart = malloc(sizeof(CartRom) + sizeof(u8) * cart_size);

    if (!cart) {
        fclose(file);
        error("Allocation failed");
        if (err) *err = CART_FILE_ERR;
        return NULL;
    }

    cart->size = cart_size;

    rewind(file);
    fread(cart->data, cart_size, 1, file);
    fclose(file);

    const CartHeaderView *header = cart_header(cart);
    assert(header);

    infof("Filename : %s", filename);
    infof("Title    : %.*s", 15, header->title);
    infof("Type     : %s", cart_type_name(header));
    infof("ROM Size : %d KB", 32 << header->rom_size);
    infof("RAM Size : %2.2X", 32 << header->ram_size);
    infof("LIC Code : %2.2X - %s", cart_licensee_code(header),
          cart_licensee_name(header));
    infof("ROM Vers : %2.2X", header->mask_rom_version);
    infof("Checksum : %2.2X - %s", header->checksum,
          cart_is_valid_header(cart) ? "PASSED" : "FAILED");

    if (err) *err = CART_OK;
    return cart;
}

void cart_dealloc(const CartRom *cart) { free((void *)cart); }

bool cart_is_valid_header(const CartRom *cart) {
    assert(cart);
    if (!cart->size) {
        return false;
    }

    u16 chk = 0;
    for (u16 addr = 0x0134; addr <= 0x014C; ++addr) {
        chk = chk - cart->data[addr] - 1;
    }
    const CartHeaderView *header = cart_header(cart);

    return (u8)(chk & 0xff) == header->checksum;
}

u8 cart_read(const CartRom *cart, u16 address) {
    assert(cart);
    return cart->data[address];
}