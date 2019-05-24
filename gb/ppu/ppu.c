#include "ppu.h"

#include "ppu_fetcher.h"
#include "ppu_regs.h"
#include "ppu_def.h"

#include <stdint.h>
#include <stdlib.h>

#define PPU_OAM_ADDRESS (0xFE00)
#define PPU_OAM_ENTRY_SIZE (4)

#define PPU_DISPLAY_LINES (144)
#define PPU_VBLANK_LINES (10)

#define PPU_MODE_0_CYCLES (51)
#define PPU_MODE_1_CYCLES (114)
#define PPU_MODE_2_CYCLES (20)
#define PPU_MODE_3_CYCLES (43)

static void ppu_load_oam_entries(ppu_t *p_ppu);

static void ppu_draw_background(ppu_t *p_ppu);
static void ppu_draw_window(ppu_t *p_ppu);
static void ppu_draw_sprites(ppu_t *p_ppu);
static void ppu_draw_line(ppu_t *p_ppu);

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

    int was_enabled = p_ppu->status.enabled;

    ppu_reg_read_lcdc(p_ppu);

    if (!p_ppu->status.enabled)
    {
        if (was_enabled)
        {
            //TODO reset ppu.
            p_ppu->status.mode = PPU_MODE_OAM_SEARCH;
            p_ppu->status.cycles = 0;
            p_ppu->status.line_y = 0;
            p_ppu->status.pixel_index = 0;
        }
        return 4;
    }

    p_ppu->status.cycles += 1;

    switch (p_ppu->status.mode)
    {
    case PPU_MODE_OAM_SEARCH:
        //TODO cpu cannot access oam

        if (p_ppu->status.cycles >= PPU_MODE_2_CYCLES)
        {
            ppu_reg_read_sc(p_ppu);
            ppu_reg_read_lyc(p_ppu);
            ppu_reg_read_bgp_obp(p_ppu);
            ppu_reg_read_w(p_ppu);

            ppu_load_oam_entries(p_ppu);

            p_ppu->status.mode = PPU_MODE_PIXEL_TRANSFER;
            p_ppu->status.cycles = 0;
            p_ppu->status.pixel_index = 0;

            ppu_reg_write_stat(p_ppu);
        }
        break;

    case PPU_MODE_PIXEL_TRANSFER:
        //cpu cannot access vram
        //cpu cannot access oam

        p_ppu->status.pixel_index += 1;

        if (p_ppu->status.cycles >= PPU_MODE_3_CYCLES)
        {
            ppu_draw_background(p_ppu);
            ppu_draw_window(p_ppu);
            ppu_draw_sprites(p_ppu);

            ppu_draw_line(p_ppu);

            p_ppu->status.mode = PPU_MODE_H_BLANK;
            p_ppu->status.cycles = 0;

            ppu_reg_write_stat(p_ppu);
        }
        break;

    case PPU_MODE_H_BLANK:
        if (p_ppu->status.cycles >= PPU_MODE_0_CYCLES)
        {
            p_ppu->status.cycles = 0;
            p_ppu->status.line_y += 1;

            ppu_reg_write_LY(p_ppu);

            if (p_ppu->status.line_y >= PPU_DISPLAY_LINES)
            {
                p_ppu->status.mode = PPU_MODE_V_BLANK;
                ppu_reg_write_stat(p_ppu);
            }
            else
            {
                p_ppu->status.mode = PPU_MODE_OAM_SEARCH;
                ppu_reg_write_stat(p_ppu);
            }
        }
        break;

    case PPU_MODE_V_BLANK:
        if (p_ppu->status.cycles >= PPU_MODE_1_CYCLES)
        {
            p_ppu->status.cycles = 0;
            p_ppu->status.line_y += 1;

            ppu_reg_write_LY(p_ppu);

            if (p_ppu->status.line_y >= (PPU_DISPLAY_LINES + PPU_VBLANK_LINES))
            {
                p_ppu->status.line_y = 0;
                p_ppu->status.mode = PPU_MODE_OAM_SEARCH;
                ppu_reg_write_stat(p_ppu);
            }
        }
        break;
    }

    return 4;
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
        /* Retrieve OAM entry. */
        uint8_t y = 0;
        (void)mmu_read_u8(p_ppu->mmu, PPU_OAM_ADDRESS + (PPU_OAM_ENTRY_SIZE * s) + 0, &y);
        p_ppu->sprites.entries[s].y = y;

        uint8_t x = 0;
        (void)mmu_read_u8(p_ppu->mmu, PPU_OAM_ADDRESS + (PPU_OAM_ENTRY_SIZE * s) + 1, &x);
        p_ppu->sprites.entries[s].x = x;

        uint8_t tile_index = 0;
        (void)mmu_read_u8(p_ppu->mmu, PPU_OAM_ADDRESS + (PPU_OAM_ENTRY_SIZE * s) + 2, &tile_index);
        p_ppu->sprites.entries[s].tile_index = tile_index;

        uint8_t flags = 0;
        (void)mmu_read_u8(p_ppu->mmu, PPU_OAM_ADDRESS + (PPU_OAM_ENTRY_SIZE * s) + 3, &flags);
        p_ppu->sprites.entries[s].flags.priority = (flags >> 7) & 0x01;
        p_ppu->sprites.entries[s].flags.flip_y = (flags >> 6) & 0x01;
        p_ppu->sprites.entries[s].flags.flip_x = (flags >> 5) & 0x01;
        p_ppu->sprites.entries[s].flags.palette = (flags >> 4) & 0x01;

        /* Retrieve tile. */
        uint16_t tile_address = p_ppu->sprites.tiles_address + (tile_index * 16);
        for (int i = 0; i < 16; i++)
        {
            uint8_t data = 0;
            (void)mmu_read_u8(p_ppu->mmu, tile_address + i, &data);
            p_ppu->sprites.entries[s].tile.data[i] = data;
        }

        /* Update list of visible tiles for the current line. */
        uint8_t ly = p_ppu->status.line_y + 16;
        if ((0 != x) && (ly >= y) && (ly < y + 8) && (v < 10))
        {
            p_ppu->sprites.visibles[v] = s;
            v++;
        }
    }

    /* Fill the rest of the list with invalid sprites. */
    for (; v < 10; v++)
    {
        p_ppu->sprites.visibles[v] = -1;
    }
}

