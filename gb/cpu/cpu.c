#include "cpu_def.h"
#include "cpu.h"

#include "cpu_opcode8.h"
#include "cpu_irq.h"

#include <stdlib.h>

void cpu_init(void)
{
	opcode8_init();
}

cpu_t *cpu_allocate(mmu_t *p_mmu)
{
	if (!p_mmu)
		return NULL;

	cpu_t *p_cpu = calloc(1, sizeof(cpu_t));

	if (p_cpu)
	{
		p_cpu->p_mmu = p_mmu;
	}

	return p_cpu;
}

int cpu_execute(cpu_t *p_cpu)
{
	if (!p_cpu)
		return 0;

	cpu_irq_process(p_cpu);

	if (p_cpu->halted)
		return 4;

	int cycles = opcode8_handler(p_cpu);

	return cycles;
}

void cpu_free(cpu_t *p_cpu)
{
	if (p_cpu)
	{
		free(p_cpu);
	}
}

/* Private function definitions */
