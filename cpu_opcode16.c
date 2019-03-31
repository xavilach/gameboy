#include "cpu_opcode16.h"

#include "cpu_opcode.h"
#include "cpu_registers.h"

/* Defines */

#define OPCODE_COUNT (256)
#define ENTRY_COUNT (sizeof(opcode16_entries) / sizeof(opcode_entry_t))

/* Typedefs */

/* Constants */

/* Private variables */

opcode_handler_t opcode16_handlers[OPCODE_COUNT] = {NULL};

/* Private function declarations */

static int opcode16_RLC_D(cpu_t *p_cpu);
static int opcode16_RLC_HL(cpu_t *p_cpu);
static int opcode16_RRC_D(cpu_t *p_cpu);
static int opcode16_RRC_HL(cpu_t *p_cpu);
static int opcode16_RL_D(cpu_t *p_cpu);
static int opcode16_RL_HL(cpu_t *p_cpu);
static int opcode16_RR_D(cpu_t *p_cpu);
static int opcode16_RR_HL(cpu_t *p_cpu);
static int opcode16_SLA_D(cpu_t *p_cpu);
static int opcode16_SLA_HL(cpu_t *p_cpu);
static int opcode16_SRA_D(cpu_t *p_cpu);
static int opcode16_SRA_HL(cpu_t *p_cpu);
static int opcode16_SWAP_D(cpu_t *p_cpu);
static int opcode16_SWAP_HL(cpu_t *p_cpu);
static int opcode16_SRL_D(cpu_t *p_cpu);
static int opcode16_SRL_HL(cpu_t *p_cpu);
static int opcode16_BIT_N_D(cpu_t *p_cpu);
static int opcode16_BIT_N_HL(cpu_t *p_cpu);
static int opcode16_RES_N_D(cpu_t *p_cpu);
static int opcode16_RES_N_HL(cpu_t *p_cpu);
static int opcode16_SET_N_D(cpu_t *p_cpu);
static int opcode16_SET_N_HL(cpu_t *p_cpu);

static const opcode_entry_t opcode16_entries[] = {
    {0xFF, 0x06, opcode16_RLC_HL},   // RLC (HL)
    {0xFF, 0x0E, opcode16_RRC_HL},   // RRC (HL)
    {0xFF, 0x16, opcode16_RL_HL},    // RL (HL)
    {0xFF, 0x1E, opcode16_RR_HL},    // RR (HL)
    {0xFF, 0x26, opcode16_SLA_HL},   // SLA (HL)
    {0xFF, 0x2E, opcode16_SRA_HL},   // SRA (HL)
    {0xFF, 0x36, opcode16_SWAP_HL},  // SWAP (HL)
    {0xFF, 0x3E, opcode16_SRL_HL},   // SRL (HL)
    {0xC7, 0x46, opcode16_BIT_N_HL}, // BIT N, (HL)
    {0xC7, 0x86, opcode16_RES_N_HL}, // RES N, (HL)
    {0xC7, 0xC6, opcode16_SET_N_HL}, // SET N, (HL)
    {0xF8, 0x00, opcode16_RLC_D},    // RLC D
    {0xF8, 0x08, opcode16_RRC_D},    // RRC D
    {0xF8, 0x10, opcode16_RL_D},     // RL D
    {0xF8, 0x18, opcode16_RR_D},     // RR D
    {0xF8, 0x20, opcode16_SLA_D},    // SLA D
    {0xF8, 0x28, opcode16_SRA_D},    // SRA D
    {0xF8, 0x30, opcode16_SWAP_D},   // SWAP D
    {0xF8, 0x38, opcode16_SRL_D},    // SRL D
    {0xC0, 0x40, opcode16_BIT_N_D},  // BIT N, D
    {0xC0, 0x80, opcode16_RES_N_D},  // RES N, D
    {0xC0, 0xC0, opcode16_SET_N_D},  // SET N, D
    {0x00, 0x00, opcode_UNHANDLED}};

/* Inlined private function definitions */

/* Public function definitions */

void opcode16_init(void)
{
    opcode_init(opcode16_entries, ENTRY_COUNT, opcode16_handlers, OPCODE_COUNT);
}

int opcode16_handler(cpu_t *p_cpu)
{
    // Check whether opcode16 module was initialized.
    assert(opcode16_handlers[0] != NULL);

    return opcode16_handlers[*(p_cpu->pc)](p_cpu);
}

/* Private function definitions */

static int opcode16_RLC_D(cpu_t *p_cpu)
{
    uint8_t d = *(p_cpu->pc + 1) & 0x07;
    uint8_t dv = get_reg3(p_cpu, d);

    uint8_t flag_c = (dv >> 7) & 0x01;
    dv = (dv << 1) | flag_c;

    set_reg3(p_cpu, d, dv);

    set_flag_Z(p_cpu, dv == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, flag_c);

    p_cpu->pc += 2;
    return 8;
}

