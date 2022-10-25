#include "err.h"

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

const char* err_str(GbErr err) {
    switch (err) {
        case OK:
            // Invalid argument for this function
            assert(0);
            return "";
        case ERR_FILE_OPEN:
            return "File could not be opened";
        case ERR_ALLOC:
            return "Allocation failed";
        default:
            assert(0);
            return "Unknown";
    }
}

void err_exit(GbErr err) {
    if (err) {
        fprintf(stderr, "Error: %s\n", err_str(err));
        exit(err);
    }
}
