#ifndef PPU_DEF_H
#define PPU_DEF_H

#include "ppu_fifo.h"
#include "mmu.h"
#include "screen.h"

#include <stdint.h>

typedef enum ppu_fetcher_state_e
{
    FETCHER_STOPPED,
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

    uint8_t background_x;
    uint8_t background_y;

    uint8_t window_x;
    uint8_t window_y;

    int oam_index;
} ppu_fetcher_t;

typedef enum ppu_mode_e
{
    PPU_MODE_H_BLANK = 0,
    PPU_MODE_V_BLANK = 1,
    PPU_MODE_OAM_SEARCH = 2,
    PPU_MODE_PIXEL_TRANSFER = 3
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

typedef struct tile_s
{
    uint8_t data[16];
} tile_t;

typedef struct oam_entry_s
{
    uint8_t x;
    uint8_t y;
    uint8_t tile_index;
    tile_t tile;
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
    int visibles[10];
} sprites_t;

typedef struct ppu_s
{
    struct
    {
        int enabled;
        ppu_mode_t mode;
        int cycles;
        uint8_t line_y;
        uint8_t line_y_compare;
    } status;

    viewport_t viewport;
    background_t background;
    window_t window;
    palette_t bg_palette;
    palette_t obj_palettes[2];
    sprites_t sprites;

    mmu_t *mmu;
    screen_t *screen;

    //ppu_fetcher_t fetcher;
    //ppu_fifo_t fifo;
    pixel_data_t line[160];
} ppu_t;

#endif /*PPU_DEF_H*/