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

/// Defer macro which wraps "cleanup" attribute
#define defer(func) __attribute__((cleanup(func)))

#define nodiscard __attribute__((warn_unused_result))

#define array_len(arr) (sizeof(arr) / (sizeof(arr[0])))
