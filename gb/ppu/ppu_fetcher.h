#ifndef PPU_FETCHER_H_
#define PPU_FETCHER_H_

#include "ppu_fifo.h"
#include "ppu_def.h"
#include "mmu.h"

#include <stdint.h>

static inline void fetch_reset(ppu_t *p_ppu)
{
    p_ppu->fetcher.cycles = 0;
    p_ppu->fetcher.mode = FETCHER_MODE_BACKGROUND;
    p_ppu->fetcher.state = FETCHER_GET_TILE;

    p_ppu->fetcher.background_x = 0;
    p_ppu->fetcher.background_y = p_ppu->status.line_y + p_ppu->viewport.y;

    p_ppu->fetcher.window_x = 0;
    p_ppu->fetcher.window_y = p_ppu->status.line_y - p_ppu->window.y;

    p_ppu->fetcher.oam_index = -1;
}

static inline void fetch_stop(ppu_t *p_ppu)
{
    p_ppu->fetcher.mode = FETCHER_MODE_BACKGROUND;
    p_ppu->fetcher.state = FETCHER_STOPPED;
}

static inline void fetch_background(ppu_t *p_ppu)
{
    switch (p_ppu->fetcher.state)
    {
    case FETCHER_GET_TILE:
    {
        /* Fetch background tile index from background map. */
        uint16_t line = (uint16_t)(p_ppu->fetcher.background_y / 8);
        uint16_t addr = p_ppu->background.map_address + (line * 32) + p_ppu->fetcher.background_x;

        uint8_t tile_index = 0;
        (void)mmu_read_u8(p_ppu->mmu, addr, &tile_index);

        p_ppu->fetcher.tile.index = tile_index;
        p_ppu->fetcher.state = FETCHER_GET_DATA_0;
    }
    break;

    case FETCHER_GET_DATA_0:
    {
        /* Fetch first byte of background tile data from background tiles. */
        uint16_t addr = p_ppu->background.tiles_address;
        addr += (uint16_t)p_ppu->fetcher.tile.index * (uint16_t)16;
        addr += (p_ppu->fetcher.background_y % 8) * 2;

        uint8_t tile_data0 = 0;
        (void)mmu_read_u8(p_ppu->mmu, addr, &tile_data0);

        p_ppu->fetcher.tile.data[0] = tile_data0;
        p_ppu->fetcher.state = FETCHER_GET_DATA_1;
    }
    break;

    case FETCHER_GET_DATA_1:
    {
        /* Fetch second byte of background tile data from background tiles. */
        uint16_t addr = p_ppu->background.tiles_address;
        addr += (uint16_t)p_ppu->fetcher.tile.index * (uint16_t)16;
        addr += (p_ppu->fetcher.background_y % 8) * 2;
        addr += 1;

        uint8_t tile_data1 = 0;
        (void)mmu_read_u8(p_ppu->mmu, addr, &tile_data1);

        p_ppu->fetcher.tile.data[1] = tile_data1;
        p_ppu->fetcher.state = FETCHER_WAIT;
    }
    break;

    case FETCHER_WAIT:
    {
        /* Data ready, wait for space in pixel fifo to be available. */
        if (p_ppu->fifo.count <= 8)
        {
            /* Enqueue pixel data, then restart fetch cycle. */

            pixel_data_t data;
            for (int p = 0; p < 8; p++)
            {
                data.type = PIXEL_TYPE_BACKGROUND;
                data.data = (p_ppu->fetcher.tile.data[0] >> (7 - p)) & 0x01;
                data.data <<= 1;
                data.data |= (p_ppu->fetcher.tile.data[1] >> (7 - p)) & 0x01;

                (void)ppu_fifo_push(&(p_ppu->fifo), data);
            }

            p_ppu->fetcher.state = FETCHER_GET_TILE;
            p_ppu->fetcher.background_x = (p_ppu->fetcher.background_x + 1) % 32;
        }
    }
    break;

    case FETCHER_STOPPED:
    default:
        /* Do nothing. */
        break;
    }
}

