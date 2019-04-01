#include "cpu.h"

#include "cpu_opcode8.h"
#include "cpu_registers.h"
#include "cpu_base.h"

#include <stdint.h>

/* Defines */

/* Typedefs */

#define IRQ_COUNT (5)
#define IF_REG_ADDR (0xFF0F)
#define IE_REG_ADDR (0xFFFF)

/* Constants */

const uint16_t VECTOR_TABLE[IRQ_COUNT] = {
	0x0040, //Vertical blank
	0x0048, //LCD status triggers
	0x0050, //Timer overflow
	0x0058, //Serial link
	0x0060  //Joypad press
};

/* Private variables */

/* Private function declarations */

/* Inlined private function definitions */

/* Public function definitions */

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

int cpu_run(cpu_t *p_cpu)
{
	int cycles = opcode8_handler(p_cpu);

	/* Update IME flag. */
	if (p_cpu->ei_counter)
	{
		p_cpu->ei_counter -= 1;
		if (!p_cpu->ei_counter)
		{
			p_cpu->irq_master_enable = 1;
		}
	}

	if (p_cpu->di_counter)
	{
		p_cpu->di_counter -= 1;
		if (!p_cpu->di_counter)
		{
			p_cpu->irq_master_enable = 0;
		}
	}

	/* Process IRQs */
	if (p_cpu->irq_master_enable)
	{
		uint8_t irq_flag, irq_enable;
		(void)mmu_read_u8(p_cpu->p_mmu, IF_REG_ADDR, &irq_flag);
		(void)mmu_read_u8(p_cpu->p_mmu, IE_REG_ADDR, &irq_enable);

		irq_flag &= irq_enable; // Mask disabled irqs.

		if (irq_flag)
		{
			p_cpu->irq_master_enable = 0; // Disable interrupts.

			push_pc(p_cpu); //Push PC on stack.

			for (int i = 0; i < IRQ_COUNT; i++)
			{
				uint8_t mask = 1 << i;
				if (mask & irq_flag)
				{
					irq_flag &= ~mask; // Clear interrupt flag.

					jump(p_cpu, VECTOR_TABLE[i]);
				}
			}
		}
	}

	return cycles;
}

/* Private function definitions */
