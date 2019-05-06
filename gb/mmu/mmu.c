#include "mmu.h"

#include "cartridge.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BOOT_ENABLE_REG (0xFF50)

#define RAM_OFFSET (0x8000)
#define RAM_SIZE (0x8000)

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

typedef int (*mmu_read_access_t)(mmu_t *p_mmu, uint16_t address, uint8_t *data);
typedef int (*mmu_write_access_t)(mmu_t *p_mmu, uint16_t address, uint8_t data);

typedef struct region_s
{
    uint16_t start;
    uint16_t end;
    mmu_read_access_t read;
    mmu_write_access_t write;
} region_t;

typedef struct mmu_s
{
    uint8_t *boot;
    uint8_t *ram;
    cartridge_t *cartridge;
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

/*******************************************/

static int load_file(char *path, void *mem, uint16_t size);

static region_t *mmu_find_readable_region(mmu_t *p_mmu, uint16_t address);
static region_t *mmu_find_writeable_region(mmu_t *p_mmu, uint16_t address);

static int mmu_read_boot(mmu_t *p_mmu, uint16_t address, uint8_t *data);
static int mmu_read_rom(mmu_t *p_mmu, uint16_t address, uint8_t *data);
static int mmu_write_rom(mmu_t *p_mmu, uint16_t address, uint8_t data);
static int mmu_read_ext_ram(mmu_t *p_mmu, uint16_t address, uint8_t *data);
static int mmu_write_ext_ram(mmu_t *p_mmu, uint16_t address, uint8_t data);
static int mmu_read_ram(mmu_t *p_mmu, uint16_t address, uint8_t *data);
static int mmu_write_ram(mmu_t *p_mmu, uint16_t address, uint8_t data);
static int mmu_read_echo_ram(mmu_t *p_mmu, uint16_t address, uint8_t *data);
static int mmu_write_echo_ram(mmu_t *p_mmu, uint16_t address, uint8_t data);
static int mmu_read_unused(mmu_t *p_mmu, uint16_t address, uint8_t *data);
static int mmu_write_unused(mmu_t *p_mmu, uint16_t address, uint8_t data);
static int mmu_read_io(mmu_t *p_mmu, uint16_t address, uint8_t *data);
static int mmu_write_io(mmu_t *p_mmu, uint16_t address, uint8_t data);

static void mmu_print_regions(mmu_t *p_mmu);

/*******************************************/

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

        p_mmu->ram = calloc(RAM_SIZE, sizeof(uint8_t));

        if (!p_mmu->regions || !p_mmu->boot || !p_mmu->ram)
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
        p_mmu->regions[r].read = NULL;
        p_mmu->regions[r].write = NULL;
    }

    p_mmu->cartridge = cartridge_allocate(rom_path);

    int loaded_boot = load_file(boot_path, p_mmu->boot, BOOT_SIZE);

    if ((!p_mmu->cartridge) && (loaded_boot < 0))
    {
        /* Nothing could be loaded. */
        return -1;
    }

    if (p_mmu->cartridge)
    {
        printf("MMU loaded ROM from %s\n", rom_path);
        p_mmu->regions[REGION_ROM].read = mmu_read_rom;
        p_mmu->regions[REGION_ROM].write = mmu_write_rom;

        p_mmu->regions[REGION_EXT_VRAM].read = mmu_read_ext_ram;
        p_mmu->regions[REGION_EXT_VRAM].write = mmu_write_ext_ram;

        printf("ROM header:\n");
        printf("Title:\t%s\n", p_mmu->cartridge->header.title);
        printf("Type:\t%02x\n", p_mmu->cartridge->header.type);
        printf("ROM size:\t%d\n", p_mmu->cartridge->header.rom_size);
        printf("RAM size:\t%d\n", p_mmu->cartridge->header.ram_size);
    }

    if (0 == loaded_boot)
    {
        printf("MMU loaded BOOT ROM from %s\n", boot_path);
        p_mmu->regions[REGION_BOOT].read = mmu_read_boot;
    }

    p_mmu->regions[REGION_VRAM].read = mmu_read_ram;
    p_mmu->regions[REGION_VRAM].write = mmu_write_ram;
    p_mmu->regions[REGION_RAM].read = mmu_read_ram;
    p_mmu->regions[REGION_RAM].write = mmu_write_ram;
    p_mmu->regions[REGION_ECHO_RAM].read = mmu_read_echo_ram;
    p_mmu->regions[REGION_ECHO_RAM].write = mmu_write_echo_ram;
    p_mmu->regions[REGION_OAM_RAM].read = mmu_read_ram;
    p_mmu->regions[REGION_OAM_RAM].write = mmu_write_ram;
    p_mmu->regions[REGION_HRAM].read = mmu_read_ram;
    p_mmu->regions[REGION_HRAM].write = mmu_write_ram;

    p_mmu->regions[REGION_UNUSED].read = mmu_read_unused;
    p_mmu->regions[REGION_UNUSED].write = mmu_write_unused;

    p_mmu->regions[REGION_IO].read = mmu_read_io;
    p_mmu->regions[REGION_IO].write = mmu_write_io;

    return 0;
}

