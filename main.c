#include "cpu.h"
#include "ppu.h"
#include "mmu.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	cpu_init();

	lcd_t lcd;

	mmu_t *p_mmu = mmu_allocate();
	cpu_t *p_cpu = cpu_allocate(p_mmu);
	ppu_t *p_ppu = ppu_allocate(p_mmu, &lcd);

	if (p_cpu && p_ppu)
	{
		(void)cpu_run(p_cpu);
		ppu_cycle(p_ppu);
	}

	return 0;
}
