#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>

typedef struct lcd_s
{
    uint8_t pixels[160 * 144];
} lcd_t;

#endif /*LCD_H_*/
