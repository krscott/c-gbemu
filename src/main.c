
#include <SDL2/SDL.h>
#include <pthread.h>
#include <pthread_time.h>
#include <stdatomic.h>
#include <stdio.h>
#include <windows.h>

#include "gbemu/ui.h"

#define PROG_NAME "c-gbemu"
#define DOCS_URL "https://github.com/krscott/c-gbemu"

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

#define STOP_ON_BLARGG_TEST_END

#define TARGET_FRAME_TIME_US (1000000 / 60)
// #define TARGET_FRAME_TIME_US 1

#define FPS_UPDATE_TIME_US 1000000

typedef struct Opts {
    const char *rom_filename;
    _Atomic bool verbose;
    _Atomic bool trace;
} Opts;

static Opts opts;
static GameBoy gb;
static pthread_mutex_t gb_mutex;

static void print_usage(void) {
    eprintf("Usage: " PROG_NAME " [-h] [-v] [-t] ROMFILE\n");
}

static void print_help(void) {
    eprintf(PROG_NAME ": A GameBoy emulator by Kris Scott\n");
    eprintf("\n");
    print_usage();
    eprintf("\n");
    eprintf("Parmeters\n");
    eprintf("  ROMFILE  GameBoy cartridge ROM file to emulate\n");
    eprintf("\n");
    eprintf("Options\n");
    eprintf("  -h       Show this help\n");
    eprintf("  -v       Verbose mode\n");
    eprintf("  -t       Enable CPU trace messages\n");
    eprintf("\n");
    eprintf("Documentation\n");
    eprintf("  " DOCS_URL "\n");
    eprintf("\n");
}

static void opts_parse(Opts *opts, int argc, char *args[]) {
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
                        default:
                            eprintf("Unexpected option: -%c\n", arg[j]);
                            print_usage();
                            exit(1);
                    }
                }
                continue;
            }
        }

        if (!opts->rom_filename) {
            opts->rom_filename = arg;
            continue;
        }

        eprintf("Error: too many options\n");
        print_usage();
        exit(1);
    }

    if (!opts->rom_filename) {
        eprintf("Error: missing ROMFILE\n");
        print_usage();
        exit(1);
    }
}

static size_t delta_time_ms(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000 +
           (end.tv_nsec - start.tv_nsec) / 1000000L;
}

static size_t delta_time_us(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000000 +
           (end.tv_nsec - start.tv_nsec) / 1000L;
}

// static size_t delta_time_s(struct timespec start, struct timespec end) {
//     return (end.tv_sec - start.tv_sec) +
//            (end.tv_nsec - start.tv_nsec) / 1000000000L;
// }

static void spin_us(size_t us) {
    struct timespec start;
    struct timespec current;

    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        panic("clock_gettime failed");
    }

    do {
        if (clock_gettime(CLOCK_MONOTONIC, &current) == -1) {
            panic("clock_gettime failed");
        }
    } while (delta_time_us(start, current) < us);
}

static void *emu_thread() {
    Sleep(100);

    struct timespec prev_time;
    struct timespec time;
    u32 frame_counter_start_frames = 0;

    if (clock_gettime(CLOCK_MONOTONIC, &prev_time) == -1) {
        panic("clock_gettime failed");
    }
    struct timespec frame_counter_start = prev_time;
    struct timespec t0 = prev_time;

    while (!gb.shutdown) {
        u32 prev_frame_count;
        u32 frame_count;

        pthread_mutex_lock(&gb_mutex);
        {
            // Loop until next frame
            if (opts.trace) {
                gb_print_trace(&gb);
            }

            prev_frame_count = gb_get_frame_count(&gb);
            gb_step(&gb);
            frame_count = gb_get_frame_count(&gb);

            assert(frame_count == prev_frame_count + 1 ||
                   frame_count == prev_frame_count);

#ifdef STOP_ON_BLARGG_TEST_END
            if ((gb.cpu.pc == 0xC7F4 && gb.cpu.opcode == 0x18) ||
                0 == strcmp("Passed\n", gb.debug_serial_message) ||
                0 == strcmp("Failed\n", gb.debug_serial_message)) {
                gb.shutdown = true;
            }
#endif
        }
        pthread_mutex_unlock(&gb_mutex);

        if (frame_count != prev_frame_count) {
            if (clock_gettime(CLOCK_MONOTONIC, &time) == -1) {
                panic("clock_gettime failed");
            }

            size_t delta_us = delta_time_us(prev_time, time);

            if (delta_us < TARGET_FRAME_TIME_US) {
                // Limit frame time
                // printf("Time %d Delay %d ms\n", time_ms - prev_time_ms,
                //        TARGET_FRAME_TIME_MS - (time_ms - prev_time_ms));

                // Sleep((TARGET_FRAME_TIME_US - delta_us) / 1000);
                spin_us(TARGET_FRAME_TIME_US - delta_us);

                // Re-get time including delay
                if (clock_gettime(CLOCK_MONOTONIC, &time) == -1) {
                    panic("clock_gettime failed");
                }
                delta_us = delta_time_us(prev_time, time);
            }

            size_t frame_count_delta_us =
                delta_time_us(frame_counter_start, time);
            if (frame_count_delta_us >= FPS_UPDATE_TIME_US) {
                u32 dframes = frame_count - frame_counter_start_frames;
                float fps = 1000000. * dframes / frame_count_delta_us;

                frame_counter_start = time;
                frame_counter_start_frames = frame_count;

                printf("Time: %lld ms Frame: %d FPS: %f\n",
                       delta_time_ms(t0, time), frame_count, fps);
            }

            prev_time = time;
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    opts_parse(&opts, argc, argv);

    err_exit(gb_init(&gb));

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