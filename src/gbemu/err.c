#include "err.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const char *err_str(GbemuError err) {
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

void err_exit(GbemuError err) {
    if (err) {
        fprintf(stderr, "Error: %s", err_str(err));
        exit(err);
    }
}