static void ppu_draw_background(ppu_t *p_ppu)
{
    uint8_t y = p_ppu->status.line_y + p_ppu->viewport.y;

    for (int x = 0; x < 32; x++)
    {
        tile_data_t tile;

        /* Fetch background tile index from background map. */
        uint16_t tile_y = y / 8;
        uint16_t tile_x = x;
        uint16_t tile_addr = p_ppu->background.map_address + (tile_y * 32) + tile_x;

        (void)mmu_read_u8(p_ppu->mmu, tile_addr, &tile.index);

        /* Fetch first byte of background tile data from background tiles. */
        uint16_t data_addr = p_ppu->background.tiles_address;
        data_addr += (uint16_t)tile.index * (uint16_t)16;
        data_addr += (y % 8) * 2;

        (void)mmu_read_u8(p_ppu->mmu, data_addr, &tile.data[0]);

        /* Fetch second byte of background tile data from background tiles. */
        data_addr += 1;

        (void)mmu_read_u8(p_ppu->mmu, data_addr, &tile.data[1]);

        for (int p = 0; p < 8; p++)
        {
            uint8_t pixel_index = (x * 8) + p;
            if (pixel_index >= p_ppu->viewport.x)
            {
                if ((pixel_index - p_ppu->viewport.x) >= 160)
                {
                    /* Reached end of line. */
                    return;
                }

                pixel_data_t *data = &p_ppu->line[pixel_index - p_ppu->viewport.x];
                data->type = PIXEL_TYPE_BACKGROUND;
                data->data = 0;
                if (p_ppu->background.enabled)
                {
                    data->data = (tile.data[0] >> (7 - p)) & 0x01;
                    data->data <<= 1;
                    data->data |= (tile.data[1] >> (7 - p)) & 0x01;
                }
            }
        }
    }
}

