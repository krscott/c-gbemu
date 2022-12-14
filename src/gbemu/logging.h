
#include <stdio.h>

__attribute__((__noreturn__)) //
void fprintf_and_abort(FILE* file, const char* fmt, ...);

// Logging macros

#define errorf(fmt, ...) \
    fprintf(stderr, "%s:%d ERROR " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define error(msg) fprintf(stderr, "%s:%d ERROR " msg "\n", __FILE__, __LINE__)

#define infof(fmt, ...) \
    fprintf(stderr, "%s:%d INFO " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define info(msg) fprintf(stderr, "%s:%d INFO " msg "\n", __FILE__, __LINE__)

#define panicf(fmt, ...)                                                   \
    fprintf_and_abort(stderr, "%s:%d PANIC " fmt "\n", __FILE__, __LINE__, \
                      ##__VA_ARGS__)

#define panic(msg) \
    fprintf_and_abort(stderr, "%s:%d PANIC " msg "\n", __FILE__, __LINE__)

#define log_assert(cond)                             \
    do {                                             \
        if (!(cond)) panic("Assert failed: " #cond); \
    } while (0)

#define log_assert_eq(a, b, fmt)                                    \
    do {                                                            \
        if ((a) != (b))                                             \
            panicf("Assert failed: `" #a " == " #b "` (left: `" fmt \
                   "`, right: `" fmt "`)",                          \
                   (a), (b));                                       \
    } while (0)
