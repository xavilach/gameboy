#include "ppu.h"

#include "ppu_base.h"

#include <stdint.h>
#include <stdlib.h>

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

    set_enabled(p_ppu);

    if (!p_ppu->status.enabled)
        return 1;

    switch (p_ppu->status.mode)
    {
    case PPU_MODE_OAM_SEARCH:
        //find 10 first sprites visible on line p_ppu->line_y.
        // (oam.x != 0) && ((ly + 16) >= (oam.y)) && ((ly + 16) < oam.y + h)

        //cpu cannot access oam

        if (p_ppu->status.cycles >= 20 * 40)
        {
            update_palettes(p_ppu);
            update_viewport(p_ppu);
            update_window(p_ppu);
            update_background(p_ppu);

            p_ppu->status.cycles = 0;
            p_ppu->status.mode = PPU_MODE_PIXEL_TRANSFER;
            p_ppu->status.pixel_index = 0;

            p_ppu->fetcher.cycles = 0;
            p_ppu->fetcher.mode = FETCHER_GET_TILE;
            p_ppu->fetcher.map_address = p_ppu->background.map_address;
            p_ppu->fetcher.tiles_address = p_ppu->background.tiles_address;
            p_ppu->fetcher.x = 0;
            p_ppu->fetcher.y = p_ppu->status.line_y + p_ppu->viewport.y;
        }
        break;
    case PPU_MODE_PIXEL_TRANSFER:
        //cpu cannot access vram
        //cpu cannot access oam

        fetch(&p_ppu->fetcher, p_ppu->mmu, &p_ppu->fifo);

        if ((p_ppu->fifo.count > 8) && (p_ppu->status.pixel_index < 160))
        {
            //more than 8 pixels, pop pixel from fifo.
            pixel_data_t data;
            if (0 == ppu_fifo_pop(&p_ppu->fifo, &data))
            {
                if (p_ppu->viewport.x > 0)
                {
                    //discard pixel;
                    p_ppu->viewport.x -= 1;
                }
                else
                {
                    // Shift pixel to LCD.
                    int index = p_ppu->status.line_y * 160;
                    index += p_ppu->status.pixel_index;

                    if (index < (160 * 144))
                    {
                        p_ppu->screen->buffer[index] = p_ppu->bg_palette.color[data.data];
                    }
                    p_ppu->status.pixel_index++;
                }
            }
        }

        if (p_ppu->status.pixel_index >= 160)
        {
            //Finished LCD line
            //Trigger HBLANK IRQ (STAT).
            p_ppu->status.mode = PPU_MODE_H_BLANK;
        }
        break;
    case PPU_MODE_H_BLANK:
        if (p_ppu->status.cycles >= (43 + 51) * 4)
        {
            p_ppu->status.cycles = 0;
            p_ppu->status.line_y += 1;

            update_lineY(p_ppu);

            if (p_ppu->status.line_y >= 144)
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

            //DEBUG_PRINT("New line %d\n", p_ppu->status.line_y);
        }
        break;
    case PPU_MODE_V_BLANK:
        if (p_ppu->status.cycles >= 114 * 4)
        {
            p_ppu->status.cycles = 0;
            p_ppu->status.line_y += 1;

            update_lineY(p_ppu);

            if (p_ppu->status.line_y >= 154)
            {
                p_ppu->status.line_y = 0;
                //Trigger OAM IRQ (STAT).
                p_ppu->status.mode = PPU_MODE_OAM_SEARCH;
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

/*
static void ppu_update(ppu_t *p_ppu)
{
    uint8_t lcdc = p_ppu->mem[0xFF40];

    p_ppu->enable = ((lcdc >> 7) & 0x01) ? 1 : 0;
    p_ppu->sprite_height = ((lcdc >> 2) & 0x01) ? 16 : 8;
    p_ppu->sprite_enable = ((lcdc >> 1) & 0x01) ? 1 : 0;
    
    // sprites_tile_data = 0x8000;

    uint8_t stat = p_ppu->mem[0xFF41];

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
