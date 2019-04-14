#ifndef CARTRIDGE_H_
#define CARTRIDGE_H_

#include <stdint.h>

/* ROM HEADER
0x0100 - 0x0103 Entry point.
0x0104 - 0x0133 Nintendo logo.
0x0134 - 0x0143 Title.
0x013F - 0x0142 Manufacturer code.
0x0143          CBG flag.
0x0144 - 0x0145 New licensee code.
0x0146          SGB flag.
0x0147          Cartridge type.
0x0148          ROM Size.
0x0149          RAM Size.
0x014A          Destination code.
0x014B          Old Licensee code.
0x014C          Mask ROM Version Number.
0x014D          Header Checksum.
0x014E - 0x014F Global Checksum.
*/

typedef enum cartridge_type_e
{
    CART_TYPE_ROM = 0x00,
    CART_TYPE_MBC1 = 0x01,
    CART_TYPE_MBC1_RAM = 0x02,
    CART_TYPE_MBC1_RAM_BATTERY = 0x03,
    CART_TYPE_MBC2 = 0x05,
    CART_TYPE_MBC2_BATTERY = 0x06,
    CART_TYPE_ROM_RAM = 0x08,
    CART_TYPE_ROM_RAM_BATTERY = 0x09,
    CART_TYPE_MMM01 = 0x0B,
    CART_TYPE_MMM01_RAM = 0x0C,
    CART_TYPE_MMM01_RAM_BATTERY = 0x0D,
    CART_TYPE_MBC3_TIMER_BATTERY = 0x0F,
    CART_TYPE_MBC3_TIMER_RAM_BATTERY = 0x10,
    CART_TYPE_MBC3 = 0x11,
    CART_TYPE_MBC3_RAM = 0x12,
    CART_TYPE_MBC3_RAM_BATTERY = 0x13,
    CART_TYPE_MBC5 = 0x19,
    CART_TYPE_MBC5_RAM = 0x1A,
    CART_TYPE_MBC5_RAM_BATTERY = 0x1B,
    CART_TYPE_MBC5_RUMBLE = 0x1C,
    CART_TYPE_MBC5_RUMBLE_RAM = 0x1D,
    CART_TYPE_MBC5_RUMBLE_RAM_BATTERY = 0x1E,
    CART_TYPE_MBC6 = 0x20,
    CART_TYPE_MBC7_SENSOR_RUMBLE_RAM_BATTERY = 0x22,
    CART_TYPE_POCKET_CAMERA = 0xFC,
    CART_TYPE_BANDAI_TAMA5 = 0xFD,
    CART_TYPE_HuC3 = 0xFE,
    CART_TYPE_HuC1_RAM_BATTERY = 0xFF
} cartridge_type_t;

typedef struct cartridge_header_s
{
    char title[17];
    char manufacturer[5];
    uint8_t cbg_flag;
    char new_licensee[3];
    uint8_t sgb_flag;
    cartridge_type_t type;
    uint32_t rom_size;
    uint16_t ram_size;
    uint8_t destination;
    uint8_t old_licensee;
    uint8_t mask_rom_version;
    uint8_t header_checksum;
    uint16_t global_checksum;

} cartridge_header_t;

typedef struct cartridge_s
{
    cartridge_header_t header;

    uint8_t *rom;
    uint8_t *ram;
} cartridge_t;

cartridge_t *cartridge_allocate(char *path);
void cartridge_free(cartridge_t *p_cartridge);

int cartridge_read_rom(cartridge_t *p_cartridge, uint16_t address, uint8_t *data);
int cartridge_write_rom(cartridge_t *p_cartridge, uint16_t address, uint8_t data);
int cartridge_read_ram(cartridge_t *p_cartridge, uint16_t address, uint8_t *data);
int cartridge_write_ram(cartridge_t *p_cartridge, uint16_t address, uint8_t data);

#endif /*CARTRIDGE_H_*/