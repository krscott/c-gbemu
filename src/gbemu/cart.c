#include "cart.h"

#include <string.h>

#define HEADER_ADDR 0x100
#define MIN_CART_SIZE (HEADER_ADDR + sizeof(CartHeaderView))

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

// TODO: Separate old and new codes, since they don't always line up
// https://gbdev.gg8.se/wiki/articles/Gameboy_ROM_Header_Info#Licensee
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
    [0x33] = "Ocean/Acclaim",
    [0x34] = "Konami",
    [0x35] = "HectorSoft",
    [0x38] = "Capcom",
    [0x39] = "Banpresto",
    [0x3C] = "*Entertainment i",
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
    [0x78] = "t*hq",
    [0x79] = "Accolade",
    [0x7A] = "Triffix Entertainment",
    [0x7C] = "Microprose",
    [0x7F] = "Kemco",
    [0x80] = "Misawa Entertainment",
    [0x83] = "Lozc",
    [0x86] = "Tokuma Shoten Intermedia",
    [0x87] = "Tsukuda Ori*",
    [0x8B] = "Bullet-Proof Software",
    [0x8C] = "Vic Tokai",
    [0x8E] = "Ape",
    [0x8F] = "I'Max",
    [0x91] = "Chunsoft Co.",
    [0x92] = "Video System",
    [0x93] = "Tsubaraya Productions Co.",
    [0x95] = "Varie Corporation",
    [0x96] = "Yonezawa/S'Pal",
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
    [0xCE] = "*Pony Canyon or",
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

void cart_print_info(const Cartridge *cart, const char *filename) {
    const CartHeaderView *header = cart_header(cart);
    assert(header);

    if (filename) printf("Filename : %s\n", filename);

    printf("Title    : %.*s\n", 15, header->title);
    printf("Type     : %s\n", cart_type_name(header));
    printf("ROM Size : %d KB\n", 32 << header->rom_size);
    printf("RAM Size : %2.2X\n", 32 << header->ram_size);
    printf("LIC Code : %2.2X - %s\n", cart_licensee_code(header),
           cart_licensee_name(header));
    printf("ROM Vers : %2.2X\n", header->mask_rom_version);
    printf("Checksum : %2.2X - %s\n", header->checksum,
           cart_is_valid_header(cart) ? "PASSED" : "FAILED");
}

const CartHeaderView *cart_header(const Cartridge *cart) {
    assert(cart);
    if (cart->rom.size < MIN_CART_SIZE) {
        return NULL;
    }
    return (CartHeaderView *)&cart->rom.data[HEADER_ADDR];
}

bool cart_is_valid_header(const Cartridge *cart) {
    assert(cart);

    const CartHeaderView *header = cart_header(cart);
    if (!header) return false;

    u16 chk = 0;
    for (u16 addr = 0x0134; addr <= 0x014C; ++addr) {
        chk = chk - cart->rom.data[addr] - 1;
    }

    return (u8)(chk & 0xff) == header->checksum;
}

GbErr cart_mc_init(Cartridge *cart, size_t ram_size) {
    assert(cart);

    GbErr err = ram_init(&cart->ram, ram_size);
    if (err) return err;

    cart->ram_en = false;
    cart->bank_sel_lower = 0;
    cart->bank_sel_upper = 0;
    cart->bank_mode = 0;

    return OK;
}

GbErr cart_init_none(Cartridge *cart) {
    assert(cart);

    GbErr err;

    do {
        rom_init_none(&cart->rom);

        err = ram_init(&cart->ram, 0);
        if (err) break;

        cart->ram_en = false;
        cart->bank_sel_lower = 0;
        cart->bank_sel_upper = 0;
        cart->bank_mode = 0;

        return OK;
    } while (0);

    cart_deinit(cart);
    return err;
}

GbErr cart_init(Cartridge *cart, const char *filename) {
    assert(cart);

    GbErr err;

    do {
        err = rom_init_from_file(&cart->rom, filename);
        if (err) break;

        // TODO: Set size based on header info
        err = cart_mc_init(cart, CART_MBC1_RAM_SIZE);
        if (err) break;

        return OK;
    } while (0);

    cart_deinit(cart);
    return err;
}

GbErr cart_init_from_buffer(Cartridge *cart, const u8 *buffer, size_t size) {
    assert(cart);

    GbErr err;

    do {
        err = rom_init_from_buffer(&cart->rom, buffer, size);
        if (err) break;

        // TODO: Set size based on header info
        err = cart_mc_init(cart, CART_MBC1_RAM_SIZE);
        if (err) break;

        return OK;
    } while (0);

    cart_deinit(cart);
    return err;
}

void cart_deinit(Cartridge *cart) {
    rom_deinit(&cart->rom);
    ram_deinit(&cart->ram);
}

u8 cart_read(const Cartridge *cart, u16 address) {
    assert(cart);

    size_t internal_address;

    // See diagrams: https://gbdev.io/pandocs/MBC1.html
    // TODO: Other RAM schemes

    // ROM Bank X0
    if (address < 0x4000) {
        if (cart->bank_mode == 0) {
            internal_address = address;
        } else {
            internal_address =
                ((cart->bank_sel_upper & 3) << 19) | (address & 0x3FFF);
        }
        return rom_read(&cart->rom, internal_address % cart->rom.size);
    }

    // ROM Bank 01-7F
    else if (address < 0x8000) {
        size_t lower5 = (cart->bank_sel_lower & 0x1F);
        if (lower5 == 0) lower5 = 1;

        internal_address = ((cart->bank_sel_upper & 3) << 19) | (lower5 << 14) |
                           (address & 0x3FFF);

        return rom_read(&cart->rom, internal_address % cart->rom.size);
    }

    // Invalid - this is VRAM space
    else if (address < 0xA000) {
        // Fallthrough
    }

    // RAM Bank 00-03
    else if (address < 0xC000) {
        if (!cart->ram_en) return 0xFF;

        if (cart->bank_mode == 0) {
            internal_address = address & 0x1FFF;
        } else {
            internal_address =
                ((cart->bank_sel_upper & 3) << 13) | (address & 0x1FFF);
        }
        return ram_read(&cart->ram, internal_address % cart->ram.size);
    }

    assert(false);
    return 0xFF;
}

void cart_write(Cartridge *cart, u16 address, u8 value) {
    assert(cart);

    // RAM Enable
    if (address < 0x2000) {
        cart->ram_en = (value & 0xA) == 0xA;
        return;
    }

    // Lower ROM Bank Number
    else if (address < 0x4000) {
        cart->bank_sel_lower = value & 0x1F;
        return;
    }

    // RAM Bank Number / Upper ROM Bank Number
    else if (address < 0x6000) {
        cart->bank_sel_upper = value & 3;
        return;
    }

    // Invalid - this is VRAM space
    else if (address < 0xA000) {
        // Fallthrough
    }

    // RAM Bank 00-03
    else if (address < 0xC000) {
        if (!cart->ram_en) return;

        size_t internal_address;
        if (cart->bank_mode == 0) {
            internal_address = address & 0x1FFF;
        } else {
            internal_address =
                ((cart->bank_sel_upper & 3) << 13) | (address & 0x1FFF);
        }

        ram_write(&cart->ram, internal_address % cart->ram.size, value);
        return;
    }

    assert(false);
}