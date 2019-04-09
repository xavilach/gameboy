#ifndef PPU_FETCHER_H_
#define PPU_FETCHER_H_

#include "ppu_fifo.h"
#include "mmu.h"

#include <stdint.h>

typedef enum ppu_fetcher_state_e
{
    FETCHER_GET_TILE,
    FETCHER_GET_DATA_0,
    FETCHER_GET_DATA_1,
    FETCHER_WAIT
} ppu_fetcher_state_t;

typedef enum ppu_fetcher_mode_e
{
    FETCHER_MODE_BACKGROUND,
    FETCHER_MODE_WINDOW,
    FETCHER_MODE_SPRITE
} ppu_fetcher_mode_t;

typedef struct tile_data_s
{
    uint8_t index;
    uint8_t data[2];
} tile_data_t;

typedef struct ppu_fetcher_s
{
    int cycles;
    ppu_fetcher_state_t state;
    ppu_fetcher_mode_t mode;

    tile_data_t tile;

    uint16_t map_address;
    uint16_t tiles_address;

    uint8_t x;
    uint8_t y;
} ppu_fetcher_t;

static inline void fetch(ppu_fetcher_t *p_fetcher, mmu_t *p_mmu, ppu_fifo_t *p_fifo)
{
    p_fetcher->cycles += 1;
    if (p_fetcher->cycles >= 2)
    {
        p_fetcher->cycles = 0;
        return;
    }

    uint16_t address;

    switch (p_fetcher->state)
    {
    case FETCHER_GET_TILE:
        address = p_fetcher->map_address + (p_fetcher->y * 32) + p_fetcher->x;
        (void)mmu_read_u8(p_mmu, address, &p_fetcher->tile.index);
        p_fetcher->state = FETCHER_GET_DATA_0;
        break;
    case FETCHER_GET_DATA_0:
        address = p_fetcher->tiles_address + (p_fetcher->tile.index * sizeof(uint16_t));
        (void)mmu_read_u8(p_mmu, address, &p_fetcher->tile.data[0]);
        p_fetcher->state = FETCHER_GET_DATA_1;
        break;
    case FETCHER_GET_DATA_1:
        address = p_fetcher->tiles_address + (p_fetcher->tile.index * sizeof(uint16_t)) + 1;
        (void)mmu_read_u8(p_mmu, address, &p_fetcher->tile.data[1]);
        p_fetcher->state = FETCHER_WAIT;
        break;
    case FETCHER_WAIT:
        if (p_fifo->count <= 8)
        {
            pixel_data_t data;
            for (int i = 0; i < 8; i++)
            {
                data.data = 0;
                data.data |= (p_fetcher->tile.data[0] >> (7 - i)) & 0x01;
                data.data <<= 1;
                data.data |= (p_fetcher->tile.data[1] >> (7 - i)) & 0x01;
                (void)ppu_fifo_push(p_fifo, data);
            }

            p_fetcher->state = FETCHER_GET_TILE;
        }
        break;

    default:
        break;
    }
}

#endif /*PPU_FETCHER_H_*/