static int opcode16_RLC_HL(cpu_t *p_cpu)
{
    uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

    uint8_t flag_c = (*p_value >> 7) & 0x01;
    *p_value = (*p_value << 1) | flag_c;

    set_flag_Z(p_cpu, *p_value == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, flag_c);

    p_cpu->pc += 2;
    return 16;
}

static int opcode16_RRC_D(cpu_t *p_cpu)
{
    uint8_t d = *(p_cpu->pc + 1) & 0x07;
    uint8_t dv = get_reg3(p_cpu, d);

    uint8_t flag_c = dv & 0x01;
    dv = (dv >> 1) | (flag_c << 7);

    set_reg3(p_cpu, d, dv);

    set_flag_Z(p_cpu, dv == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, flag_c);

    p_cpu->pc += 2;
    return 8;
}

static int opcode16_RRC_HL(cpu_t *p_cpu)
{
    uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

    uint8_t flag_c = *p_value & 0x01;
    *p_value = (*p_value >> 1) | (flag_c << 7);

    set_flag_Z(p_cpu, *p_value == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, flag_c);

    p_cpu->pc += 2;
    return 16;
}

static int opcode16_RL_D(cpu_t *p_cpu)
{
    uint8_t d = *(p_cpu->pc + 1) & 0x07;
    uint8_t dv = get_reg3(p_cpu, d);

    uint8_t flag_c = (dv >> 7) & 0x01;
    dv = (dv << 1) | get_flag_C(p_cpu);

    set_reg3(p_cpu, d, dv);

    set_flag_Z(p_cpu, dv == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, flag_c);

    p_cpu->pc += 2;
    return 8;
}

static int opcode16_RL_HL(cpu_t *p_cpu)
{
    uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

    uint8_t flag_c = (*p_value >> 7) & 0x01;
    *p_value = (*p_value << 1) | get_flag_C(p_cpu);

    set_flag_Z(p_cpu, *p_value == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, flag_c);

    p_cpu->pc += 2;
    return 16;
}

static int opcode16_RR_D(cpu_t *p_cpu)
{
    uint8_t d = *(p_cpu->pc + 1) & 0x07;
    uint8_t dv = get_reg3(p_cpu, d);

    uint8_t flag_c = dv & 0x01;
    dv = (dv >> 1) | (get_flag_C(p_cpu) << 7);

    set_reg3(p_cpu, d, dv);

    set_flag_Z(p_cpu, dv == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, flag_c);

    p_cpu->pc += 2;
    return 8;
}

static int opcode16_RR_HL(cpu_t *p_cpu)
{
    uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

    uint8_t flag_c = *p_value & 0x01;
    *p_value = (*p_value >> 1) | (get_flag_C(p_cpu) << 7);

    set_flag_Z(p_cpu, *p_value == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, flag_c);

    p_cpu->pc += 2;
    return 16;
}

//Shift left into Carry. LSB of n set to 0.
static int opcode16_SLA_D(cpu_t *p_cpu)
{
    uint8_t d = *(p_cpu->pc + 1) & 0x07;
    uint8_t dv = get_reg3(p_cpu, d);

    uint8_t flag_c = (dv >> 7) & 0x01;
    dv = (dv << 1) & 0xFE;

    set_reg3(p_cpu, d, dv);

    set_flag_Z(p_cpu, dv == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, flag_c);

    p_cpu->pc += 2;
    return 8;
}

//Shift left into Carry. LSB of n set to 0.
static int opcode16_SLA_HL(cpu_t *p_cpu)
{
    uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

    uint8_t flag_c = (*p_value >> 7) & 0x01;
    *p_value = (*p_value << 1) & 0xFE;

    set_flag_Z(p_cpu, *p_value == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, flag_c);

    p_cpu->pc += 2;
    return 16;
}

//Shift right into Carry. MSB doesn't change.
static int opcode16_SRA_D(cpu_t *p_cpu)
{
    uint8_t d = *(p_cpu->pc + 1) & 0x07;
    uint8_t dv = get_reg3(p_cpu, d);

    uint8_t flag_c = dv & 0x01;
    dv = (dv & 0x80) | ((dv >> 1) & 0x7F);

    set_reg3(p_cpu, d, dv);

    set_flag_Z(p_cpu, dv == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, flag_c);

    p_cpu->pc += 2;
    return 8;
}

