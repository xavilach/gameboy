#include "cartridge.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static uint8_t *load_file(char *path, cartridge_header_t *p_header);
static void parse_header(uint8_t *rom, cartridge_header_t *p_header);

static int cartridge_read_rom_simple(cartridge_t *p_cartridge, uint16_t address, uint8_t *data);
static int cartridge_write_rom_simple(cartridge_t *p_cartridge, uint16_t address, uint8_t data);
static int cartridge_read_ram_simple(cartridge_t *p_cartridge, uint16_t address, uint8_t *data);
static int cartridge_write_ram_simple(cartridge_t *p_cartridge, uint16_t address, uint8_t data);

static int cartridge_read_rom_mbc1(cartridge_t *p_cartridge, uint16_t address, uint8_t *data);
static int cartridge_write_rom_mbc1(cartridge_t *p_cartridge, uint16_t address, uint8_t data);
static int cartridge_read_ram_mbc1(cartridge_t *p_cartridge, uint16_t address, uint8_t *data);
static int cartridge_write_ram_mbc1(cartridge_t *p_cartridge, uint16_t address, uint8_t data);

cartridge_t *cartridge_allocate(char *path)
{
    cartridge_t *p_cartridge = malloc(sizeof(cartridge_t));

    if (p_cartridge)
    {
        p_cartridge->rom = load_file(path, &p_cartridge->header);

        p_cartridge->ram = malloc(0x2000); //TODO

        p_cartridge->rom_bank = 1;
        p_cartridge->ram_bank = 0,
        p_cartridge->ram_banking_mode = 0;
        p_cartridge->ram_enabled = 0;

        switch (p_cartridge->header.type)
        {
        case CART_TYPE_ROM:
            p_cartridge->read_rom = cartridge_read_rom_simple;
            p_cartridge->write_rom = cartridge_write_rom_simple;
            p_cartridge->read_ram = cartridge_read_ram_simple;
            p_cartridge->write_ram = cartridge_write_ram_simple;
            break;

        case CART_TYPE_MBC1:
        case CART_TYPE_MBC1_RAM:
        case CART_TYPE_MBC1_RAM_BATTERY:
            p_cartridge->read_rom = cartridge_read_rom_mbc1;
            p_cartridge->write_rom = cartridge_write_rom_mbc1;
            p_cartridge->read_ram = cartridge_read_ram_mbc1;
            p_cartridge->write_ram = cartridge_write_ram_mbc1;
            break;

        default:
            printf("Unsuported cartridge type %d", p_cartridge->header.type);
            exit(-1);
            break;
        }

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
    return p_cartridge->read_rom(p_cartridge, address, data);
}

int cartridge_write_rom(cartridge_t *p_cartridge, uint16_t address, uint8_t data)
{
    return p_cartridge->write_rom(p_cartridge, address, data);
}

int cartridge_read_ram(cartridge_t *p_cartridge, uint16_t address, uint8_t *data)
{
    return p_cartridge->read_ram(p_cartridge, address, data);
}

int cartridge_write_ram(cartridge_t *p_cartridge, uint16_t address, uint8_t data)
{
    return p_cartridge->write_ram(p_cartridge, address, data);
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
    p_header->manufacturer[4] = '\0';

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

static int cartridge_read_rom_simple(cartridge_t *p_cartridge, uint16_t address, uint8_t *data)
{
    *data = p_cartridge->rom[address];
    return 0;
}

static int cartridge_write_rom_simple(cartridge_t *p_cartridge, uint16_t address, uint8_t data)
{
    /* TODO Ignore for now. */
    return 0;
}

static int cartridge_read_ram_simple(cartridge_t *p_cartridge, uint16_t address, uint8_t *data)
{
    //TODO
    *data = p_cartridge->ram[address - 0xA000];
    return 0;
}

static int cartridge_write_ram_simple(cartridge_t *p_cartridge, uint16_t address, uint8_t data)
{
    p_cartridge->ram[address - 0xA000] = data;
}

static int cartridge_read_rom_mbc1(cartridge_t *p_cartridge, uint16_t address, uint8_t *data)
{
    if (address < 0x4000)
    {
        /* Bank 0. */
        *data = p_cartridge->rom[address];
    }
    else
    {
        uint32_t addr = address - 0x4000;
        addr += (uint32_t)p_cartridge->rom_bank * (uint32_t)0x4000;

        /* Bank n. */
        *data = p_cartridge->rom[addr];
    }
    return 0;
}

static int cartridge_write_rom_mbc1(cartridge_t *p_cartridge, uint16_t address, uint8_t data)
{
    if (address < 0x2000)
    {
        // ram enable
        data &= 0x0F;

        p_cartridge->ram_enabled = (0x0A == data);

        //printf("CART: External RAM enabled %d.\n", p_cartridge->ram_enabled);
    }
    else if (address < 0x4000)
    {
        // ROM bank number
        data &= 0x1F;
        if (0 == data)
        {
            data = 0x01;
        }
        p_cartridge->rom_bank &= 0xE0;
        p_cartridge->rom_bank |= data;

        //printf("CART: ROM bank %d selected.\n", p_cartridge->rom_bank);
    }
    else if (address < 0x6000)
    {
        //RAM bank number or ROM bank number
        data &= 0x03;
        if (p_cartridge->ram_banking_mode)
        {
            p_cartridge->ram_bank = data;
            //printf("CART: RaM bank %d selected.\n", p_cartridge->ram_bank);
        }
        else
        {
            p_cartridge->rom_bank &= 0x1F;
            p_cartridge->rom_bank |= (data << 5);
            //printf("CART: ROM bank %d selected.\n", p_cartridge->rom_bank);
        }
    }
    else
    {
        //ROM/RAM mode select
        p_cartridge->ram_banking_mode = (0 != (data & 0x01));

        //printf("CART: External RAM banking mode %d.\n", p_cartridge->ram_banking_mode);
    }
    return 0;
}

static int cartridge_read_ram_mbc1(cartridge_t *p_cartridge, uint16_t address, uint8_t *data)
{
    if (p_cartridge->ram_enabled)
    {
        uint16_t addr = address - 0xA000;
        addr += (uint16_t)p_cartridge->ram_bank * (uint16_t)0x2000;

        *data = p_cartridge->ram[addr];
    }
    else
    {
        *data = 0xFF;
    }
    return 0;
}

static int cartridge_write_ram_mbc1(cartridge_t *p_cartridge, uint16_t address, uint8_t data)
{
    if (p_cartridge->ram_enabled)
    {
        uint16_t addr = address - 0xA000;
        addr += (uint16_t)p_cartridge->ram_bank * (uint16_t)0x2000;

        p_cartridge->ram[addr] = data;
    }
    return 0;
}
