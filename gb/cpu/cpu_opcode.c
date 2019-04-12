#include "cpu_opcode.h"

#include <assert.h>

/* Defines */

/* Typedefs */

/* Constants */

/* Private variables */

/* Private function declarations */

static opcode_handler_t find_handler(const opcode_entry_t opcode_entries[], int entries_count, uint8_t opcode);

/* Inlined private function definitions */

/* Public function definitions */

void opcode_init(const opcode_entry_t opcode_entries[], int entries_count, opcode_handler_t opcode_handlers[], int handlers_count)
{
    for (int opcode = 0; opcode < handlers_count; opcode++)
    {
        opcode_handler_t handler = find_handler(opcode_entries, entries_count, (uint8_t)opcode);
        if (handler)
        {
            opcode_handlers[opcode] = handler;
        }
        else
        {
            opcode_handlers[opcode] = opcode_UNHANDLED;
        }
    }
}

int opcode_UNHANDLED(cpu_t *p_cpu)
{
    assert(0);
    return 0;
}

/* Private function definitions */

static opcode_handler_t find_handler(const opcode_entry_t opcode_entries[], int entries_count, uint8_t opcode)
{
    for (int entry = 0; entry < entries_count; entry++)
    {
        uint8_t entry_mask = opcode_entries[entry].opcode & opcode_entries[entry].mask;
        uint8_t opcode_mask = opcode & opcode_entries[entry].mask;

        if (entry_mask == opcode_mask)
        {
            return opcode_entries[entry].handler;
        }
    }

    return NULL;
}