static inline void fetch_window(ppu_t *p_ppu)
{
    switch (p_ppu->fetcher.state)
    {
    case FETCHER_GET_TILE:
    {
        /* Fetch window tile index from window map. */
        uint16_t line = (uint16_t)(p_ppu->fetcher.window_y / 8);
        uint16_t addr = p_ppu->window.map_address + (line * 32) + p_ppu->fetcher.window_x;

        uint8_t tile_index = 0;
        (void)mmu_read_u8(p_ppu->mmu, addr, &tile_index);

        p_ppu->fetcher.tile.index = tile_index;
        p_ppu->fetcher.state = FETCHER_GET_DATA_0;
    }
    break;

    case FETCHER_GET_DATA_0:
    {
        /* Fetch first byte of window tile data from background tiles. */
        uint16_t addr = p_ppu->background.tiles_address;
        addr += (uint16_t)p_ppu->fetcher.tile.index * (uint16_t)16;
        addr += (p_ppu->fetcher.window_y % 8) * 2;

        uint8_t tile_data0 = 0;
        (void)mmu_read_u8(p_ppu->mmu, addr, &tile_data0);

        p_ppu->fetcher.tile.data[0] = tile_data0;
        p_ppu->fetcher.state = FETCHER_GET_DATA_1;
    }
    break;

    case FETCHER_GET_DATA_1:
    {
        /* Fetch second byte of window tile data from background tiles. */
        uint16_t addr = p_ppu->background.tiles_address;
        addr += (uint16_t)p_ppu->fetcher.tile.index * (uint16_t)16;
        addr += (p_ppu->fetcher.window_y % 8) * 2;
        addr += 1;

        uint8_t tile_data1 = 0;
        (void)mmu_read_u8(p_ppu->mmu, addr, &tile_data1);

        p_ppu->fetcher.tile.data[1] = tile_data1;
        p_ppu->fetcher.state = FETCHER_WAIT;
    }
    break;

    case FETCHER_WAIT:
    {
        /* Data ready, wait for space in pixel fifo to be available. */
        if (p_ppu->fifo.count <= 8)
        {
            /* Enqueue pixel data, then restart fetch cycle. */

            pixel_data_t data;
            for (int p = 0; p < 8; p++)
            {
                data.type = PIXEL_TYPE_WINDOW;
                data.data = (p_ppu->fetcher.tile.data[0] >> (7 - p)) & 0x01;
                data.data <<= 1;
                data.data |= (p_ppu->fetcher.tile.data[1] >> (7 - p)) & 0x01;

                (void)ppu_fifo_push(&(p_ppu->fifo), data);
            }

            p_ppu->fetcher.state = FETCHER_GET_TILE;
            p_ppu->fetcher.window_x = (p_ppu->fetcher.window_x + 1) % 32;
        }
    }
    break;

    case FETCHER_STOPPED:
    default:
        /* Do nothing. */
        break;
    }
}

