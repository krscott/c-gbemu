#pragma once

#define breakpoint_assert(cond) \
    do {                        \
        if (!(cond)) {          \
            breakpoint();       \
            assert(cond);       \
        }                       \
    } while (0)

typedef enum {
    OK,
    ERR_FILE_OPEN,
    ERR_ALLOC,
} GbErr;

/// @brief Return a string explanation of the given error code
/// @param err Non-zero error code
/// @return A string
const char *err_str(GbErr err);

/// @brief If err is non-zero, print an error message to stderr and exit.
/// @param err
void err_exit(GbErr err);

/// @brief Function to enable breakpoint_assert macro
/// @param cond
void breakpoint();
