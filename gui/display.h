#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "../gb/gb.h"

typedef enum dbg_registers_e
{
    DBG_REGISTERS_NONE = 0,
    DBG_REGISTERS_INTC,
    DBG_REGISTERS_APU,
    DBG_REGISTERS_JOYPAD,
    DBG_REGISTERS_SERIAL,
    DBG_REGISTERS_TIMER,
    DBG_REGISTERS_PPU,
    DBG_REGISTERS_MAX
} dbg_registers_t;

typedef struct display_s display_t;

int display_init(void);

display_t *display_create(void);

int display_render_gb(display_t *p_display, gb_t *p_gb);

int display_dbg_registers(display_t *p_display, gb_t *p_gb, dbg_registers_t regs);

int display_refresh(display_t *p_display);

void display_text(display_t *p_display, int x, int y, char *str);

void display_free(display_t *p_display);

void display_quit(void);

#endif /*DISPLAY_H_*/