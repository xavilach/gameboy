#ifndef CPU_OPCODE_H_
#define CPU_OPCODE_H_

#include "cpu_def.h"

typedef int (*opcode_handler_t)(cpu_t *p_cpu);

typedef struct opcode_entry_s
{
    uint8_t mask;
    uint8_t opcode;
    opcode_handler_t handler;
} opcode_entry_t;

void opcode_init(const opcode_entry_t opcode_entries[], int entries_count, opcode_handler_t opcode_handlers[], int handlers_count);

int opcode_UNHANDLED(cpu_t *p_cpu);

#endif /*CPU_OPCODE_H_*/
