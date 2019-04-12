#ifndef PPU_BASE_H_
#define PPU_BASE_H_

#include "ppu_fetcher.h"
#include "ppu_fifo.h"
#include "lcd.h"
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
    lcd_t *lcd;

    ppu_fetcher_t fetcher;
    ppu_fifo_t fifo;

    /*
    sprite_t visible_sprites[10];
    int sprite_height;
    uint8_t line_y_compare;
    */
} ppu_t;

static inline uint8_t get_color(uint8_t palette_color)
{
    const uint8_t PPU_PALETTE_COLORS[4] = {0x00, 0x55, 0xAA, 0xFF};
    return PPU_PALETTE_COLORS[palette_color];
}

static inline void set_enabled(ppu_t *p_ppu)
{
    uint8_t lcdc;
    (void)mmu_read_u8(p_ppu->mmu, 0xFF40, &lcdc);

    p_ppu->status.enabled = (0 != ((lcdc >> 7) & 0x01));
}

static inline void update_palettes(ppu_t *p_ppu)
{
    uint8_t bgp;
    (void)mmu_read_u8(p_ppu->mmu, 0xFF47, &bgp);

    uint8_t obp[2];
    (void)mmu_read_u8(p_ppu->mmu, 0xFF48, &obp[0]);
    (void)mmu_read_u8(p_ppu->mmu, 0xFF49, &obp[1]);

    for (int i = 0; i < 4; i++)
    {
        p_ppu->bg_palette.color[i] = get_color((bgp >> (i * 2)) & 0x03);
        p_ppu->obj_palettes[0].color[i] = get_color((obp[0] >> (i * 2)) & 0x03);
        p_ppu->obj_palettes[1].color[i] = get_color((obp[1] >> (i * 2)) & 0x03);
    }
}

static inline void update_viewport(ppu_t *p_ppu)
{
    uint8_t scx, scy;
    (void)mmu_read_u8(p_ppu->mmu, 0xFF42, &scx);
    (void)mmu_read_u8(p_ppu->mmu, 0xFF43, &scy);

    p_ppu->viewport.x = scx;
    p_ppu->viewport.y = scy;
}

static inline void update_background(ppu_t *p_ppu)
{
    uint8_t lcdc;
    (void)mmu_read_u8(p_ppu->mmu, 0xFF40, &lcdc);

    p_ppu->background.enabled = (0 != ((lcdc >> 0) & 0x01));
    p_ppu->background.map_address = ((lcdc >> 3) & 0x01) ? 0x9C00 : 0x9800;
    p_ppu->background.tiles_address = ((lcdc >> 4) & 0x01) ? 0x8000 : 0x8800;
}

static inline void update_window(ppu_t *p_ppu)
{
    uint8_t lcdc, wx, wy;
    (void)mmu_read_u8(p_ppu->mmu, 0xFF40, &lcdc);
    (void)mmu_read_u8(p_ppu->mmu, 0xFF4A, &wy);
    (void)mmu_read_u8(p_ppu->mmu, 0xFF4B, &wx);

    p_ppu->window.enabled = (0 != ((lcdc >> 5) & 0x01));
    p_ppu->window.x = wx;
    p_ppu->window.y = wy;
    p_ppu->window.map_address = ((lcdc >> 6) & 0x01) ? 0x9C00 : 0x9800;
}

static inline void update_lineY(ppu_t *p_ppu)
{
    uint8_t ly = p_ppu->status.line_y;

    (void)mmu_write_u8(p_ppu->mmu, 0xFF44, ly);
}

#endif /*PPU_BASE_H_*/