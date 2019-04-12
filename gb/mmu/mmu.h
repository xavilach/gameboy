#ifndef MMU_H_
#define MMU_H_

#include <stdint.h>

/*
0x0000 - 0x8000 ROM
0x0000 - 0x00FF Boot ROM
0x0000 - 0x4000 ROM Bank 0
0x4000 - 0x8000 ROM Bank 1-n

0x8000 - 0xA000 Video RAM
0xA000 - 0xC000 External RAM 0-n
0xC000 - 0xE000 RAM

0xFE00 - 0xFEA0 OAM RAM

0xFF00 - 0xFF80 I/O
0xFF80 - 0xFFFF HRAM
*/

typedef struct mmu_s mmu_t;

mmu_t *mmu_allocate(void);

int mmu_load_boot(mmu_t *p_mmu, char *path);
int mmu_load_rom(mmu_t *p_mmu, char *path);

int mmu_read_u8(mmu_t *p_mmu, uint16_t address, uint8_t *data);
int mmu_write_u8(mmu_t *p_mmu, uint16_t address, uint8_t data);

int mmu_read_u16(mmu_t *p_mmu, uint16_t address, uint16_t *data);
int mmu_write_u16(mmu_t *p_mmu, uint16_t address, uint16_t data);

void mmu_free(mmu_t *p_mmu);

#endif /*MMU_H_*/