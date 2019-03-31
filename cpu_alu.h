#ifndef _CPU_ALU_H_
#define _CPU_ALU_H_

#include "cpu_registers.h"

static inline void alu_ADD(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);
    uint16_t result = reg_a + operand;

    set_flag_N(p_cpu, 0);
    set_flag_Z(p_cpu, (result & 0x00FF) == 0);
    set_flag_H(p_cpu, ((reg_a & 0x0F) + (operand & 0x0F)) >> 4);
    set_flag_C(p_cpu, result >> 8);

    set_msb(&p_cpu->reg_AF, result);
}

static inline void alu_ADC(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);
    uint8_t flag_c = get_flag_C(p_cpu);
    uint16_t result = reg_a + operand + flag_c;

    set_flag_N(p_cpu, 0);
    set_flag_Z(p_cpu, (result & 0x00FF) == 0);
    set_flag_H(p_cpu, ((reg_a & 0x0F) + (operand & 0x0F) + flag_c) >> 4);
    set_flag_C(p_cpu, result >> 8);

    set_msb(&p_cpu->reg_AF, result);
}

static inline void alu_SUB(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);
    uint16_t result = reg_a - operand;

    set_flag_N(p_cpu, 1);
    set_flag_Z(p_cpu, (result & 0x00FF) == 0);
    set_flag_H(p_cpu, ((reg_a & 0x0F) - (operand & 0x0F)) >> 4);
    set_flag_C(p_cpu, result >> 8);

    set_msb(&p_cpu->reg_AF, result);
}

static inline void alu_SBC(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);
    uint8_t flag_c = get_flag_C(p_cpu);
    uint16_t result = reg_a - (operand + flag_c);

    set_flag_N(p_cpu, 1);
    set_flag_Z(p_cpu, (result & 0x00FF) == 0);
    set_flag_H(p_cpu, ((reg_a & 0x0F) - ((operand & 0x0F) + flag_c)) >> 4);
    set_flag_C(p_cpu, result >> 8);

    set_msb(&p_cpu->reg_AF, result);
}

static inline void alu_AND(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);

    reg_a &= operand;

    set_flag_N(p_cpu, 0);
    set_flag_Z(p_cpu, reg_a == 0);
    set_flag_H(p_cpu, 1);
    set_flag_C(p_cpu, 0);

    set_msb(&p_cpu->reg_AF, reg_a);
}

static inline void alu_XOR(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);

    reg_a ^= operand;

    set_flag_N(p_cpu, 0);
    set_flag_Z(p_cpu, reg_a == 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, 0);

    set_msb(&p_cpu->reg_AF, reg_a);
}

static inline void alu_OR(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);

    reg_a |= operand;

    set_flag_N(p_cpu, 0);
    set_flag_Z(p_cpu, reg_a == 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, 0);

    set_msb(&p_cpu->reg_AF, reg_a);
}

static inline void alu_CP(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);
    uint16_t result = reg_a - operand;

    set_flag_N(p_cpu, 1);
    set_flag_Z(p_cpu, (result & 0x00FF) == 0);
    set_flag_H(p_cpu, ((reg_a & 0x0F) - (operand & 0x0F)) >> 4);
    set_flag_C(p_cpu, result >> 8);
}

#endif /*_CPU_ALU_H_*/