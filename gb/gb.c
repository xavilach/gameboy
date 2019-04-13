#include "gb.h"

#include "mmu/mmu.h"
#include "cpu/cpu.h"
#include "ppu/ppu.h"
#include "screen.h"

#include <stdlib.h>

typedef struct gb_s
{
    mmu_t *mmu;
    cpu_t *cpu;
    ppu_t *ppu;
    screen_t *screen;
} gb_t;

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

void gb_init(void)
{
    cpu_init();
}

gb_t *gb_allocate(void)
{
    gb_t *p_gb = calloc(1, sizeof(gb_t));

    if (p_gb)
    {
        p_gb->mmu = mmu_allocate();
        p_gb->cpu = cpu_allocate(p_gb->mmu);
        p_gb->screen = screen_allocate();
        p_gb->ppu = ppu_allocate(p_gb->mmu, p_gb->screen);

        if (!p_gb->mmu || !p_gb->cpu || !p_gb->ppu || !p_gb->screen)
        {
            gb_free(p_gb);
            p_gb = NULL;
        }
    }

    return p_gb;
}

int gb_load_program(gb_t *p_gb, char *boot, char *rom)
{
    if (!p_gb)
    {
        return -1;
    }

    if (mmu_load(p_gb->mmu, rom, boot) < 0)
    {
        return -1;
    }

    return 0;
}

screen_t *gb_get_screen(gb_t *p_gb)
{
    if (!p_gb)
    {
        return NULL;
    }

    return p_gb->screen;
}

int gb_execute(gb_t *p_gb, double duration_ms)
{
    if (!p_gb || (duration_ms <= 0))
    {
        return -1;
    }

    int cycles = (int)(duration_ms * ((4.0 * 1024.0 * 1024.0) / 1000.0));
    int cpu_cycles = 0;
    int ppu_cycles = 0;
    int mmu_cycles = 0;

    while (cycles > 0)
    {
        if (!cpu_cycles)
        {
            cpu_cycles = cpu_execute(p_gb->cpu);
        }

        if (!ppu_cycles)
        {
            ppu_cycles = ppu_execute(p_gb->ppu);
        }

        if (!mmu_cycles)
        {
            mmu_cycles = mmu_execute(p_gb->mmu);
        }

        int wait_cycles = min(cpu_cycles, ppu_cycles);

        if (!wait_cycles)
        {
            //TODO error
        }

        cycles -= wait_cycles;
        cpu_cycles -= wait_cycles;
        ppu_cycles -= wait_cycles;
    }

    return 0;
}

void gb_free(gb_t *p_gb)
{
    if (p_gb)
    {
        ppu_free(p_gb->ppu);
        p_gb->ppu = NULL;

        cpu_free(p_gb->cpu);
        p_gb->mmu = NULL;

        mmu_free(p_gb->mmu);
        p_gb->mmu = NULL;

        screen_free(p_gb->screen);
        p_gb->screen = NULL;

        free(p_gb);
    }
}

/* DEBUG */

void gb_dbg_read_mem(gb_t *p_gb, int address, int size, char *buffer)
{
    for (int i = 0; i < size; i++)
    {
        (void)mmu_read_u8(p_gb->mmu, address + i, buffer + i);
    }
}