static inline void fetch_sprite(ppu_t *p_ppu)
{
    switch (p_ppu->fetcher.state)
    {
    case FETCHER_GET_TILE:
    {
        /* Fetch sprite index from oam entry. */
        if ((p_ppu->fetcher.oam_index >= 0) && (p_ppu->fetcher.oam_index < 40))
        {
            oam_entry_t *entry = &(p_ppu->sprites.entries[p_ppu->fetcher.oam_index]);

            p_ppu->fetcher.tile.index = entry->tile_index;
            p_ppu->fetcher.state = FETCHER_GET_DATA_0;
        }
        else
        {
            /* Error */
            p_ppu->fetcher.state = FETCHER_STOPPED;
        }
    }
    break;

    case FETCHER_GET_DATA_0:
    {
        /* Fetch first byte of sprite tile data from sprite tiles. */
        oam_entry_t *entry = &p_ppu->sprites.entries[p_ppu->fetcher.oam_index];
        uint16_t line = (uint16_t)p_ppu->status.line_y - (uint16_t)(entry->y - 16);
        uint16_t addr = p_ppu->sprites.tiles_address;
        addr += (uint16_t)p_ppu->fetcher.tile.index * (uint16_t)16;
        addr += line * 2;

        uint8_t tile_data0 = 0;
        (void)mmu_read_u8(p_ppu->mmu, addr, &tile_data0);

        p_ppu->fetcher.tile.data[0] = tile_data0;
        p_ppu->fetcher.state = FETCHER_GET_DATA_1;
    }
    break;

    case FETCHER_GET_DATA_1:
    {
        /* Fetch second byte of sprite tile data from sprite tiles. */
        oam_entry_t *entry = &p_ppu->sprites.entries[p_ppu->fetcher.oam_index];
        uint16_t line = (uint16_t)p_ppu->status.line_y - (uint16_t)(entry->y - 16);
        if (entry->flags.flip_y)
        {
            line = 7 - line;
        }

        uint16_t addr = p_ppu->sprites.tiles_address;
        addr += (uint16_t)p_ppu->fetcher.tile.index * (uint16_t)16;
        addr += line * 2;
        addr += 1;

        uint8_t tile_data1 = 0;
        (void)mmu_read_u8(p_ppu->mmu, addr, &tile_data1);

        p_ppu->fetcher.tile.data[1] = tile_data1;
        p_ppu->fetcher.state = FETCHER_WAIT;
    }
    break;

    case FETCHER_WAIT:
    {
        /* Data ready. */
        if (p_ppu->fifo.count >= 8)
        {
            oam_entry_t *entry = &p_ppu->sprites.entries[p_ppu->fetcher.oam_index];

            for (int p = 0; p < 8; p++)
            {
                //TODO Pixel priority handling
                pixel_data_t *p_data = &(p_ppu->fifo.data[(p_ppu->fifo.read + p) % 16]);

                if (PIXEL_TYPE_SPRITE != p_data->type)
                {
                    p_data->type = PIXEL_TYPE_SPRITE;

                    if (entry->flags.flip_x)
                    {
                        p_data->data = (p_ppu->fetcher.tile.data[0] >> p) & 0x01;
                        p_data->data <<= 1;
                        p_data->data |= (p_ppu->fetcher.tile.data[1] >> p) & 0x01;
                    }
                    else
                    {
                        p_data->data = (p_ppu->fetcher.tile.data[0] >> (7 - p)) & 0x01;
                        p_data->data <<= 1;
                        p_data->data |= (p_ppu->fetcher.tile.data[1] >> (7 - p)) & 0x01;
                    }
                }
            }

            p_ppu->fetcher.state = FETCHER_STOPPED;
        }
    }
    break;

    case FETCHER_STOPPED:
    default:
        /* Do nothing. */
        break;
    }
}

static inline void fetch(ppu_t *p_ppu)
{
    /* Fetcher runs only every other cycles. */
    p_ppu->fetcher.cycles += 1;
    if (p_ppu->fetcher.cycles >= 2)
    {
        p_ppu->fetcher.cycles = 0;
        return;
    }

    if ((FETCHER_MODE_BACKGROUND == p_ppu->fetcher.mode) && (FETCHER_STOPPED == p_ppu->fetcher.state))
    {
        if (p_ppu->window.enabled && (p_ppu->status.line_y >= p_ppu->window.y) && (p_ppu->status.pixel_index >= p_ppu->window.x))
        {
            /* Go back to fetching window. */
            p_ppu->fetcher.cycles = 0;
            p_ppu->fetcher.mode = FETCHER_MODE_WINDOW;
            p_ppu->fetcher.state = FETCHER_GET_TILE;
        }
        else
        {
            /* Go back to fetching background. */
            p_ppu->fetcher.cycles = 0;
            p_ppu->fetcher.mode = FETCHER_MODE_BACKGROUND;
            p_ppu->fetcher.state = FETCHER_GET_TILE;
        }
    }

    if (FETCHER_MODE_BACKGROUND == p_ppu->fetcher.mode)
    {
        if (p_ppu->window.enabled && (p_ppu->status.line_y >= p_ppu->window.y) && (p_ppu->status.pixel_index >= p_ppu->window.x))
        {
            /* Clear pixel fifo and fetch window instead of background. */

            p_ppu->fetcher.cycles = 0;
            p_ppu->fetcher.mode = FETCHER_MODE_WINDOW;
            p_ppu->fetcher.state = FETCHER_GET_TILE;

            ppu_fifo_clear(&p_ppu->fifo);
        }
    }

    switch (p_ppu->fetcher.mode)
    {
    case FETCHER_MODE_BACKGROUND:
        fetch_background(p_ppu);
        break;

    case FETCHER_MODE_WINDOW:
        fetch_window(p_ppu);
        break;

    case FETCHER_MODE_SPRITE:
        fetch_sprite(p_ppu);
        break;

    default:
        break;
    }
}

