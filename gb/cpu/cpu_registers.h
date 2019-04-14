#ifndef CPU_REGISTERS_H_
#define CPU_REGISTERS_H_

#include "cpu_def.h"
#include "cpu_utils.h"

#include <assert.h>

/* Typedefs */

typedef enum reg3_e
{
    REG3_B = 0b000,
    REG3_C = 0b001,
    REG3_D = 0b010,
    REG3_E = 0b011,
    REG3_H = 0b100,
    REG3_L = 0b101,
    REG3_A = 0b111
} reg3_t;

typedef enum reg2_e
{
    REG2_BC = 0b00,
    REG2_DE = 0b01,
    REG2_HL = 0b10,
    REG2_SP = 0b11
} reg2_t;

typedef enum reg1_e
{
    REG1_BC = 0b00,
    REG1_DE = 0b01,
    REG1_HL = 0b10,
    REG1_PC = 0b11
} reg1_t;

typedef enum mnemonic_e
{
    MNEMONIC_NZ = 0b00,
    MNEMONIC_Z = 0b01,
    MNEMONIC_NC = 0b10,
    MNEMONIC_C = 0b11,
} mnemonic_t;

/* Inlined function definitions */

static inline void set_flag_Z(cpu_t *p_cpu, uint8_t flag)
{
    flag &= (uint8_t)0x01;
    flag <<= 7;

    p_cpu->reg_AF &= (uint16_t)0xFF7F;
    p_cpu->reg_AF |= (uint16_t)flag;
}

static inline uint8_t get_flag_Z(cpu_t *p_cpu)
{
    return (uint8_t)((p_cpu->reg_AF >> 7) & 0x01);
}

static inline void set_flag_N(cpu_t *p_cpu, uint8_t flag)
{
    flag &= (uint8_t)0x01;
    flag <<= 6;

    p_cpu->reg_AF &= (uint16_t)0xFFBF;
    p_cpu->reg_AF |= (uint16_t)flag;
}

static inline uint8_t get_flag_N(cpu_t *p_cpu)
{
    return (uint8_t)((p_cpu->reg_AF >> 6) & 0x01);
}

static inline void set_flag_H(cpu_t *p_cpu, uint8_t flag)
{
    flag &= (uint8_t)0x01;
    flag <<= 5;

    p_cpu->reg_AF &= (uint16_t)0xFFDF;
    p_cpu->reg_AF |= (uint16_t)flag;
}

static inline uint8_t get_flag_H(cpu_t *p_cpu)
{
    return (uint8_t)((p_cpu->reg_AF >> 5) & 0x01);
}

static inline void set_flag_C(cpu_t *p_cpu, uint8_t flag)
{
    flag &= (uint8_t)0x01;
    flag <<= 4;

    p_cpu->reg_AF &= (uint16_t)0xFFEF;
    p_cpu->reg_AF |= (uint16_t)flag;
}

static inline uint8_t get_flag_C(cpu_t *p_cpu)
{
    return (uint8_t)((p_cpu->reg_AF >> 4) & 0x01);
}

static inline void push_u8(cpu_t *p_cpu, uint8_t u8)
{
    p_cpu->sp -= 1;
    (void)mmu_write_u8(p_cpu->p_mmu, p_cpu->sp, u8);
}

static inline uint8_t pop_u8(cpu_t *p_cpu)
{
    uint8_t u8;
    (void)mmu_read_u8(p_cpu->p_mmu, p_cpu->sp, &u8);
    p_cpu->sp += 1;
    return u8;
}

static inline void push_u16(cpu_t *p_cpu, uint16_t u16)
{
    push_u8(p_cpu, u16);
    push_u8(p_cpu, u16 >> 8);
}

static inline uint16_t pop_u16(cpu_t *p_cpu)
{
    uint16_t u16 = pop_u8(p_cpu);
    u16 <<= 8;
    u16 |= pop_u8(p_cpu);
    return u16;
}

static inline void push_pc(cpu_t *p_cpu)
{
    push_u16(p_cpu, p_cpu->pc);
}

static inline void pop_pc(cpu_t *p_cpu)
{
    p_cpu->pc = pop_u16(p_cpu);
}

static inline void jump(cpu_t *p_cpu, uint16_t addr)
{
    p_cpu->pc = addr;
}

/* Register accessors */

static const char str_B[] = "B";
static const char str_C[] = "C";
static const char str_D[] = "D";
static const char str_E[] = "E";
static const char str_H[] = "H";
static const char str_L[] = "L";
static const char str_A[] = "A";
static const char str_BC[] = "BC";
static const char str_DE[] = "DE";
static const char str_HL[] = "HL";
static const char str_SP[] = "SP";
static const char str_PC[] = "PC";

static const char str_Z[] = "Z";
static const char str_NZ[] = "NZ";
static const char str_NC[] = "CC";