static void ppu_draw_window(ppu_t *p_ppu)
{
    if (p_ppu->status.line_y >= p_ppu->window.y)
    {
        uint8_t y = p_ppu->status.line_y - p_ppu->window.y;

        for (int x = 0; x < 32; x++)
        {
            tile_data_t tile;

            /* Fetch background tile index from background map. */
            uint16_t tile_y = y / 8;
            uint16_t tile_x = x;
            uint16_t tile_addr = p_ppu->window.map_address + (tile_y * 32) + tile_x;

            (void)mmu_read_u8(p_ppu->mmu, tile_addr, &tile.index);

            /* Fetch first byte of background tile data from background tiles. */
            uint16_t data_addr = p_ppu->background.tiles_address;
            data_addr += (uint16_t)tile.index * (uint16_t)16;
            data_addr += (y % 8) * 2;

            (void)mmu_read_u8(p_ppu->mmu, data_addr, &tile.data[0]);

            /* Fetch second byte of background tile data from background tiles. */
            data_addr += 1;

            (void)mmu_read_u8(p_ppu->mmu, data_addr, &tile.data[1]);

            for (int p = 0; p < 8; p++)
            {
                uint8_t pixel_index = ((p_ppu->window.x + x) * 8) + p;

                if ((pixel_index + p_ppu->window.x) >= 160)
                {
                    /* Reached end of line. */
                    return;
                }

                pixel_data_t *data = &p_ppu->line[pixel_index - p_ppu->viewport.x];
                data->type = PIXEL_TYPE_BACKGROUND;
                data->data = 0;
                if (p_ppu->background.enabled)
                {
                    data->data = (tile.data[0] >> (7 - p)) & 0x01;
                    data->data <<= 1;
                    data->data |= (tile.data[1] >> (7 - p)) & 0x01;
                }
            }
        }
    }
}

static void ppu_draw_sprites(ppu_t *p_ppu)
{
}

static void ppu_draw_line(ppu_t *p_ppu)
{
    for (int p = 0; p < 160; p++)
    {
        pixel_data_t *data = &p_ppu->line[p];

        int index = (p_ppu->status.line_y * 160) + p;

        if (index < (160 * 144))
        {
            uint8_t pixel_r = 0;
            uint8_t pixel_g = 0;
            uint8_t pixel_b = 0;

            switch (data->type)
            {
            case PIXEL_TYPE_BACKGROUND:
                pixel_r = p_ppu->bg_palette.color[data->data];
                //pixel_g = p_ppu->bg_palette.color[data.data];
                //pixel_b = p_ppu->bg_palette.color[data.data];

                pixel_r = (data->data != 0) ? 255 : 0;
                break;

            case PIXEL_TYPE_WINDOW:
                //pixel_r = p_ppu->bg_palette.color[data.data];
                pixel_g = p_ppu->bg_palette.color[data->data];
                //pixel_b = p_ppu->bg_palette.color[data.data];
                break;

            case PIXEL_TYPE_SPRITE:
                //pixel_r = p_ppu->obj_palettes[0].color[data.data];
                //pixel_g = p_ppu->obj_palettes[0].color[data.data];
                pixel_b = p_ppu->obj_palettes[0].color[data->data];
                break;

            default:
                break;
            }

            p_ppu->screen->buffer[(index * 3) + 0] = pixel_r;
            p_ppu->screen->buffer[(index * 3) + 1] = pixel_g;
            p_ppu->screen->buffer[(index * 3) + 2] = pixel_b;
        }
    }
}