static inline void fetch_run(ppu_t *p_ppu)
{
    /* Check if a sprite needs to be drawn. */
    if (FETCHER_MODE_SPRITE != p_ppu->fetcher.mode && p_ppu->sprites.enabled)
    {
        for (int v = 0; v < 10; v++)
        {
            int s = p_ppu->sprites.visibles[v];
            if (s >= 0 && s < 40)
            {
                oam_entry_t *entry = &(p_ppu->sprites.entries[s]);

                if (entry->x == p_ppu->status.pixel_index)
                {
                    /* Disable this entry. */
                    p_ppu->sprites.visibles[v] = -1;

                    /* Start fetching sprite. */
                    p_ppu->fetcher.oam_index = s;
                    p_ppu->fetcher.mode = FETCHER_MODE_SPRITE;
                    p_ppu->fetcher.state = FETCHER_GET_TILE;
                }
            }
        }
    }

    fetch(p_ppu);

    if ((FETCHER_MODE_SPRITE != p_ppu->fetcher.mode) && (p_ppu->fifo.count > 8) && (p_ppu->status.pixel_index < 160))
    {
        //more than 8 pixels, pop pixel from fifo.
        pixel_data_t data;
        if (0 == ppu_fifo_pop(&p_ppu->fifo, &data))
        {
            // Shift pixel to LCD.
            int index = (p_ppu->status.line_y * 160) + p_ppu->status.pixel_index;

            if (index < (160 * 144))
            {
                uint8_t pixel_r = 0;
                uint8_t pixel_g = 0;
                uint8_t pixel_b = 0;

                switch (data.type)
                {
                case PIXEL_TYPE_BACKGROUND:
                    pixel_r = p_ppu->bg_palette.color[data.data];
                    //pixel_g = p_ppu->bg_palette.color[data.data];
                    //pixel_b = p_ppu->bg_palette.color[data.data];

                    pixel_r = (data.data != 0) ? 255 : 0;
                    break;

                case PIXEL_TYPE_WINDOW:
                    //pixel_r = p_ppu->bg_palette.color[data.data];
                    pixel_g = p_ppu->bg_palette.color[data.data];
                    //pixel_b = p_ppu->bg_palette.color[data.data];
                    break;

                case PIXEL_TYPE_SPRITE:
                    //pixel_r = p_ppu->obj_palettes[0].color[data.data];
                    //pixel_g = p_ppu->obj_palettes[0].color[data.data];
                    pixel_b = p_ppu->obj_palettes[0].color[data.data];
                    break;

                default:
                    break;
                }

                p_ppu->screen->buffer[(index * 3) + 0] = pixel_r;
                p_ppu->screen->buffer[(index * 3) + 1] = pixel_g;
                p_ppu->screen->buffer[(index * 3) + 2] = pixel_b;
            }

            p_ppu->status.pixel_index++;
        }
    }
}

#endif /*PPU_FETCHER_H_*/