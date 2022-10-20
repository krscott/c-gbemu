
#include "../src/gbemu/cart.h"
#include "../src/gbemu/rom.h"

// Make sure NDEBUG was not set during build step to ensure all asserts are
// enabled.
#ifdef NDEBUG
#error "NDEBUG must not be set for testing"
#endif

void test_cart(void) {
    const char *filename = "roms/01-special.gb";

    RomLoadErr err;
    const CartRom *cart = cart_alloc_from_file(filename, &err);
    defer(cart_dealloc(cart), cart = NULL) {
        assert(err == ROM_OK);
        assert(cart_is_valid_header(cart));

        // Peek some arbitrary memory location
        assert(cart_read(cart, 0x200) == 0x47);
        assert(cart_read(cart, 0x210) == 0xC3);
    }

    // Check that calling cart_dealloc(NULL) is ok.
    assert(!cart);
    cart_dealloc(cart);
}

void test_cpu(void) {}

int main(void) {
    test_cart();

    info("All tests passed!");
}