int mmu_execute(mmu_t *p_mmu)
{
    if (p_mmu->dma.enabled)
    {
        uint16_t destination = p_mmu->dma.destination + p_mmu->dma.offset;
        uint16_t source = p_mmu->dma.source + p_mmu->dma.offset;

        uint8_t value;
        (void)mmu_read_u8(p_mmu, source, &value);
        (void)mmu_write_u8(p_mmu, destination, value);

        p_mmu->dma.offset += 1;
        if (p_mmu->dma.offset >= 160)
        {
            p_mmu->dma.enabled = 0;
            //printf("MMU: Finished DMA transfer from 0x%04x to 0x%04x\n", p_mmu->dma.source + p_mmu->dma.offset - 1, p_mmu->dma.destination + p_mmu->dma.offset - 1);
        }
    }
    return 1;
}

int mmu_read_u8(mmu_t *p_mmu, uint16_t address, uint8_t *data)
{
    if (!p_mmu || !data)
        return -1;

    region_t *p_region = mmu_find_readable_region(p_mmu, address);
    if (p_region)
    {
        int ret = p_region->read(p_mmu, address, data);
        if (ret < 0)
        {
            printf("MMU: Read access failure: 0x%04x\n", address);
        }
        return ret;
    }

    printf("MMU: Read access violation: 0x%04x\n", address);
    mmu_print_regions(p_mmu);
    exit(-1);
    return -1;
}

int mmu_write_u8(mmu_t *p_mmu, uint16_t address, uint8_t data)
{
    if (!p_mmu)
        return -1;

    region_t *p_region = mmu_find_writeable_region(p_mmu, address);
    if (p_region)
    {
        int ret = p_region->write(p_mmu, address, data);
        if (ret < 0)
        {
            printf("MMU: Write access failure: 0x%04x\n", address);
        }
        return ret;
    }

    printf("MMU: Write access violation: 0x%04x\n", address);
    mmu_print_regions(p_mmu);
    exit(-1);
    return -1;
}

int mmu_read_u16(mmu_t *p_mmu, uint16_t address, uint16_t *data)
{
    if (!p_mmu || !data)
        return -1;

    uint8_t lsb, msb;

    int ret = mmu_read_u8(p_mmu, address, &lsb);
    ret += mmu_read_u8(p_mmu, address + 1, &msb);

    if (ret < 0)
    {
        return -1;
    }

    *data = msb;
    *data <<= 8;
    *data |= lsb;

    return 0;
}

int mmu_write_u16(mmu_t *p_mmu, uint16_t address, uint16_t data)
{
    if (!p_mmu)
        return -1;

    int ret = mmu_write_u8(p_mmu, address, (uint8_t)data);
    ret += mmu_write_u8(p_mmu, address + 1, (uint8_t)(data >> 8));

    if (ret < 0)
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

        if (p_mmu->ram)
        {
            free(p_mmu->ram);
            p_mmu->ram = NULL;
        }

        if (p_mmu->cartridge)
        {
            cartridge_free(p_mmu->cartridge);
            p_mmu->cartridge = NULL;
        }
    }
}

