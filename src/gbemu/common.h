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
