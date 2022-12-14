#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "err.h"
#include "logging.h"

// Type abbreviations

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

/// Alias for attribute cleanup
#define defer(func) __attribute__((cleanup(func)))

/// Alias for attribute warn_unused_result
#define nodiscard __attribute__((warn_unused_result))

/// Macro to get the length of fixed-size array
#define array_len(arr) (sizeof(arr) / (sizeof(arr[0])))

#define to_u16(hi, lo) ((((u16)(hi)&0xFF) << 8) | ((u16)(lo)&0xFF))
#define high_byte(word) (((word) >> 8) & 0xFF)
#define low_byte(word) ((word)&0xFF)

#define maskbit(b) (1 << (b))
#define getbit(n, b) (((n) & (1 << (b))) != 0)
