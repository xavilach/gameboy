#ifndef PPU_BASE_H_
#define PPU_BASE_H_

#include "ppu_fetcher.h"
#include "lcd.h"

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

    ppu_fifo_t *fifo;
    mmu_t *mmu;
    lcd_t *lcd;
    ppu_fetcher_t *fetcher;

    /*
    int pixel_index;

    sprite_t visible_sprites[10];

    int sprite_height;

    uint8_t scroll_x;
    uint8_t scroll_y;
    uint8_t line_y;
    uint8_t line_y_compare;
    uint8_t column_x;
    */
} ppu_t;

static inline uint8_t get_color(uint8_t palette_color)
{
    const uint8_t PPU_PALETTE_COLORS[4] = {0x00, 0x55, 0xAA, 0xFF};
    return PPU_PALETTE_COLORS[palette_color];
}

static inline void update_palettes(ppu_t *p_ppu)
{
    uint8_t bgp = p_ppu->mem[0xFF47];
    uint8_t obp[2] = {p_ppu->mem[0xFF48], p_ppu->mem[0xFF49]};

    for (int i = 0; i < 4; i++)
    {
        p_ppu->bg_palette.color[i] = get_color((bgp >> (i * 2)) & 0x03);
        p_ppu->obj_palettes[0].color[i] = get_color((obp[0] >> (i * 2)) & 0x03);
        p_ppu->obj_palettes[1].color[i] = get_color((obp[1] >> (i * 2)) & 0x03);
    }
}

static inline window_t get_window(ppu_t *p_ppu)
{
    window_t window;

    window.enabled = (0 != ((p_ppu->mem[0xFF40] >> 5) & 0x01));
    window.x = p_ppu->mem[0xFF4B];
    window.y = p_ppu->mem[0xFF4A];

    return window;
}

#endif /*PPU_BASE_H_*/