
#include "gbemu/cart.h"

// Make sure NDEBUG was not set during build step to ensure all asserts are enabled.
#ifdef NDEBUG
#error "NDEBUG must not be set for testing"
#endif

void test_cart()
{
    // Load cart from file
    {
        const char *filename = "roms/01-special.gb";

        CartLoadErr err;
        const CartRom *cart = cart_alloc_from_file(filename, &err);

        assert(err == CART_OK);
        assert(cart_is_valid_header(cart));

        cart_dealloc(cart);
    }
}

int main(void)
{
    test_cart();

    info("All tests passed!");
}