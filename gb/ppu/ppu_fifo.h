#ifndef PPU_FIFO_H_
#define PPU_FIFO_H_

#include <stdint.h>

typedef enum pixel_type_s
{
    PIXEL_TYPE_BACKGROUND,
    PIXEL_TYPE_WINDOW,
    PIXEL_TYPE_SPRITE
} pixel_type_t;

typedef struct pixel_data_s
{
    pixel_type_t type;
    uint8_t data;
} pixel_data_t;

typedef struct ppu_fifo_s
{
    pixel_data_t data[16];
    int read;
    int write;
    int count;
} ppu_fifo_t;

static inline void ppu_fifo_clear(ppu_fifo_t *p_fifo)
{
    p_fifo->read = 0;
    p_fifo->write = 0;
    p_fifo->count = 0;
}

static inline int ppu_fifo_pop(ppu_fifo_t *p_fifo, pixel_data_t *p_data)
{
    int ret = -1;
    if (p_fifo->count > 0)
    {
        *p_data = p_fifo->data[p_fifo->read];
        p_fifo->read = (p_fifo->read + 1) % 16;
        p_fifo->count -= 1;
        ret = 0;
    }
    return ret;
}

static inline int ppu_fifo_push(ppu_fifo_t *p_fifo, pixel_data_t data)
{
    int ret = -1;
    if (p_fifo->count < 16)
    {
        p_fifo->data[p_fifo->write] = data;
        p_fifo->write = (p_fifo->write + 1) % 16;
        p_fifo->count += 1;
        ret = 0;
    }
    return ret;
}

#endif /*PPU_FIFO_H_*/