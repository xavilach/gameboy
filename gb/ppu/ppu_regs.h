#ifndef PPU_REGS_H_
#define PPU_REGS_H_

#include "ppu_def.h"

#include <stdio.h>
#include <stdlib.h>

#define PPU_REG_LCDC (0xFF40)
#define PPU_REG_STAT (0xFF41)
#define PPU_REG_SCY (0xFF42)
#define PPU_REG_SCX (0xFF43)
#define PPU_REG_LY (0xFF44)
#define PPU_REG_LYC (0xFF45)
#define PPU_REG_BGP (0xFF47)
#define PPU_REG_OBP0 (0xFF48)
#define PPU_REG_OBP1 (0xFF49)
#define PPU_REG_WY (0xFF4A)
#define PPU_REG_WX (0xFF4B)

/**/

static inline uint8_t get_color(uint8_t palette_color)
{
    const uint8_t PPU_PALETTE_COLORS[4] = {0x00, 0x55, 0xAA, 0xFF};
    return PPU_PALETTE_COLORS[palette_color];
}

/* Read accesses. */

static inline void ppu_reg_read_lcdc(ppu_t *p_ppu)
{
    uint8_t lcdc;
    (void)mmu_read_u8(p_ppu->mmu, PPU_REG_LCDC, &lcdc);

    p_ppu->status.enabled = (0 != ((lcdc >> 7) & 0x01));
    p_ppu->window.map_address = ((lcdc >> 6) & 0x01) ? 0x9C00 : 0x9800;
    p_ppu->window.enabled = (0 != ((lcdc >> 5) & 0x01));
    p_ppu->background.tiles_address = ((lcdc >> 4) & 0x01) ? 0x8000 : 0x8800;
    p_ppu->sprites.tiles_address = 0x8000;
    p_ppu->background.map_address = ((lcdc >> 3) & 0x01) ? 0x9C00 : 0x9800;
    p_ppu->sprites.enabled = (0 != ((lcdc >> 2) & 0x01));
    //TODO
    // 1 obj size
    p_ppu->background.enabled = (0 != ((lcdc >> 0) & 0x01));
}

static inline void ppu_reg_read_sc(ppu_t *p_ppu)
{
    uint8_t scy, scx;
    (void)mmu_read_u8(p_ppu->mmu, PPU_REG_SCY, &scy);
    (void)mmu_read_u8(p_ppu->mmu, PPU_REG_SCX, &scx);

    p_ppu->viewport.y = scy;
    p_ppu->viewport.x = scx;
}

static inline void ppu_reg_read_lyc(ppu_t *p_ppu)
{
    uint8_t lyc;
    (void)mmu_read_u8(p_ppu->mmu, PPU_REG_LYC, &lyc);

    p_ppu->status.line_y_compare = lyc;
}

static inline void ppu_reg_read_bgp_obp(ppu_t *p_ppu)
{
    uint8_t bgp;
    (void)mmu_read_u8(p_ppu->mmu, PPU_REG_BGP, &bgp);

    uint8_t obp[2];
    (void)mmu_read_u8(p_ppu->mmu, PPU_REG_OBP0, &obp[0]);
    (void)mmu_read_u8(p_ppu->mmu, PPU_REG_OBP1, &obp[1]);

    for (int i = 0; i < 4; i++)
    {
        p_ppu->bg_palette.color[i] = get_color((bgp >> (i * 2)) & 0x03);
        p_ppu->obj_palettes[0].color[i] = get_color((obp[0] >> (i * 2)) & 0x03);
        p_ppu->obj_palettes[1].color[i] = get_color((obp[1] >> (i * 2)) & 0x03);
    }
}

static inline void ppu_reg_read_w(ppu_t *p_ppu)
{
    uint8_t wy, wx;
    (void)mmu_read_u8(p_ppu->mmu, PPU_REG_WY, &wx);
    (void)mmu_read_u8(p_ppu->mmu, PPU_REG_WX, &wy);

    p_ppu->window.y = wy;
    p_ppu->window.x = wx;
}

/* Write accesses. */

static inline void ppu_reg_write_stat(ppu_t *p_ppu)
{
    uint8_t stat;
    (void)mmu_read_u8(p_ppu->mmu, PPU_REG_STAT, &stat);

    int coincidence_irq = (0 != ((stat >> 6) & 0x01));
    int oam_irq = (0 != ((stat >> 5) & 0x01));
    int vblank_irq = (0 != ((stat >> 4) & 0x01));
    int hblank_irq = (0 != ((stat >> 3) & 0x01));

    int coincidence_flag = (p_ppu->status.line_y == p_ppu->status.line_y_compare);

    stat = (stat & 0xF8);

    /* LYC == LY */
    if (coincidence_flag)
    {
        stat |= 0x04;
    }

    /* PPU mode */
    stat |= p_ppu->status.mode & 0x03;

    (void)mmu_write_u8(p_ppu->mmu, PPU_REG_STAT, stat);

    //TODO Cleaner way
    uint8_t irq_flags;
    (void)mmu_read_u8(p_ppu->mmu, 0xFF0F, &irq_flags);

    int stat_flag = (0 != ((irq_flags >> 1) & 0x01));

    if (!stat_flag && coincidence_irq && coincidence_flag)
    {
        stat_flag = 1;
    }

    if (!stat_flag && oam_irq && (PPU_MODE_OAM_SEARCH == p_ppu->status.mode))
    {
        stat_flag = 1;
    }

    if (!stat_flag && vblank_irq && (PPU_MODE_V_BLANK == p_ppu->status.mode))
    {
        stat_flag = 1;
    }

    if (!stat_flag && hblank_irq && (PPU_MODE_H_BLANK == p_ppu->status.mode))
    {
        stat_flag = 1;
    }

    if (stat_flag)
    {
        irq_flags |= 0x02;
    }

    int vblank_flag = (PPU_MODE_V_BLANK == p_ppu->status.mode);

    if (vblank_flag)
    {
        irq_flags |= 0x01;
    }

    (void)mmu_write_u8(p_ppu->mmu, 0xFF0F, irq_flags);
}

static inline void ppu_reg_write_LY(ppu_t *p_ppu)
{
    uint8_t ly;
    ly = p_ppu->status.line_y;

    (void)mmu_write_u8(p_ppu->mmu, PPU_REG_LY, ly);
}

#endif /*PPU_REGS_H_*/