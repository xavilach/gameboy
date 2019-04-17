#ifndef TIMER_H_
#define TIMER_H_

#include "cpu_def.h"

/* Timer
0xFF04 DIV  Divider Register.
0xFF05 TIMA Timer Counter.
0xFF06 TMA  Timer Modulo.
0xFF07 TAC  Timer Control.
-> TAC2   Timer Start.
-> TAC1-0 Input clock.
--> 01 : 262,144 Hz.
--> 10 : 65536 Hz.
--> 11 : 16384 Hz.
--> 00 : 4096 Hz.
*/

#define TIMER_REG_DIV (0xFF04)
#define TIMER_REG_TIMA (0xFF05)
#define TIMER_REG_TMA (0xFF06)
#define TIMER_REG_TAC (0xFF07)

static inline void timer_run(cpu_t *p_cpu)
{
    p_cpu->div_counter += 1;

    if (p_cpu->div_counter >= 256)
    {
        p_cpu->div_counter = 0;

        uint8_t div;
        (void)mmu_read_u8(p_cpu->p_mmu, TIMER_REG_DIV, &div);

        div += 1;

        (void)mmu_write_u8(p_cpu->p_mmu, TIMER_REG_DIV, div);
    }

    uint8_t tac;
    (void)mmu_read_u8(p_cpu->p_mmu, TIMER_REG_TAC, &tac);

    if ((tac >> 2) & 0x01)
    {
        //timer enabled.

        switch (tac & 0x03)
        {
        case 0:
            p_cpu->tim_clock = 1024;
            break;
        case 1:
            p_cpu->tim_clock = 16;
            break;
        case 2:
            p_cpu->tim_clock = 64;
            break;
        case 3:
            p_cpu->tim_clock = 256;
            break;
        }

        p_cpu->tim_counter += 1;

        if (p_cpu->tim_counter >= p_cpu->tim_clock)
        {
            p_cpu->tim_counter = 0;

            uint8_t tima;
            (void)mmu_read_u8(p_cpu->p_mmu, TIMER_REG_TIMA, &tima);

            if (0xFF == tima)
            {
                uint8_t tma;
                (void)mmu_read_u8(p_cpu->p_mmu, TIMER_REG_TMA, &tma);

                tima = tma;

                (void)mmu_write_u8(p_cpu->p_mmu, TIMER_REG_TIMA, tima);

                uint8_t irq_flags;
                (void)mmu_read_u8(p_cpu->p_mmu, 0xFF0F, &irq_flags);

                irq_flags |= (1 << 2);

                (void)mmu_write_u8(p_cpu->p_mmu, 0xFF0F, irq_flags);
            }
            else
            {
                tima += 1;

                (void)mmu_write_u8(p_cpu->p_mmu, TIMER_REG_TIMA, tima);
            }
        }
    }
}

#endif /*TIMER_H_*/