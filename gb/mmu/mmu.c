#include "mmu.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BOOT_ENABLE_REG (0xFF50)

#define MEM_SIZE (0x10000)

#define BOOT_SIZE (regions_init[REGION_BOOT].end - regions_init[REGION_BOOT].start + 1)
#define ROM_SIZE (regions_init[REGION_ROM].end - regions_init[REGION_ROM].start + 1)

enum region_e
{
    REGION_BOOT = 0,
    REGION_ROM,
    REGION_VRAM,
    REGION_EXT_VRAM,
    REGION_RAM,
    REGION_ECHO_RAM,
    REGION_OAM_RAM,
    REGION_UNUSED,
    REGION_IO,
    REGION_HRAM,
    REGION_MAX
};

typedef struct region_s
{
    uint16_t start;
    uint16_t end;
    int read;
    int write;
} region_t;

typedef struct mmu_s
{
    uint8_t *boot;
    uint8_t *mem;
    region_t *regions;

    struct
    {
        int enabled;
        uint16_t source;
        uint16_t destination;
        uint16_t offset;
    } dma;

} mmu_t;

static const region_t regions_init[REGION_MAX] = {
    {0x0000, 0x00FF, 0, 0}, // Boot ROM
    {0x0000, 0x7FFF, 0, 0}, // ROM
    {0x8000, 0x9FFF, 0, 0}, // Video RAM
    {0xA000, 0xBFFF, 0, 0}, // External Video RAM
    {0xC000, 0xDFFF, 0, 0}, // RAM
    {0xE000, 0xFDFF, 0, 0}, // Echo RAM
    {0xFE00, 0xFE9F, 0, 0}, // OAM RAM
    {0xFEA0, 0xFEFF, 0, 0}, // Unused
    {0xFF00, 0xFF7F, 0, 0}, // IO
    {0xFF80, 0xFFFF, 0, 0}, // HRAM
};

static int load_file(char *path, void *mem, uint16_t size);

mmu_t *mmu_allocate(void)
{
    mmu_t *p_mmu = calloc(1, sizeof(mmu_t));

    if (p_mmu)
    {
        p_mmu->regions = malloc(sizeof(regions_init));

        if (p_mmu->regions)
        {
            (void)memcpy(p_mmu->regions, regions_init, sizeof(regions_init));
        }

        p_mmu->boot = calloc(BOOT_SIZE, sizeof(uint8_t));

        p_mmu->mem = calloc(MEM_SIZE, sizeof(uint8_t));

        if (!p_mmu->regions || !p_mmu->boot || !p_mmu->mem)
        {
            mmu_free(p_mmu);
            p_mmu = NULL;
        }
    }

    return p_mmu;
}

int mmu_load(mmu_t *p_mmu, char *rom_path, char *boot_path)
{
    if (!p_mmu)
    {
        return -1;
    }

    for (int r = 0; r < REGION_MAX; r++)
    {
        /* Disallow all accesses. */
        p_mmu->regions[r].read = 0;
        p_mmu->regions[r].write = 0;
    }

    int loaded_rom = load_file(rom_path, p_mmu->mem, ROM_SIZE);
    int loaded_boot = load_file(boot_path, p_mmu->boot, BOOT_SIZE);

    if ((loaded_rom < 0) && (loaded_boot < 0))
    {
        /* Nothing could be loaded. */
        return -1;
    }

    if (0 == loaded_rom)
    {
        printf("MMU loaded ROM from %s\n", rom_path);
        p_mmu->regions[REGION_ROM].read = 1;
        p_mmu->regions[REGION_EXT_VRAM].read = 1;
        p_mmu->regions[REGION_EXT_VRAM].write = 1;
    }

    if (0 == loaded_boot)
    {
        printf("MMU loaded BOOT ROM from %s\n", boot_path);
        p_mmu->regions[REGION_BOOT].read = 1;
    }

    p_mmu->regions[REGION_VRAM].read = 1;
    p_mmu->regions[REGION_VRAM].write = 1;
    p_mmu->regions[REGION_RAM].read = 1;
    p_mmu->regions[REGION_RAM].write = 1;
    p_mmu->regions[REGION_ECHO_RAM].read = 0;
    p_mmu->regions[REGION_ECHO_RAM].write = 0;
    p_mmu->regions[REGION_OAM_RAM].read = 1;
    p_mmu->regions[REGION_OAM_RAM].write = 1;
    p_mmu->regions[REGION_UNUSED].read = 1;
    p_mmu->regions[REGION_UNUSED].write = 1;
    p_mmu->regions[REGION_IO].read = 1;
    p_mmu->regions[REGION_IO].write = 1;
    p_mmu->regions[REGION_HRAM].read = 1;
    p_mmu->regions[REGION_HRAM].write = 1;

    return 0;
}

