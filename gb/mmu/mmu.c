#include "mmu.h"

#include <stdlib.h>
#include <stdio.h>

#define BOOT_ENABLE_REG (0xFF50)

#define BOOT_SIZE (0x100)
#define ROM_SIZE (0x8000)
#define MEM_SIZE (0x10000)

typedef struct mmu_s
{
    uint8_t *boot;
    uint8_t *mem;
} mmu_t;

static inline void set_boot_enabled(mmu_t *p_mmu, uint8_t enabled)
{
    p_mmu->mem[BOOT_ENABLE_REG] = enabled;
}

static inline int get_boot_enabled(mmu_t *p_mmu)
{
    return (0 != p_mmu->mem[BOOT_ENABLE_REG]);
}

static int load_file(char *path, void *mem, uint16_t size);

mmu_t *mmu_allocate(void)
{
    mmu_t *p_mmu = calloc(1, sizeof(mmu_t));

    if (p_mmu)
    {
        p_mmu->boot = calloc(BOOT_SIZE, sizeof(uint8_t));
        p_mmu->mem = calloc(MEM_SIZE, sizeof(uint8_t));

        if (!p_mmu->boot || !p_mmu->mem)
        {
            mmu_free(p_mmu);
            p_mmu = NULL;
        }
    }

    return p_mmu;
}

int mmu_load_boot(mmu_t *p_mmu, char *path)
{
    if (!p_mmu || !path)
        return -1;

    return load_file(path, p_mmu->boot, BOOT_SIZE);
}

int mmu_load_rom(mmu_t *p_mmu, char *path)
{
    if (!p_mmu || !path)
        return -1;

    return load_file(path, p_mmu->mem, ROM_SIZE);
}

int mmu_read_u8(mmu_t *p_mmu, uint16_t address, uint8_t *data)
{
    if (!p_mmu || !data)
        return -1;

    if (get_boot_enabled(p_mmu) && (address < BOOT_SIZE))
    {
        *data = p_mmu->boot[address];
    }
    else if (address < MEM_SIZE)
    {
        *data = p_mmu->mem[address];
    }

    return 0;
}

int mmu_write_u8(mmu_t *p_mmu, uint16_t address, uint8_t data)
{
    if (!p_mmu)
        return -1;

    if (address < MEM_SIZE)
    {
        p_mmu->mem[address] = data;
    }

    return 0;
}

int mmu_read_u16(mmu_t *p_mmu, uint16_t address, uint16_t *data)
{
    if (!p_mmu || !data)
        return -1;

    if (get_boot_enabled(p_mmu) && (address < (BOOT_SIZE - 1)))
    {
        *data = p_mmu->boot[address + 1];
        *data <<= 8;
        *data |= p_mmu->boot[address];
    }
    else if (address < (MEM_SIZE - 1))
    {
        *data = p_mmu->mem[address + 1];
        *data <<= 8;
        *data |= p_mmu->mem[address];
    }

    return 0;
}

int mmu_write_u16(mmu_t *p_mmu, uint16_t address, uint16_t data)
{
    if (!p_mmu)
        return -1;

    if (address < (MEM_SIZE - 1))
    {
        p_mmu->mem[address] = data;
        p_mmu->mem[address + 1] = data >> 8;
    }

    return 0;
}

void mmu_free(mmu_t *p_mmu)
{
    if (p_mmu)
    {
        if (p_mmu->boot)
        {
            free(p_mmu->boot);
            p_mmu->boot = NULL;
        }

        if (p_mmu->mem)
        {
            free(p_mmu->mem);
            p_mmu->mem = NULL;
        }
    }
}

static int load_file(char *path, void *mem, uint16_t size)
{
    FILE *file = fopen(path, "rb");

    if (!file)
    {
        return -1;
    }

    /* Retrieve file size. */
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size != file_size)
    {
        fclose(file);
        return -1;
    }

    (void)fread(mem, size, 1, file);

    fclose(file);
    return 0;
}
