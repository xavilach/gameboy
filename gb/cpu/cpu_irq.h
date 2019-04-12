#ifndef CPU_IRQ_H_
#define CPU_IRQ_H_

#include "mmu.h"
#include "cpu_def.h"
#include "cpu_registers.h"

#define IRQ_COUNT (5)

#define IF_REG_ADDR (0xFF0F)
#define IE_REG_ADDR (0xFFFF)

const uint16_t VECTOR_TABLE[IRQ_COUNT] = {
    0x0040, //Vertical blank
    0x0048, //LCD status triggers
    0x0050, //Timer overflow
    0x0058, //Serial link
    0x0060  //Joypad press
};

static inline void cpu_irq_process(cpu_t *p_cpu)
{
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

        irq_flag &= (irq_enable & 0x1F); // Mask disabled irqs.

        if (irq_flag)
        {
            for (int i = 0; i < IRQ_COUNT; i++)
            {
                uint8_t mask = 1 << i;
                if (mask & irq_flag)
                {
                    push_pc(p_cpu); //Push PC on stack.

                    p_cpu->irq_master_enable = 0; // Disable interrupts.
                    irq_flag &= ~mask;            // Clear interrupt flag.

                    jump(p_cpu, VECTOR_TABLE[i]);
                    break;
                }
            }
        }
    }
}

#endif /*CPU_IRQ_H_*/