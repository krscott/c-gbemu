#include "ui.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

int ui_main(pthread_mutex_t *gb_mutex, GameBoy *gb) {
    (void)gb_mutex;
    (void)gb;

    SDL_Window *window;
    SDL_Renderer *renderer;
    // SDL_Texture *texture;
    SDL_Surface *screen;
    SDL_Event e;

    int err = 0;

    err = SDL_Init(SDL_INIT_VIDEO);
    if (err) goto ui_main_exit;

    err = TTF_Init();
    if (err) goto ui_main_exit;

    err = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window,
                                      &renderer);
    if (err) goto ui_main_exit;

    screen = SDL_GetWindowSurface(window);

    // UI Main loop
    for (;;) {
        // Handle events
        while (SDL_PollEvent(&e) > 0) {
            // TODO window updates

            if (e.type == SDL_WINDOWEVENT &&
                e.window.event == SDL_WINDOWEVENT_CLOSE) {
                goto ui_main_exit;
            }

            // Fill the surface purple
            SDL_FillRect(screen, NULL,
                         SDL_MapRGB(screen->format, 0x33, 0x00, 0x33));

            SDL_UpdateWindowSurface(window);
        }
    }

ui_main_exit:
    if (err) fprintf(stderr, "SDL Error: %s\n", SDL_GetError());

    SDL_DestroyWindow(window);
    SDL_Quit();

    return err;
}

// void window(void) {
//     // The window we'll be rendering to
//     SDL_Window *window = NULL;

//     // The surface contained by the window
//     SDL_Surface *screenSurface = NULL;

//     // Initialize SDL
//     if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//         printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
//     } else {
//         // Create window
//         window = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_UNDEFINED,
//                                   SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
//                                   SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
//         if (window == NULL) {
//             printf("Window could not be created! SDL_Error: %s\n",
//                    SDL_GetError());
//         } else {
//             // Get window surface
//             screenSurface = SDL_GetWindowSurface(window);

//             // Fill the surface pink
//             SDL_FillRect(screenSurface, NULL,
//                          SDL_MapRGB(screenSurface->format, 0xFF, 0x00,
//                          0xFF));

//             // Update the surface
//             SDL_UpdateWindowSurface(window);

//             // Wait two seconds
//             SDL_Delay(2000);
//         }
//     }

//     // Destroy window
//     SDL_DestroyWindow(window);

//     // Quit SDL subsystems
//     SDL_Quit();
// }