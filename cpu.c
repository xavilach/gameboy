#include "cpu.h"

#include "cpu_opcode8.h"
#include "cpu_base.h"

#include <stdint.h>

/* Defines */

/* Typedefs */

/* Constants */

/* Private variables */

/* Private function declarations */

/* Inlined private function definitions */

/* Public function definitions */

int cpu_run(cpu_t *p_cpu)
{
	int cycles = opcode8_handler(p_cpu);

	if (p_cpu->ei_counter)
	{
		p_cpu->ei_counter -= 1;
		if (!p_cpu->ei_counter)
		{
			p_cpu->irq_mask = 0xFF;
		}
	}

	if (p_cpu->di_counter)
	{
		p_cpu->di_counter -= 1;
		if (!p_cpu->di_counter)
		{
			p_cpu->irq_mask = 0x00;
		}
	}

	return cycles;
}

/* Private function definitions */
