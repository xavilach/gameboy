#ifndef CPU_OPCODE16_H_
#define CPU_OPCODE16_H_

#include "cpu_base.h"

void opcode16_init(void);

int opcode16_handler(cpu_t *p_cpu);

#endif /*CPU_OPCODE16_H_*/