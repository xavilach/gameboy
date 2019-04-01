#include "cpu.h"
#include "mmu.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	cpu_init();

	mmu_t *p_mmu = mmu_allocate();
	cpu_t *p_cpu = cpu_allocate(p_mmu);

	if (p_cpu)
	{
		(void)cpu_run(p_cpu);
	}

	return 0;
}
