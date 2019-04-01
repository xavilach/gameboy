#ifndef CPU_H_
#define CPU_H_

#include "mmu.h"

typedef struct cpu_s cpu_t;

void cpu_init(void);

cpu_t *cpu_allocate(mmu_t *p_mmu);

int cpu_run(cpu_t *p_cpu);

#endif /*CPU_H_*/
