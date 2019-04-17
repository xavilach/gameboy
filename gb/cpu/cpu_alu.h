#ifndef _CPU_ALU_H_
#define _CPU_ALU_H_

#include "cpu_registers.h"
#include "cpu_def.h"
#include "cpu_utils.h"

static inline void alu_ADD(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);

    uint16_t result = (uint16_t)reg_a;
    result += (uint16_t)operand;

    set_msb(&p_cpu->reg_AF, result);

    set_flag_Z(p_cpu, 0 == (result & 0x00FF));
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, ((reg_a & 0x0F) + (operand & 0x0F)) > 0x0F);
    set_flag_C(p_cpu, result > 0xFF);
}

static inline void alu_ADC(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);
    uint8_t flag_c = get_flag_C(p_cpu);

    uint16_t result = (uint16_t)reg_a;
    result += (uint16_t)operand;
    result += (uint16_t)flag_c;

    set_msb(&p_cpu->reg_AF, result);

    set_flag_Z(p_cpu, 0 == (result & 0x00FF));
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, ((reg_a & 0x0F) + (operand & 0x0F) + flag_c) > 0x0F);
    set_flag_C(p_cpu, (result > 0x00FF));
}

static inline void alu_SUB(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);

    uint16_t result = (uint16_t)reg_a;
    result -= (uint16_t)operand;

    set_msb(&p_cpu->reg_AF, result);

    set_flag_Z(p_cpu, (reg_a == operand));
    set_flag_N(p_cpu, 1);
    set_flag_H(p_cpu, ((reg_a & 0x0F) < (operand & 0x0F)));
    set_flag_C(p_cpu, (reg_a < operand));
}

static inline void alu_SBC(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);
    uint8_t flag_c = get_flag_C(p_cpu);

    uint16_t result = (uint16_t)reg_a;
    result -= (uint16_t)operand;
    result -= (uint16_t)flag_c;

    set_msb(&p_cpu->reg_AF, result);

    set_flag_Z(p_cpu, 0 == (result & (uint16_t)0x00FF));
    set_flag_N(p_cpu, 1);
    set_flag_H(p_cpu, ((reg_a & 0x0F) < ((operand & 0x0F) + flag_c)));
    set_flag_C(p_cpu, result > (uint16_t)0xFF);
}

static inline void alu_AND(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);

    reg_a &= operand;

    set_msb(&p_cpu->reg_AF, reg_a);

    set_flag_Z(p_cpu, (reg_a == 0));
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 1);
    set_flag_C(p_cpu, 0);
}

static inline void alu_XOR(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);

    reg_a ^= operand;

    set_msb(&p_cpu->reg_AF, reg_a);

    set_flag_Z(p_cpu, (reg_a == 0));
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, 0);
}

static inline void alu_OR(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);

    reg_a |= operand;

    set_msb(&(p_cpu->reg_AF), reg_a);

    set_flag_Z(p_cpu, (reg_a == 0));
    set_flag_N(p_cpu, 0);
    set_flag_H(p_cpu, 0);
    set_flag_C(p_cpu, 0);
}

static inline void alu_CP(cpu_t *p_cpu, uint8_t operand)
{
    uint8_t reg_a = get_msb(p_cpu->reg_AF);

    set_flag_Z(p_cpu, (reg_a == operand));
    set_flag_N(p_cpu, 1);
    set_flag_H(p_cpu, ((reg_a & 0x0F) < (operand & 0x0F)));
    set_flag_C(p_cpu, (reg_a < operand));
}

#endif /*_CPU_ALU_H_*/