int mmu_execute(mmu_t *p_mmu)
{
    if (p_mmu->dma.enabled)
    {
        uint16_t destination = p_mmu->dma.destination + p_mmu->dma.offset;
        uint16_t source = p_mmu->dma.source + p_mmu->dma.offset;

        p_mmu->mem[destination] = p_mmu->mem[source];

        p_mmu->dma.offset += 1;
        if (p_mmu->dma.offset >= 160)
        {
            p_mmu->dma.enabled = 0;
            printf("MMU: Finished DMA transfer from 0x%04x to 0x%04x\n", p_mmu->dma.source + p_mmu->dma.offset - 1, p_mmu->dma.destination + p_mmu->dma.offset - 1);
        }
    }
    return 1;
}

int mmu_read_u8(mmu_t *p_mmu, uint16_t address, uint8_t *data)
{
    if (!p_mmu || !data)
        return -1;

    for (int r = 0; r < REGION_MAX; r++)
    {
        region_t *p_region = p_mmu->regions + r;
        if (p_region->read && (p_region->start <= address) && (address <= p_region->end))
        {
            //TODO Nicer way.
            if (REGION_BOOT == r)
            {
                *data = p_mmu->boot[address];
            }
            else if (REGION_ECHO_RAM == r)
            {
                *data = p_mmu->mem[address - 0x2000];
            }
            else if (REGION_UNUSED == r)
            {
                *data = 0;
            }
            else
            {
                *data = p_mmu->mem[address];
            }
            return 0;
        }
    }

    *data = 0;
    printf("MMU: Read access violation: 0x%04x\n", address);
    for (int r = 0; r < REGION_MAX; r++)
    {
        region_t *p_region = p_mmu->regions + r;

        printf("MMU: Region %d [0x%04x - 0x%04x] [R%d W%d]\n", r, p_region->start, p_region->end, p_region->read, p_region->write);
    }
    exit(-1);
    return -1;
}

int mmu_write_u8(mmu_t *p_mmu, uint16_t address, uint8_t data)
{
    if (!p_mmu)
        return -1;

    for (int r = 0; r < REGION_MAX; r++)
    {
        region_t *p_region = p_mmu->regions + r;
        if (p_region->read && (p_region->start <= address) && (address <= p_region->end))
        {
            if (REGION_ECHO_RAM == r)
            {
                p_mmu->mem[address - 0x2000] = data;
            }
            else if (REGION_UNUSED == r)
            {
                /* Ignore */
            }
            else
            {
                p_mmu->mem[address] = data;

                if (0xFF46 == address)
                {
                    if (!p_mmu->dma.enabled)
                    {
                        p_mmu->dma.enabled = 1;
                        p_mmu->dma.offset = 0;
                        p_mmu->dma.source = data;
                        p_mmu->dma.source <= 8;
                        p_mmu->dma.destination = 0xFE00;

                        printf("MMU: Starting DMA transfer from 0x%04x to 0x%04x\n", p_mmu->dma.source, p_mmu->dma.destination);
                    }
                }
                else if (0xFF50 == address)
                {
                    if (data)
                    {
                        /* Disable boot. */
                        p_mmu->regions[REGION_BOOT].read = 0;
                    }
                }
            }

            return 0;
        }
    }

    printf("MMU: Write access violation: 0x%04x\n", address);
    for (int r = 0; r < REGION_MAX; r++)
    {
        region_t *p_region = p_mmu->regions + r;

        printf("MMU: Region %d [0x%04x - 0x%04x] [R%d W%d]\n", r, p_region->start, p_region->end, p_region->read, p_region->write);
    }
    exit(-1);
    return -1;
}

int mmu_read_u16(mmu_t *p_mmu, uint16_t address, uint16_t *data)
{
    if (!p_mmu || !data)
        return -1;

    uint8_t value;
    if (mmu_read_u8(p_mmu, address + 1, &value) < 0)
    {
        return -1;
    }

    *data = value;
    *data <<= 8;

    if (mmu_read_u8(p_mmu, address, &value) < 0)
    {
        return -1;
    }

    *data |= value;
    return 0;
}

int mmu_write_u16(mmu_t *p_mmu, uint16_t address, uint16_t data)
{
    if (!p_mmu)
        return -1;

    if (mmu_write_u8(p_mmu, address, (uint8_t)data) < 0)
    {
        return -1;
    }

    if (mmu_write_u8(p_mmu, address + 1, (uint8_t)(data >> 8) < 0))
    {
        return -1;
    }

    return 0;
}

void mmu_free(mmu_t *p_mmu)
{
    if (p_mmu)
    {
        if (p_mmu->regions)
        {
            free(p_mmu->regions);
            p_mmu->regions = NULL;
        }

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
        printf("Failed to open file: %s\n", path);
        return -1;
    }

    /* Retrieve file size. */
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size > size)
    {
        printf("Failed to copy file: %s (%d > %d)\n", path, file_size, size);
        fclose(file);
        return -1;
    }

    (void)fread(mem, file_size, 1, file);

    fclose(file);
    return 0;
}
