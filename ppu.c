#include "ppu.h"

#include "ppu_base.h"

#include <stdint.h>
#include <stdlib.h>

ppu_t *ppu_allocate(mmu_t *p_mmu, lcd_t *p_lcd)
{
    if (!p_mmu || !p_lcd)
        return NULL;

    ppu_t *p_ppu = calloc(1, sizeof(ppu_t));

    if (p_ppu)
    {
        p_ppu->mmu = p_mmu;
        p_ppu->lcd = p_lcd;
    }

    return p_ppu;
}

void ppu_cycle(ppu_t *p_ppu)
{
    p_ppu->status.cycles += 1;

    switch (p_ppu->status.mode)
    {
    case PPU_MODE_OAM_SEARCH:
        //find 10 first sprites visible on line p_ppu->line_y.
        // (oam.x != 0) && ((ly + 16) >= (oam.y)) && ((ly + 16) < oam.y + h)

        //cpu cannot access oam

        if (20 == p_ppu->status.cycles)
        {
            p_ppu->status.cycles = 0;
            p_ppu->status.mode = PPU_MODE_PIXEL_TRANSFER;
            p_ppu->status.pixel_index = 0;
        }
        break;
    case PPU_MODE_PIXEL_TRANSFER:
        //cpu cannot access vram
        //cpu cannot access oam

        fetch(p_ppu->fetcher, p_ppu->mmu, p_ppu->fifo);

        if (p_ppu->fifo->count > 8)
        {
            //more than 8 pixels, pop pixel from fifo.
            pixel_data_t data;
            if (0 == ppu_fifo_pop(p_ppu->fifo, &data))
            {
                if (p_ppu->viewport.x > 0)
                {
                    //discard pixel;
                    p_ppu->viewport.x -= 1;
                }
                else
                {
                    // Shift pixel to LCD.
                    p_ppu->lcd->pixels[p_ppu->status.pixel_index] = p_ppu->bg_palette.color[data.data];
                    p_ppu->status.pixel_index++;
                }

                if (0 == (p_ppu->status.pixel_index % 160))
                {
                    //Finished LCD line
                    //Trigger HBLANK IRQ (STAT).
                    p_ppu->status.mode = PPU_MODE_H_BLANK;
                }
            }
        }
        break;
    case PPU_MODE_H_BLANK:
        if ((43 + 51) == p_ppu->status.cycles)
        {
            p_ppu->status.cycles = 0;
            p_ppu->status.line_y += 1;
            if (144 == p_ppu->status.line_y)
            {
                // Trigger VBLANK IRQ.
                //Trigger VBLANK IRQ (STAT).
                p_ppu->status.mode = PPU_MODE_V_BLANK;
            }
            else
            {
                //Trigger OAM IRQ (STAT).
                p_ppu->status.mode = PPU_MODE_OAM_SEARCH;
            }
        }
        break;
    case PPU_MODE_V_BLANK:
        if (114 == p_ppu->status.cycles)
        {
            p_ppu->status.cycles = 0;
            p_ppu->status.line_y += 1;
            if (154 == p_ppu->status.line_y)
            {
                p_ppu->status.line_y = 0;
                //Trigger OAM IRQ (STAT).
                p_ppu->status.mode = PPU_MODE_OAM_SEARCH;
            }
        }
        break;
    }
}

/*
static void ppu_update(ppu_t *p_ppu)
{
    uint8_t lcdc = p_ppu->mem[0xFF40];

    p_ppu->enable = ((lcdc >> 7) & 0x01) ? 1 : 0;
    p_ppu->window_tile_map = ((lcdc >> 6) & 0x01) ? 0x9C00 : 0x9800; // 32 * 32 indexes (1k)
    p_ppu->window_enable = ((lcdc >> 5) & 0x01) ? 1 : 0;
    p_ppu->window_bg_tile_data = ((lcdc >> 4) & 0x01) ? 0x8000 : 0x8800; // 256 tiles * 16 bytes (4k)
    p_ppu->bg_tile_map = ((lcdc >> 3) & 0x01) ? 0x9C00 : 0x9800;         // 32 * 32 indexes (1k)
    p_ppu->sprite_height = ((lcdc >> 2) & 0x01) ? 16 : 8;
    p_ppu->sprite_enable = ((lcdc >> 1) & 0x01) ? 1 : 0;
    p_ppu->bg_window_enable = ((lcdc >> 0) & 0x01) ? 1 : 0;

    // sprites_tile_data = 0x8000;

    uint8_t stat = p_ppu->mem[0xFF41];

    p_ppu->scroll_y = p_ppu->mem[0xFF42];
    p_ppu->scroll_x = p_ppu->mem[0xFF43];
    p_ppu->mem[0xFF44] = p_ppu->line_y;
    p_ppu->line_y_compare = p_ppu->mem[0xFF46];

    int i, j;
    for (i = 0; i < 40; i++)
    {
        p_ppu->sprites[i].y = p_ppu->mem[0xFE00 + (4 * i) + 0];
        p_ppu->sprites[i].x = p_ppu->mem[0xFE00 + (4 * i) + 1];
        p_ppu->sprites[i].tile = p_ppu->mem[0xFE00 + (4 * i) + 2];
        uint8_t flags = p_ppu->mem[0xFE00 + (4 * i) + 3];

        p_ppu->sprites[i].priority = (flags >> 7) & 0x01;
        p_ppu->sprites[i].flip_y = (flags >> 6) & 0x01;
        p_ppu->sprites[i].flip_x = (flags >> 5) & 0x01;
        p_ppu->sprites[i].palette = (flags >> 4) & 0x01;
    }
}
*/