//Shift right into Carry. MSB doesn't change.
static int opcode16_SRA_HL(cpu_t *p_cpu)
{
    uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

    uint8_t flag_c = *p_value & 0x01;
    *p_value = (*p_value & 0x80) | ((*p_value >> 1) & 0x7F);

    set_flag_Z(p_cpu, *p_value == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, flag_c);

    p_cpu->pc += 2;
    return 16;
}

//Swap upper & lower nibles
static int opcode16_SWAP_D(cpu_t *p_cpu)
{
    uint8_t d = *(p_cpu->pc + 1) & 0x07;
    uint8_t dv = get_reg3(p_cpu, d);

    dv = ((dv << 4) & 0xF0) | ((dv >> 4) & 0x0F);

    set_reg3(p_cpu, d, dv);

    set_flag_Z(p_cpu, dv == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, 0);

    p_cpu->pc += 2;
    return 8;
}

//Swap upper & lower nibles
static int opcode16_SWAP_HL(cpu_t *p_cpu)
{
    uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

    *p_value = ((*p_value << 4) & 0xF0) | ((*p_value >> 4) & 0x0F);

    set_flag_Z(p_cpu, *p_value == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, 0);

    p_cpu->pc += 2;
    return 16;
}

//Shift right into Carry. MSB set to 0.
static int opcode16_SRL_D(cpu_t *p_cpu)
{
    uint8_t d = *(p_cpu->pc + 1) & 0x07;
    uint8_t dv = get_reg3(p_cpu, d);

    uint8_t flag_c = dv & 0x01;
    dv = (dv >> 1) & 0x7F;

    set_reg3(p_cpu, d, dv);

    set_flag_Z(p_cpu, dv == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, flag_c);

    p_cpu->pc += 2;
    return 8;

    p_cpu->pc += 2;
    return 8;
}

//Shift right into Carry. MSB set to 0.
static int opcode16_SRL_HL(cpu_t *p_cpu)
{
    uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

    uint8_t flag_c = *p_value & 0x01;
    *p_value = (*p_value >> 1) & 0x7F;

    set_flag_Z(p_cpu, *p_value == 0);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, flag_c);

    p_cpu->pc += 2;
    return 16;
}

//Test bit n in register d.
static int opcode16_BIT_N_D(cpu_t *p_cpu)
{
    uint8_t n = (*(p_cpu->pc + 1) >> 3) & 0x07;
    uint8_t mask = 1 << n;

    uint8_t d = *(p_cpu->pc + 1) & 0x07;
    uint8_t dv = get_reg3(p_cpu, d);

    set_flag_Z(p_cpu, (dv ^ mask) >> n);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 1);

    p_cpu->pc += 2;
    return 8;
}

//Test bit n in register d.
static int opcode16_BIT_N_HL(cpu_t *p_cpu)
{
    uint8_t n = (*(p_cpu->pc + 1) >> 3) & 0x07;
    uint8_t mask = 1 << n;

    const uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

    set_flag_Z(p_cpu, (*p_value ^ mask) >> n);
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 1);

    p_cpu->pc += 2;
    return 16;
}

//Reset bit n in register d.
static int opcode16_RES_N_D(cpu_t *p_cpu)
{
    uint8_t n = (*(p_cpu->pc + 1) >> 3) & 0x07;
    uint8_t mask = ~(1 << n);

    uint8_t d = *(p_cpu->pc + 1) & 0x07;
    uint8_t dv = get_reg3(p_cpu, d);

    dv &= mask;

    set_reg3(p_cpu, d, dv);

    p_cpu->pc += 2;
    return 8;
}

//Reset bit n in register d.
static int opcode16_RES_N_HL(cpu_t *p_cpu)
{
    uint8_t n = (*(p_cpu->pc + 1) >> 3) & 0x07;
    uint8_t mask = ~(1 << n);

    uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

    *p_value &= mask;

    p_cpu->pc += 2;
    return 16;
}

//Set bit n in register d.
static int opcode16_SET_N_D(cpu_t *p_cpu)
{
    uint8_t n = (*(p_cpu->pc + 1) >> 3) & 0x07;
    uint8_t mask = 1 << n;

    uint8_t d = *(p_cpu->pc + 1) & 0x07;
    uint8_t dv = get_reg3(p_cpu, d);

    dv |= mask;

    set_reg3(p_cpu, d, dv);

    p_cpu->pc += 2;
    return 8;
}

//Set bit n in register d.
static int opcode16_SET_N_HL(cpu_t *p_cpu)
{
    uint8_t n = (*(p_cpu->pc + 1) >> 3) & 0x07;
    uint8_t mask = 1 << n;

    uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

    *p_value |= mask;

    p_cpu->pc += 2;
    return 16;
}
