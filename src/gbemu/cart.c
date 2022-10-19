#include "cart.h"

#define HEADER_ADDR 0x100
#define MIN_CART_SIZE (HEADER_ADDR + sizeof(CartHeaderView))

CartRom cart_empty()
{
    const CartRom cart = {
        .size = 0,
        .data = NULL,
    };
    return cart;
}

const CartHeaderView *cart_header(const CartRom cart)
{
    if (cart.size == 0)
    {
        return NULL;
    }
    assert(cart.data);
    assert(cart.size >= MIN_CART_SIZE);
    return (CartHeaderView *)&cart.data[HEADER_ADDR];
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

const char *cart_type_name(const CartHeaderView *header)
{
    if (header->cart_type <= 0x22)
    {
        return ROM_TYPES[header->cart_type];
    }
    return "UNKNOWN";
}

// TODO: Add old codes
static const char *LICENSEE_CODE[0xA5] = {
    [0x00] = "None",
    [0x01] = "Nintendo R&D1",
    [0x08] = "Capcom",
    [0x13] = "Electronic Arts",
    [0x18] = "Hudson Soft",
    [0x19] = "b-ai",
    [0x20] = "kss",
    [0x22] = "pow",
    [0x24] = "PCM Complete",
    [0x25] = "san-x",
    [0x28] = "Kemco Japan",
    [0x29] = "seta",
    [0x30] = "Viacom",
    [0x31] = "Nintendo",
    [0x32] = "Bandai",
    [0x33] = "Ocean/Acclaim",
    [0x34] = "Konami",
    [0x35] = "Hector",
    [0x37] = "Taito",
    [0x38] = "Hudson",
    [0x39] = "Banpresto",
    [0x41] = "Ubi Soft",
    [0x42] = "Atlus",
    [0x44] = "Malibu",
    [0x46] = "angel",
    [0x47] = "Bullet-Proof",
    [0x49] = "irem",
    [0x50] = "Absolute",
    [0x51] = "Acclaim",
    [0x52] = "Activision",
    [0x53] = "American sammy",
    [0x54] = "Konami",
    [0x55] = "Hi tech entertainment",
    [0x56] = "LJN",
    [0x57] = "Matchbox",
    [0x58] = "Mattel",
    [0x59] = "Milton Bradley",
    [0x60] = "Titus",
    [0x61] = "Virgin",
    [0x64] = "LucasArts",
    [0x67] = "Ocean",
    [0x69] = "Electronic Arts",
    [0x70] = "Infogrames",
    [0x71] = "Interplay",
    [0x72] = "Broderbund",
    [0x73] = "sculptured",
    [0x75] = "sci",
    [0x78] = "THQ",
    [0x79] = "Accolade",
    [0x80] = "misawa",
    [0x83] = "lozc",
    [0x86] = "Tokuma Shoten Intermedia",
    [0x87] = "Tsukuda Original",
    [0x91] = "Chunsoft",
    [0x92] = "Video system",
    [0x93] = "Ocean/Acclaim",
    [0x95] = "Varie",
    [0x96] = "Yonezawa/s'pal",
    [0x97] = "Kaneko",
    [0x99] = "Pack in soft",
    [0xA4] = "Konami (Yu-Gi-Oh!)"};

const char *cart_licensee_name(const CartHeaderView *header)
{
    if (header->old_licensee_code == 0x33)
    {
        if (header->new_licensee_code <= 0xA4)
        {
            return LICENSEE_CODE[header->new_licensee_code];
        }
    }
    else
    {
        // return LICENSEE_CODE[header->old_licensee_code];
        return "old licensee code";
    }

    return "UNKNOWN";
}

CartRom alloc_cart_from_file(const Allocator allocator, const char *filename, CartLoadErr *err)
{
    assert(filename);

    FILE *file = fopen(filename, "r");

    if (!file)
    {
        errorf("Failed to open file: %s", filename);
        *err = CART_FILE_ERR;
        return cart_empty();
    }

    fseek(file, 0, SEEK_END);
    u32 cart_size = ftell(file);

    if (cart_size < MIN_CART_SIZE)
    {
        fclose(file);
        error("File too small to be valid cart ROM");
        *err = CART_TOO_SMALL;
        return cart_empty();
    }

    // Allocate and read data from file
    u8 *cart_data = (u8 *)allocator.alloc(sizeof(u8) * cart_size);

    if (!cart_data)
    {
        cart_size = 0;
        fclose(file);
        error("Allocation failed");
        *err = CART_FILE_ERR;
        return cart_empty();
    }

    rewind(file);
    fread(cart_data, cart_size, 1, file);
    fclose(file);

    CartRom cart = {
        .size = cart_size,
        .data = cart_data,
    };

    const CartHeaderView *header = cart_header(cart);

    infof("Filename : %s", filename);
    infof("Title    : %.*s", 15, header->title);
    infof("Type     : %s", cart_type_name(header));
    infof("ROM Size : %d KB", 32 << header->rom_size);
    infof("RAM Size : %2.2X", 32 << header->ram_size);
    infof("LIC Code : %2.2X %2.2X - %s",
          header->old_licensee_code,
          header->new_licensee_code,
          cart_licensee_name(header));
    infof("ROM Vers : %2.2X", header->mask_rom_version);
    infof("Checksum : %2.2X - %s",
          header->checksum,
          cart_is_valid_header(cart) ? "PASSED" : "FAILED");

    *err = CART_OK;
    return cart;
}

bool cart_is_valid_header(const CartRom cart)
{
    if (!cart.size || !cart.data)
    {
        return false;
    }

    u16 chk = 0;
    for (u16 addr = 0x0134; addr <= 0x014C; ++addr)
    {
        chk = chk - cart.data[addr] - 1;
    }
    const CartHeaderView *header = cart_header(cart);

    return (u8)(chk & 0xff) == header->checksum;
}