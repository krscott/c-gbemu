
#include <SDL2/SDL.h>
#include <stdio.h>

#include "gbemu/gb.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void window(void) {
    // The window we'll be rendering to
    SDL_Window *window = NULL;

    // The surface contained by the window
    SDL_Surface *screenSurface = NULL;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    } else {
        // Create window
        window = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                  SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            printf("Window could not be created! SDL_Error: %s\n",
                   SDL_GetError());
        } else {
            // Get window surface
            screenSurface = SDL_GetWindowSurface(window);

            // Fill the surface pink
            SDL_FillRect(screenSurface, NULL,
                         SDL_MapRGB(screenSurface->format, 0xFF, 0x00, 0xFF));

            // Update the surface
            SDL_UpdateWindowSurface(window);

            // Wait two seconds
            SDL_Delay(2000);
        }
    }

    // Destroy window
    SDL_DestroyWindow(window);

    // Quit SDL subsystems
    SDL_Quit();
}

int main(int argc, char *args[]) {
    char *filename = NULL;

    if (argc == 2) {
        filename = args[1];
    }

    if (!filename) {
        printf("Usage: c-gbemu ROMFILE\n");
        return 1;
    }

    RomLoadErr err;
    GameBoy *gb defer(gb_dealloc) = gb_alloc_with_cart(filename, &err);
    if (err) panicf("Error loading cart: %s", filename);

    cart_print_info(gb->bus.cart, filename);

    gb_run_until_halt(gb);

    // window();

    return 0;
}