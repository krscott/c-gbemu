#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Logging macros

#define errorf(fmt, ...) \
    fprintf(stderr, "%s:%d ERROR " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define error(msg) \
    fprintf(stderr, "%s:%d ERROR " msg "\n", __FILE__, __LINE__)

#define infof(fmt, ...) \
    fprintf(stderr, "%s:%d INFO " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define info(msg) \
    fprintf(stderr, "%s:%d INFO " msg "\n", __FILE__, __LINE__)

// Type abbreviations

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// Allocator

typedef struct
{
    void *(*alloc)(size_t);
    void (*free)(void *);
} Allocator;

inline Allocator std_allocator()
{
    Allocator allocator = {
        .alloc = malloc,
        .free = free,
    };
    return allocator;
}
