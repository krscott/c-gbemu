#pragma once

#include <limits.h>

#include "common.h"

#define PIXEL_FIFO_MAX_LENGTH 16

static_assert(PIXEL_FIFO_MAX_LENGTH - 1 <= UCHAR_MAX);
typedef struct PixelFifo {
    uint_fast8_t head;
    uint_fast8_t tail;
    uint_fast8_t size;
    u8 pixels[PIXEL_FIFO_MAX_LENGTH];
} PixelFifo;

void pixelfifo_zero(PixelFifo *fifo);

void pixelfifo_push(PixelFifo *fifo, u8 color_index);

u8 pixelfifo_pop(PixelFifo *fifo);