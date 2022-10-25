#pragma once

typedef enum GbErr {
    OK,
    ERR_FILE_OPEN,
    ERR_ALLOC,
} GbErr;

/// @brief Return a string explanation of the given error code
/// @param err Non-zero error code
/// @return A string
const char *err_str(GbErr err);

/// @brief If err is non-zero, print an error message to stderr and exit.
/// This function is for user-errors. For programmer errors, use log_assert() in
/// logging.h.
/// @param err
void err_exit(GbErr err);
