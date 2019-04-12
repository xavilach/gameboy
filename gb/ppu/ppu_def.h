#ifndef PPU_DEF_H
#define PPU_DEF_H

#include "ppu_fetcher.h"
#include "ppu_fifo.h"
#include "mmu.h"

#include <stdint.h>

typedef enum ppu_mode_e
{
    PPU_MODE_OAM_SEARCH = 2,
    PPU_MODE_PIXEL_TRANSFER = 3,
    PPU_MODE_H_BLANK = 0,
    PPU_MODE_V_BLANK = 1
} ppu_mode_t;

typedef struct palette_s
{
    uint8_t color[4];
} palette_t;

typedef struct viewport_s
{
    uint8_t x;
    uint8_t y;
} viewport_t;

typedef struct background_s
{
    int enabled;
    uint16_t map_address;
    uint16_t tiles_address;
} background_t;

typedef struct window_s
{
    int enabled;
    uint8_t x;
    uint8_t y;
    uint16_t map_address;
} window_t;

typedef struct oam_entry_s
{
    uint8_t x;
    uint8_t y;
    uint8_t tile;
    struct
    {
        int priority;
        int flip_x;
        int flip_y;
        int palette;
    } flags;
} oam_entry_t;

typedef struct sprites_s
{
    int enabled;
    oam_entry_t entries[40];
    uint16_t tiles_address;
} sprites_t;

typedef struct ppu_s
{
    struct
    {
        int enabled;
        ppu_mode_t mode;
        int cycles;
        uint8_t line_y;
        int pixel_index;
    } status;

    viewport_t viewport;
    background_t background;
    window_t window;
    palette_t bg_palette;
    palette_t obj_palettes[2];
    sprites_t sprites;

    mmu_t *mmu;
    screen_t *screen;

    ppu_fetcher_t fetcher;
    ppu_fifo_t fifo;
} ppu_t;

#endif /*PPU_DEF_H*/