#ifndef MMU_H_
#define MMU_H_

#include <stdint.h>

typedef struct mmu_s mmu_t;

mmu_t *mmu_allocate(void);

int mmu_read(mmu_t *p_mmu, uint16_t address, uint8_t *data, uint16_t size);
int mmu_write(mmu_t *p_mmu, uint16_t address, uint8_t *data, uint16_t size);

int mmu_read_u8(mmu_t *p_mmu, uint16_t address, uint8_t *data);
int mmu_write_u8(mmu_t *p_mmu, uint16_t address, uint8_t data);

int mmu_read_u16(mmu_t *p_mmu, uint16_t address, uint16_t *data);
int mmu_write_u16(mmu_t *p_mmu, uint16_t address, uint16_t data);

#endif /*MMU_H_*/