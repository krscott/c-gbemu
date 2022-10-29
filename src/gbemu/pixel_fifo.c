#include "pixel_fifo.h"

#include <string.h>

void pixelfifo_zero(PixelFifo *fifo) { memset(fifo, 0, sizeof(*fifo)); }

void pixelfifo_push(PixelFifo *fifo, u8 color_index) {
    assert(fifo->size < PIXEL_FIFO_MAX_LENGTH);

    fifo->pixels[fifo->tail] = color_index;

    fifo->tail = (fifo->tail + 1) % PIXEL_FIFO_MAX_LENGTH;
    ++fifo->size;
}

u8 pixelfifo_pop(PixelFifo *fifo) {
    assert(fifo->size > 0);

    u8 color_index = fifo->pixels[fifo->head];

    fifo->head = (fifo->head + 1) % PIXEL_FIFO_MAX_LENGTH;
    --fifo->size;

    return color_index;
}