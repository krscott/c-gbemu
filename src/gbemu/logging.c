#include "logging.h"

#include <stdarg.h>
#include <stdlib.h>

__attribute__((__noreturn__)) //
void fprintf_and_abort(FILE* file, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(file, fmt, args);
    va_end(args);

    abort();
}
