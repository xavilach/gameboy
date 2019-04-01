#include "mmu.h"

#include <stdlib.h>

#define MEM_SIZE (0x10000)

typedef struct mmu_s
{
    uint8_t *mem;
} mmu_t;

mmu_t *mmu_allocate(void)
{
    mmu_t *p_mmu = calloc(1, sizeof(mmu_t));

    if (p_mmu)
    {
        p_mmu->mem = calloc(MEM_SIZE, sizeof(uint8_t));
    }
}

int mmu_read(mmu_t *p_mmu, uint16_t address, uint8_t *data, uint16_t size)
{
    if (!p_mmu || !data || !size)
        return -1;

    for (uint16_t index = 0; index < size; index++)
    {
        uint16_t addr = address + index;
        data[index] = p_mmu->mem[addr];
    }

    return 0;
}

int mmu_write(mmu_t *p_mmu, uint16_t address, uint8_t *data, uint16_t size)
{
    if (!p_mmu || !data || !size)
        return -1;

    for (uint16_t index = 0; index < size; index++)
    {
        uint16_t addr = address + index;
        p_mmu->mem[addr] = data[index];
    }

    return 0;
}

int mmu_read_u8(mmu_t *p_mmu, uint16_t address, uint8_t *data)
{
    if (!p_mmu || !data)
        return -1;

    *data = p_mmu->mem[address];

    return 0;
}

int mmu_write_u8(mmu_t *p_mmu, uint16_t address, uint8_t data)
{
    if (!p_mmu)
        return -1;

    p_mmu->mem[address] = data;

    return 0;
}

int mmu_read_u16(mmu_t *p_mmu, uint16_t address, uint16_t *data)
{
    if (!p_mmu || !data)
        return -1;

    *data = p_mmu->mem[address + 1];
    *data <<= 8;
    *data |= p_mmu->mem[address];

    return 0;
}

int mmu_write_u16(mmu_t *p_mmu, uint16_t address, uint16_t data)
{
    if (!p_mmu)
        return -1;

    p_mmu->mem[address] = data;
    p_mmu->mem[address + 1] = data >> 8;

    return 0;
}