static inline const char *str_reg3(reg3_t d)
{
    switch (d)
    {
    case REG3_B:
        return str_B;
    case REG3_C:
        return str_C;
    case REG3_D:
        return str_D;
    case REG3_E:
        return str_E;
    case REG3_H:
        return str_H;
    case REG3_L:
        return str_L;
    case REG3_A:
        return str_A;
    default:
        assert(0);
        return 0;
    }
}

static inline void set_reg3(cpu_t *p_cpu, reg3_t d, uint8_t value)
{
    switch (d)
    {
    case REG3_B:
        set_msb(&(p_cpu->reg_BC), value);
        break;
    case REG3_C:
        set_lsb(&(p_cpu->reg_BC), value);
        break;
    case REG3_D:
        set_msb(&(p_cpu->reg_DE), value);
        break;
    case REG3_E:
        set_lsb(&(p_cpu->reg_DE), value);
        break;
    case REG3_H:
        set_msb(&(p_cpu->reg_HL), value);
        break;
    case REG3_L:
        set_lsb(&(p_cpu->reg_HL), value);
        break;
    case REG3_A:
        set_msb(&(p_cpu->reg_AF), value);
        break;
    default:
        assert(0);
        break;
    }
}

static inline uint8_t get_reg3(cpu_t *p_cpu, reg3_t d)
{
    switch (d)
    {
    case REG3_B:
        return get_msb(p_cpu->reg_BC);
    case REG3_C:
        return get_lsb(p_cpu->reg_BC);
    case REG3_D:
        return get_msb(p_cpu->reg_DE);
    case REG3_E:
        return get_lsb(p_cpu->reg_DE);
    case REG3_H:
        return get_msb(p_cpu->reg_HL);
    case REG3_L:
        return get_lsb(p_cpu->reg_HL);
    case REG3_A:
        return get_msb(p_cpu->reg_AF);
    default:
        assert(0);
        return 0;
    }
}

static inline const char *str_reg2(reg2_t r)
{
    switch (r)
    {
    case REG2_BC:
        return str_BC;
    case REG2_DE:
        return str_DE;
    case REG2_HL:
        return str_HL;
    case REG2_SP:
        return str_SP;
    default:
        assert(0);
        break;
    }
}

static inline void set_reg2(cpu_t *p_cpu, reg2_t r, uint16_t value)
{
    switch (r)
    {
    case REG2_BC:
        p_cpu->reg_BC = value;
        break;
    case REG2_DE:
        p_cpu->reg_DE = value;
        break;
    case REG2_HL:
        p_cpu->reg_HL = value;
        break;
    case REG2_SP:
        p_cpu->sp = value;
        break;
    default:
        assert(0);
        break;
    }
}

static inline uint16_t get_reg2(cpu_t *p_cpu, reg2_t r)
{
    switch (r)
    {
    case REG2_BC:
        return p_cpu->reg_BC;
    case REG2_DE:
        return p_cpu->reg_DE;
    case REG2_HL:
        return p_cpu->reg_HL;
    case REG2_SP:
        return p_cpu->sp;
    default:
        assert(0);
        return 0;
    }
}

static inline const char *str_reg1(reg3_t r)
{
    switch (r)
    {
    case REG1_BC:
        return str_BC;
    case REG1_DE:
        return str_DE;
    case REG1_HL:
        return str_HL;
    case REG1_PC:
        return str_PC;
    default:
        assert(0);
        return 0;
    }
}

static inline void set_reg1(cpu_t *p_cpu, reg1_t r, uint16_t value)
{
    switch (r)
    {
    case REG1_BC:
        p_cpu->reg_BC = value;
        break;
    case REG1_DE:
        p_cpu->reg_DE = value;
        break;
    case REG1_HL:
        p_cpu->reg_HL = value;
        break;
    case REG1_PC:
        p_cpu->pc = value;
        break;
    default:
        assert(0);
        break;
    }
}

static inline uint16_t get_reg1(cpu_t *p_cpu, reg1_t r)
{
    switch (r)
    {
    case REG1_BC:
        return p_cpu->reg_BC;
    case REG1_DE:
        return p_cpu->reg_DE;
    case REG1_HL:
        return p_cpu->reg_HL;
    case REG1_PC:
        return p_cpu->pc;
    default:
        assert(0);
        return 0;
    }
}

static inline const char *str_mnemonic(reg3_t m)
{
    switch (m)
    {
    case MNEMONIC_NZ:
        return str_NZ;
    case MNEMONIC_Z:
        return str_Z;
    case MNEMONIC_NC:
        return str_NC;
    case MNEMONIC_C:
        return str_C;
    default:
        assert(0);
        return 0;
    }
}

static inline uint8_t get_mnemonic(cpu_t *p_cpu, mnemonic_t m)
{
    switch (m)
    {
    case MNEMONIC_NZ:
        return !get_flag_Z(p_cpu);
    case MNEMONIC_Z:
        return get_flag_Z(p_cpu);
    case MNEMONIC_NC:
        return !get_flag_C(p_cpu);
    case MNEMONIC_C:
        return get_flag_C(p_cpu);
    default:
        assert(0);
        return 0;
    }
}

#endif /*CPU_REGISTERS_H_*/