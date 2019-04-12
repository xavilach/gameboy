#ifndef PPU_BASE_H_
#define PPU_BASE_H_

#include "ppu_def.h"

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