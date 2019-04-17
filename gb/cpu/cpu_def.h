#ifndef CPU_DEF_H_
#define CPU_DEF_H_

#include "mmu.h"
#include <stdint.h>

typedef struct cpu_s
{
    uint16_t reg_AF;
    uint16_t reg_BC;
    uint16_t reg_DE;
    uint16_t reg_HL;

    uint16_t sp;
    uint16_t pc;

    mmu_t *p_mmu;

    uint8_t irq_master_enable;
    int di_counter;
    int ei_counter;

    int halted;

    int div_counter;
    int tim_counter;
    int tim_clock;
} cpu_t;

#endif /*CPU_DEF_H_*/