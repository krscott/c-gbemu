
#include <SDL2/SDL.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>

#include "gbemu/ui.h"

typedef struct Opts {
    const char *rom_filename;
    _Atomic bool verbose;
    _Atomic bool trace;
} Opts;

Opts opts;
GameBoy gb;
pthread_mutex_t gb_mutex;

void print_usage(void) { printf("Usage: c-gbemu [-h] [-v] [-t] ROMFILE\n"); }

void print_help(void) {
    printf("c-gbemu: A GameBoy emulator by Kris Scott\n");
    printf("\n");
    print_usage();
    printf("Load and run a ROMFILE.\n");
    printf("\n");
    printf("Options\n");
    printf("  -h    Show this help.\n");
    printf("  -v    Verbose mode.\n");
    printf("  -t    Enable CPU trace messages.\n");
    printf("\n");
}

void opts_parse(Opts *opts, int argc, char *args[]) {
    int i;
    size_t j, len, arglen;
    const char *arg;
    bool disable_args = false;

    memset(opts, 0, sizeof(*opts));

    for (i = 1; i < argc; ++i) {
        arg = args[i];
        len = strlen(arg);
        if (len == 0) continue;

        if (!disable_args) {
            if (0 == strcmp("--", arg)) {
                disable_args = true;
                continue;
            }

            if (arg[0] == '-' && len > 1) {
                arglen = strlen(arg);
                for (j = 1; j < arglen; ++j) {
                    switch (arg[j]) {
                        case 'h':
                            print_help();
                            exit(1);
                        case 'v':
                            opts->verbose = true;
                            break;
                        case 't':
                            opts->trace = true;
                            break;
                    }
                }
                continue;
            }
        }

        if (!opts->rom_filename) {
            opts->rom_filename = arg;
            continue;
        }

        printf("Error: too many options\n");
        print_usage();
        exit(1);
    }

    if (!opts->rom_filename) {
        printf("Error: missing ROMFILE\n");
        print_usage();
        exit(1);
    }
}

void *emu_thread() {
    bool shutdown = false;
    while (!shutdown) {
        pthread_mutex_lock(&gb_mutex);

        if (opts.trace) {
            gb_print_trace(&gb);
        }

        gb_step(&gb);

        if (0 == strcmp("Passed\n", gb.debug_serial_message) ||
            0 == strcmp("Failed\n", gb.debug_serial_message)) {
            gb.shutdown = true;
        }

        shutdown = gb.shutdown;

        pthread_mutex_unlock(&gb_mutex);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    opts_parse(&opts, argc, argv);

    err_exit(gb_init(&gb));

    gb.trace_cpu_en = opts.trace;

    err_exit(gb_load_rom_file(&gb, opts.rom_filename));

    if (opts.verbose) {
        cart_print_info(&gb.bus.cart, opts.rom_filename);
    }

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
        gb.shutdown = true;

        // Wait for emu thread to exit
        pthread_join(emu_thread_handle, NULL);
    }

    if (opts.verbose) {
        printf("Final CPU State:\n");
        cpu_print_trace(&gb.cpu, &gb.bus);
        printf("SB> %s\n", gb.debug_serial_message);
    }

    gb_deinit(&gb);
    return ui_err;
}