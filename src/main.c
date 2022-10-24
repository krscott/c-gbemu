
#include <SDL2/SDL.h>
#include <pthread.h>
#include <stdio.h>

#include "gbemu/ui.h"

GameBoy gb;
pthread_mutex_t gb_mutex;

void *emu_thread() {
    bool shutdown = false;
    while (!shutdown) {
        pthread_mutex_lock(&gb_mutex);

        gb_step(&gb);
        shutdown = gb.shutdown;

        pthread_mutex_unlock(&gb_mutex);
    }
    return NULL;
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

    err_exit(gb_init(&gb));

    gb.trace_cpu_en = true;

    err_exit(gb_load_rom_file(&gb, filename));
    // cart_print_info(&gb.bus.cart, filename);

    if (!cart_is_valid_header(&gb.bus.cart)) {
        error("ROM header is invalid");
    }

    gb_boot_dmg(&gb);

    int ui_err = 0;

    // Start threads and wait for exit
    {
        pthread_t emu_thread_handle;

        // Start emu thread
        int thread_err =
            pthread_create(&emu_thread_handle, NULL, emu_thread, NULL);
        if (thread_err) panicf("Emu thread error %d", thread_err);

        // Run UI main function
        ui_err = ui_main(&gb_mutex, &gb);

        // When UI exits, tell gameboy to shutdown
        pthread_mutex_lock(&gb_mutex);
        gb.shutdown = true;
        pthread_mutex_unlock(&gb_mutex);

        // Wait for emu thread to exit
        pthread_join(emu_thread_handle, NULL);
    }

    printf("Final CPU State:\n");
    cpu_print_trace(&gb.cpu, &gb.bus);
    printf("SB> %s\n", gb.debug_serial_message);

    gb_deinit(&gb);
    return ui_err;
}