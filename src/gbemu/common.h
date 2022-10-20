#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Logging macros

#define errorf(fmt, ...) \
    fprintf(stderr, "%s:%d ERROR " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define error(msg) fprintf(stderr, "%s:%d ERROR " msg "\n", __FILE__, __LINE__)

#define infof(fmt, ...) \
    fprintf(stderr, "%s:%d INFO " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define info(msg) fprintf(stderr, "%s:%d INFO " msg "\n", __FILE__, __LINE__)

#define panicf(fmt, ...)                                             \
    do {                                                             \
        fprintf(stderr, "%s:%d PANIC " fmt "\n", __FILE__, __LINE__, \
                ##__VA_ARGS__);                                      \
        abort();                                                     \
    } while (0)

#define panic(msg)                                                    \
    do {                                                              \
        fprintf(stderr, "%s:%d PANIC " msg "\n", __FILE__, __LINE__); \
        abort();                                                      \
    } while (0)

// Type abbreviations

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

/// Defer macro which wraps "cleanup" attribute
#define defer(func) __attribute__((cleanup(func)))