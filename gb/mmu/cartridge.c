#include "cartridge.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static uint8_t *load_file(char *path, cartridge_header_t *p_header);
static void parse_header(uint8_t *rom, cartridge_header_t *p_header);

cartridge_t *cartridge_allocate(char *path)
{
    cartridge_t *p_cartridge = malloc(sizeof(cartridge_t));

    if (p_cartridge)
    {
        p_cartridge->rom = load_file(path, &p_cartridge->header);

        p_cartridge->ram = malloc(0x2000); //TODO

        if (!p_cartridge->rom || !p_cartridge->rom)
        {
            cartridge_free(p_cartridge);
            p_cartridge = NULL;
        }
    }

    return p_cartridge;
}

void cartridge_free(cartridge_t *p_cartridge)
{
    if (p_cartridge)
    {
        if (p_cartridge->ram)
        {
            free(p_cartridge->ram);
            p_cartridge->ram = NULL;
        }

        if (p_cartridge->rom)
        {
            free(p_cartridge->rom);
            p_cartridge->rom = NULL;
        }

        free(p_cartridge);
    }
}

int cartridge_read_rom(cartridge_t *p_cartridge, uint16_t address, uint8_t *data)
{
    *data = p_cartridge->rom[address];
    return 0;
}

int cartridge_write_rom(cartridge_t *p_cartridge, uint16_t address, uint8_t data)
{
    /* TODO Ignore for now. */
    return 0;
}

int cartridge_read_ram(cartridge_t *p_cartridge, uint16_t address, uint8_t *data)
{
    //TODO
    *data = p_cartridge->ram[address - 0xA000];
    return 0;
}

int cartridge_write_ram(cartridge_t *p_cartridge, uint16_t address, uint8_t data)
{
    p_cartridge->ram[address - 0xA000] = data;
}

/************************************/

static uint8_t *load_file(char *path, cartridge_header_t *p_header)
{
    FILE *file = fopen(path, "rb");

    if (!file)
    {
        printf("Failed to open file: %s\n", path);
        return NULL;
    }

    /* Retrieve file size. */
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if ((file_size < 0x150) || (file_size > 0x800000))
    {
        printf("Invalid file size: %d\n", file_size);
        fclose(file);
        return NULL;
    }

    uint8_t *rom = malloc(file_size);

    if (!rom)
    {
        printf("Failed to allocate rom, size %d\n", file_size);
        fclose(file);
        return NULL;
    }

    (void)fread(rom, file_size, 1, file);

    parse_header(rom, p_header);

    fclose(file);
    return rom;
}

static void parse_header(uint8_t *rom, cartridge_header_t *p_header)
{
    (void)memcpy(p_header->title, rom + 0x134, 16);
    p_header->title[16] = '\0';

    (void)memcpy(p_header->manufacturer, rom + 0x13F, 4);
    p_header->title[4] = '\0';

    p_header->cbg_flag = *(rom + 0x143);

    (void)memcpy(p_header->new_licensee, rom + 0x144, 2);
    p_header->new_licensee[2] = '\0';

    p_header->sgb_flag = *(rom + 0x146);

    p_header->type = *(rom + 0x147);

    p_header->rom_size = *(rom + 0x148);

    p_header->ram_size = *(rom + 0x149);

    p_header->destination = *(rom + 0x14A);

    p_header->old_licensee = *(rom + 0x14B);

    p_header->mask_rom_version = *(rom + 0x14C);

    p_header->header_checksum = *(rom + 0x14D);

    p_header->global_checksum = *((uint16_t *)(rom + 0x14E));
}