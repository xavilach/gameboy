#ifndef PPU_H_
#define PPU_H_

#include "mmu.h"
#include "lcd.h"

typedef struct ppu_s ppu_t;

/* Pixel Processing Unit.
160 * 144 PIxels
4 Shades of Grey
8*8 pixels tile-based, 20*18 tiles
40 sprites (10 per line)
8 kB VRAM

16 Bytes per tile.

Background map
256 tiles
32*32 tiles = 256*256 pixels

0xFF40 LCDC LCD Control.
-> LCDC7 LCD Display Enable
-> LCDC6 Window Tile Map Address
-> LCDC5 Window enable
-> LCDC4 BG & Window Tile data
-> LCDC3 BG Tile Map address
-> LCDC2 OBJ Size
-> LCDC1 OBJ Enable
-> LCDC0 BG Enable
0xFF41 STAT LCDC Status.
-> STAT6 LYC=LY Interrupt
-> STAT5 Mode 2 OAM Interrupt
-> STAT4 Mode 1 VBlank Interrupt
-> STAT3 Mode 0 HBlank Interrupt
-> STAT2 LYC=LY Flag
-> STAT1-0 Mode
0xFF42 SCY  Scroll Y.
0xFF43 SCX  Scroll X.
0xFF44 LY   LCDC Y-Coordinate.
0xFF45 LYC  LY Compare.
0xFF46 DMA  DMA Transfer and Start address.
0xFF47 BGP  BG Palette Data.
-> BGP6-7 Color for 11
-> BGP4-5 Color for 10
-> BGP2-3 Color for 01
-> BGP0-1 Color for 00
0xFF48 OBP0 Object Palette 0 Data.
0xFF49 OBP1 Object Palette 1 Data.
0xFF4A WY   Window Y Position.
0xFF4B WX   Window X Position - 7;

0xFE00 - FE9F (40 entries of 4 bytes)
OAM Entry
X
Y
Tile
Priority
Flip X
Flip Y
Palette

Each line * 144:
20 Clocks : OAM Search
43 Clocks : Pixel Transfer
51 Clocks : H-Blank

V-Blank = 10 lines

Pixel FIFO 16 pixels
Fetcher

*/

ppu_t *ppu_allocate(mmu_t *p_mmu, lcd_t *p_lcd);

void ppu_cycle(ppu_t *p_ppu);

uint8_t ppu_get_pixel(ppu_t *p_ppu, int x, int y);

#endif /*PPU_H_*/