/***************************************/

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

static region_t *mmu_find_readable_region(mmu_t *p_mmu, uint16_t address)
{
    for (int r = 0; r < REGION_MAX; r++)
    {
        region_t *p_region = p_mmu->regions + r;
        if (p_region->read && (p_region->start <= address) && (address <= p_region->end))
        {
            return p_region;
        }
    }

    return NULL;
}

static region_t *mmu_find_writeable_region(mmu_t *p_mmu, uint16_t address)
{
    for (int r = 0; r < REGION_MAX; r++)
    {
        region_t *p_region = p_mmu->regions + r;
        if (p_region->write && (p_region->start <= address) && (address <= p_region->end))
        {
            return p_region;
        }
    }

    return NULL;
}

static int mmu_read_boot(mmu_t *p_mmu, uint16_t address, uint8_t *data)
{
    *data = p_mmu->boot[address];
    return 0;
}

static int mmu_read_rom(mmu_t *p_mmu, uint16_t address, uint8_t *data)
{
    return cartridge_read_rom(p_mmu->cartridge, address, data);
}

static int mmu_write_rom(mmu_t *p_mmu, uint16_t address, uint8_t data)
{
    return cartridge_write_rom(p_mmu->cartridge, address, data);
}

static int mmu_read_ext_ram(mmu_t *p_mmu, uint16_t address, uint8_t *data)
{
    return cartridge_read_ram(p_mmu->cartridge, address, data);
}

static int mmu_write_ext_ram(mmu_t *p_mmu, uint16_t address, uint8_t data)
{
    return cartridge_write_ram(p_mmu->cartridge, address, data);
}

static int mmu_read_ram(mmu_t *p_mmu, uint16_t address, uint8_t *data)
{
    *data = p_mmu->ram[address - RAM_OFFSET];
    return 0;
}

static int mmu_write_ram(mmu_t *p_mmu, uint16_t address, uint8_t data)
{
    p_mmu->ram[address - RAM_OFFSET] = data;
    return 0;
}

static int mmu_read_echo_ram(mmu_t *p_mmu, uint16_t address, uint8_t *data)
{
    *data = p_mmu->ram[(address - 0x2000) - RAM_OFFSET];
    return 0;
}

static int mmu_write_echo_ram(mmu_t *p_mmu, uint16_t address, uint8_t data)
{
    p_mmu->ram[(address - 0x2000) - RAM_OFFSET] = data;
    return 0;
}

static int mmu_read_unused(mmu_t *p_mmu, uint16_t address, uint8_t *data)
{
    *data = 0xFF;
    return 0;
}

static int mmu_write_unused(mmu_t *p_mmu, uint16_t address, uint8_t data)
{
    //Ignore.
    return 0;
}

static int mmu_read_io(mmu_t *p_mmu, uint16_t address, uint8_t *data)
{
    *data = p_mmu->ram[address - RAM_OFFSET];
    return 0;
}

static int mmu_write_io(mmu_t *p_mmu, uint16_t address, uint8_t data)
{
    switch (address)
    {
    case 0xFF46:
        if (!p_mmu->dma.enabled)
        {
            p_mmu->dma.enabled = 1;
            p_mmu->dma.offset = 0;
            p_mmu->dma.source = data;
            p_mmu->dma.source <= 8;
            p_mmu->dma.destination = 0xFE00;

            //printf("MMU: Starting DMA transfer from 0x%04x to 0x%04x\n", p_mmu->dma.source, p_mmu->dma.destination);
            //exit(-1);
        }
        break;

    case 0xFF50:
        if (data)
        {
            p_mmu->regions[REGION_BOOT].read = NULL;
        }
        break;

    default:
        break;
    }

    p_mmu->ram[address - RAM_OFFSET] = data;
    return 0;
}

static void mmu_print_regions(mmu_t *p_mmu)
{
    for (int r = 0; r < REGION_MAX; r++)
    {
        region_t *p_region = p_mmu->regions + r;
        printf("MMU: Region %d [0x%04x - 0x%04x] [R%d W%d]\n", r, p_region->start, p_region->end, p_region->read != 0, p_region->write != 0);
    }
}
