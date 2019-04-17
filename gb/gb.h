#ifndef GB_H_
#define GB_H_

#include "screen.h"
#include "mmu.h"
#include "cpu.h"
#include "ppu.h"
#include "cpu_def.h"

//typedef struct gb_s gb_t;

typedef struct gb_s
{
    mmu_t *mmu;
    cpu_t *cpu;
    ppu_t *ppu;
    screen_t *screen;
} gb_t;

void gb_init(void);

gb_t *gb_allocate(void);

int gb_load_program(gb_t *p_gb, char *boot, char *rom);

screen_t *gb_get_screen(gb_t *p_gb);

int gb_execute(gb_t *p_gb, double duration_ms);

void gb_free(gb_t *p_gb);

/***********************/

void gb_dbg_read_mem(gb_t *p_gb, int address, int size, char *buffer);

#endif /*GB_H_*/