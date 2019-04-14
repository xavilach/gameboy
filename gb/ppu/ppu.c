#include "ppu.h"

#include "ppu_fetcher.h"
#include "ppu_regs.h"
#include "ppu_def.h"

#include <stdint.h>
#include <stdlib.h>

static void ppu_load_oam_entries(ppu_t *p_ppu);

ppu_t *ppu_allocate(mmu_t *p_mmu, screen_t *p_screen)
{
    if (!p_mmu || !p_screen)
        return NULL;

    ppu_t *p_ppu = calloc(1, sizeof(ppu_t));

    if (p_ppu)
    {
        p_ppu->mmu = p_mmu;
        p_ppu->screen = p_screen;
    }

    return p_ppu;
}

int ppu_execute(ppu_t *p_ppu)
{
    if (!p_ppu)
    {
        return 0;
    }

    p_ppu->status.cycles += 1;

    ppu_reg_read_lcdc(p_ppu);

    if (!p_ppu->status.enabled)
        return 1;

    switch (p_ppu->status.mode)
    {
    case PPU_MODE_OAM_SEARCH:
        //cpu cannot access oam

        if (p_ppu->status.cycles >= 20 * 40)
        {
            ppu_reg_read_sc(p_ppu);
            ppu_reg_read_lyc(p_ppu);
            ppu_reg_read_bgp_obp(p_ppu);
            ppu_reg_read_w(p_ppu);

            ppu_load_oam_entries(p_ppu);

            p_ppu->status.cycles = 0;
            p_ppu->status.pixel_index = 0;

            p_ppu->status.mode = PPU_MODE_PIXEL_TRANSFER;
            ppu_reg_write_stat(p_ppu);

            fetch_reset(p_ppu);
        }
        break;
    case PPU_MODE_PIXEL_TRANSFER:
        //cpu cannot access vram
        //cpu cannot access oam

        fetch_run(p_ppu);

        if (p_ppu->status.pixel_index >= 160)
        {
            fetch_stop(p_ppu);

            //Finished LCD line
            //Trigger HBLANK IRQ (STAT).
            p_ppu->status.mode = PPU_MODE_H_BLANK;
            ppu_reg_write_stat(p_ppu);
        }
        break;
    case PPU_MODE_H_BLANK:
        if (p_ppu->status.cycles >= ((43 + 51) * 4))
        {
            p_ppu->status.cycles = 0;
            p_ppu->status.line_y += 1;

            ppu_reg_write_LY(p_ppu);

            if (p_ppu->status.line_y >= 144)
            {
                // Trigger VBLANK IRQ.
                //Trigger VBLANK IRQ (STAT).
                p_ppu->status.mode = PPU_MODE_V_BLANK;
                ppu_reg_write_stat(p_ppu);
            }
            else
            {
                //Trigger OAM IRQ (STAT).
                p_ppu->status.mode = PPU_MODE_OAM_SEARCH;
                ppu_reg_write_stat(p_ppu);
            }

            //DEBUG_PRINT("New line %d\n", p_ppu->status.line_y);
        }
        break;
    case PPU_MODE_V_BLANK:
        if (p_ppu->status.cycles >= 114 * 4)
        {
            p_ppu->status.cycles = 0;
            p_ppu->status.line_y += 1;

            ppu_reg_write_LY(p_ppu);

            if (p_ppu->status.line_y >= 154)
            {
                p_ppu->status.line_y = 0;
                //Trigger OAM IRQ (STAT).
                p_ppu->status.mode = PPU_MODE_OAM_SEARCH;
                ppu_reg_write_stat(p_ppu);
            }

            //DEBUG_PRINT("New line %d\n", p_ppu->status.line_y);
        }
        break;
    }

    return 1;
}

void ppu_free(ppu_t *p_ppu)
{
    if (p_ppu)
    {
        free(p_ppu);
    }
}

static void ppu_load_oam_entries(ppu_t *p_ppu)
{
    int v = 0;

    for (int s = 0; s < 40; s++)
    {
        uint8_t y = 0;
        (void)mmu_read_u8(p_ppu->mmu, 0xFE00 + (4 * s) + 0, &y);
        p_ppu->sprites.entries[s].y = y;

        uint8_t x = 0;
        (void)mmu_read_u8(p_ppu->mmu, 0xFE00 + (4 * s) + 1, &x);
        p_ppu->sprites.entries[s].x = x;

        uint8_t tile_index = 0;
        (void)mmu_read_u8(p_ppu->mmu, 0xFE00 + (4 * s) + 2, &tile_index);
        p_ppu->sprites.entries[s].tile_index = tile_index;

        uint16_t tile_address = p_ppu->sprites.tiles_address + (tile_index * 16);
        for (int i = 0; i < 16; i++)
        {
            uint8_t data = 0;
            (void)mmu_read_u8(p_ppu->mmu, tile_address + i, &data);
            p_ppu->sprites.entries[s].tile.data[i] = data;
        }

        uint8_t flags = 0;
        (void)mmu_read_u8(p_ppu->mmu, 0xFE00 + (4 * s) + 3, &flags);
        p_ppu->sprites.entries[s].flags.priority = (flags >> 7) & 0x01;
        p_ppu->sprites.entries[s].flags.flip_y = (flags >> 6) & 0x01;
        p_ppu->sprites.entries[s].flags.flip_x = (flags >> 5) & 0x01;
        p_ppu->sprites.entries[s].flags.palette = (flags >> 4) & 0x01;

        uint8_t ly = p_ppu->status.line_y + 16;
        if ((0 != x) && (ly >= y) && (ly < y + 8) && (v < 10))
        {
            p_ppu->sprites.visibles[v] = s;
            v++;
        }
    }

    for (; v < 10; v++)
    {
        p_ppu->sprites.visibles[v] = -1;
    }
}
