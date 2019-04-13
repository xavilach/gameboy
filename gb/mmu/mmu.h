#ifndef MMU_H_
#define MMU_H_

#include <stdint.h>

typedef struct mmu_s mmu_t;

mmu_t *mmu_allocate(void);

int mmu_load(mmu_t *p_mmu, char *rom_path, char *boot_path);
int mmu_execute(mmu_t *p_mmu);

int mmu_read_u8(mmu_t *p_mmu, uint16_t address, uint8_t *data);
int mmu_write_u8(mmu_t *p_mmu, uint16_t address, uint8_t data);

int mmu_read_u16(mmu_t *p_mmu, uint16_t address, uint16_t *data);
int mmu_write_u16(mmu_t *p_mmu, uint16_t address, uint16_t data);

void mmu_free(mmu_t *p_mmu);

#endif /*MMU_H_*/