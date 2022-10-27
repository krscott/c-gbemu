#include "ui.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define SCALE 4

static const unsigned long tile_colors[4] = {
    0xFFFFFFFF,
    0xFFAAAAAA,
    0xFF555555,
    0xFF000000,
};

// static void delay_ms(u32 ms) { SDL_Delay(ms); }

static void draw_tile(SDL_Surface *surface, GameBoy *gb, u16 vram_address,
                      u16 tile_index, int x, int y) {
    SDL_Rect rc;

    for (int tile_y = 0; tile_y < 16; tile_y += 2) {
        u16 addr = vram_address + (tile_index * 16) + tile_y;

        u8 bits_1 = bus_peek(&gb->bus, addr);
        u8 bits_0 = bus_peek(&gb->bus, addr + 1);

        for (int bit = 7; bit >= 0; --bit) {
            u8 mask = 1 << bit;
            u8 bit_1 = (bits_1 & mask) ? 2 : 0;
            u8 bit_0 = (bits_0 & mask) ? 1 : 0;
            u8 color_index = bit_1 | bit_0;

            rc.x = x + ((7 - bit) * SCALE);
            rc.y = y + (tile_y / 2 * SCALE);
            rc.w = SCALE;
            rc.h = SCALE;

            assert(color_index < 4);
            SDL_FillRect(surface, &rc, tile_colors[color_index]);
        }
    }
}

static void update_debug_window(SDL_Renderer *debug_renderer,
                                SDL_Texture *debug_texture,
                                SDL_Surface *debug_screen, GameBoy *gb) {
    int draw_x = 0;
    int draw_y = 0;
    int tile_index = 0;

    SDL_Rect rc;
    rc.x = 0;
    rc.y = 0;
    rc.w = debug_screen->w;
    rc.h = debug_screen->h;
    SDL_FillRect(debug_screen, &rc, 0xFF111111);

    // Draw tiles (24x16)
    for (int y = 0; y < 24; ++y) {
        for (int x = 0; x < 16; ++x) {
            draw_tile(debug_screen, gb, 0x8000, tile_index,
                      draw_x + (x * SCALE), draw_y + (y * SCALE));

            draw_x += 8 * SCALE;
            ++tile_index;
        }

        draw_x = 0;
        draw_y += 8 * SCALE;
    }

    SDL_UpdateTexture(debug_texture, NULL, debug_screen->pixels,
                      debug_screen->pitch);
    SDL_RenderClear(debug_renderer);
    SDL_RenderCopy(debug_renderer, debug_texture, NULL, NULL);
    SDL_RenderPresent(debug_renderer);
}

int ui_main(pthread_mutex_t *gb_mutex, GameBoy *gb) {
    assert(gb_mutex);
    assert(gb);

    SDL_Window *window;
    SDL_Renderer *renderer;
    // SDL_Texture *texture;
    SDL_Surface *screen;

    SDL_Window *debug_window;
    SDL_Renderer *debug_renderer;
    SDL_Texture *debug_texture;
    SDL_Surface *debug_screen;

    SDL_Event e;

    int err = 0;
    bool shutdown = false;

    // Initialize SDL windows

    err = SDL_Init(SDL_INIT_VIDEO);
    if (err) goto ui_main_exit;

    err = TTF_Init();
    if (err) goto ui_main_exit;

    // Setup main screen

    err = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window,
                                      &renderer);
    if (err) goto ui_main_exit;

    screen = SDL_GetWindowSurface(window);

    // Setup debug screen

    err = SDL_CreateWindowAndRenderer(16 * 8 * SCALE, 32 * 8 * SCALE, 0,
                                      &debug_window, &debug_renderer);
    if (err) goto ui_main_exit;

    debug_screen = SDL_CreateRGBSurface(
        0, (16 * 8 * SCALE) + (16 * SCALE), (32 * 8 * SCALE) + (64 * SCALE), 32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    debug_texture = SDL_CreateTexture(
        debug_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
        (16 * 8 * SCALE) + (16 * SCALE), (32 * 8 * SCALE) + (64 * SCALE));

    // Move debug window next to main window
    {
        int x, y;
        SDL_GetWindowPosition(window, &x, &y);
        SDL_SetWindowPosition(debug_window, x + SCREEN_WIDTH + 10, y);
    }

    // UI Main loop
    while (!shutdown) {
        pthread_mutex_lock(gb_mutex);
        {
            update_debug_window(debug_renderer, debug_texture, debug_screen,
                                gb);

            // TODO Main window
            {
                // Fill the surface purple
                SDL_FillRect(screen, NULL,
                             SDL_MapRGB(screen->format, 0x33, 0x00, 0x33));

                SDL_UpdateWindowSurface(window);
            }

            // Detect shutdown
            shutdown |= gb->shutdown;
        }
        pthread_mutex_unlock(gb_mutex);

        // Handle events
        while (SDL_PollEvent(&e) > 0) {
            // TODO window updates

            if (e.type == SDL_WINDOWEVENT &&
                e.window.event == SDL_WINDOWEVENT_CLOSE) {
                shutdown = true;
            }
        }

        // delay_ms(8);
    }

ui_main_exit:
    if (err) fprintf(stderr, "SDL Error: %s\n", SDL_GetError());

    SDL_DestroyWindow(window);
    SDL_DestroyWindow(debug_window);
    SDL_Quit();

    return err;
}
