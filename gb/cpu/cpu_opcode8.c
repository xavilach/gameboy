#include "cpu_opcode8.h"

#include "cpu_opcode16.h"
#include "cpu_opcode.h"
#include "cpu_alu.h"
#include "cpu_registers.h"

#if 0
#define DBG_ENABLED
static int debug_enabled = 0;

#define DEBUG_PRINT(fmt, ...) \
	if (debug_enabled)        \
	printf(fmt, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) (void)0
#endif /*NDEBUG*/

#include <stdio.h>

/* Defines */

#define OPCODE_COUNT (256)
#define ENTRY_COUNT (sizeof(opcode8_entries) / sizeof(opcode_entry_t))

/* Typedefs */

/* Constants */

/* Private variables */

opcode_handler_t opcode8_handlers[OPCODE_COUNT] = {NULL};

/* Private function declarations */

static int opcode8_UNHANDLED(cpu_t *p_cpu);
static int opcode8_NOP(cpu_t *p_cpu);
static int opcode8_LD_N_SP(cpu_t *p_cpu);
static int opcode8_LD_R_N(cpu_t *p_cpu);
static int opcode8_ADD_HL_R(cpu_t *p_cpu);
static int opcode8_LD_R_A(cpu_t *p_cpu);
static int opcode8_LD_A_R(cpu_t *p_cpu);
static int opcode8_INC_R(cpu_t *p_cpu);
static int opcode8_DEC_R(cpu_t *p_cpu);
static int opcode8_INC_D(cpu_t *p_cpu);
static int opcode8_INC_HL(cpu_t *p_cpu);
static int opcode8_DEC_D(cpu_t *p_cpu);
static int opcode8_DEC_HL(cpu_t *p_cpu);
static int opcode8_LD_D_N(cpu_t *p_cpu);
static int opcode8_LD_HL_N(cpu_t *p_cpu);
static int opcode8_RLCA(cpu_t *p_cpu);
static int opcode8_RRCA(cpu_t *p_cpu);
static int opcode8_RLA(cpu_t *p_cpu);
static int opcode8_RRA(cpu_t *p_cpu);
static int opcode8_STOP(cpu_t *p_cpu);
static int opcode8_JR_N(cpu_t *p_cpu);
static int opcode8_JR_F_N(cpu_t *p_cpu);
static int opcode8_LDI_HL_A(cpu_t *p_cpu);
static int opcode8_LDI_A_HL(cpu_t *p_cpu);
static int opcode8_LDD_HL_A(cpu_t *p_cpu);
static int opcode8_LDD_A_HL(cpu_t *p_cpu);
static int opcode8_DAA(cpu_t *p_cpu);
static int opcode8_CPL(cpu_t *p_cpu);
static int opcode8_SCF(cpu_t *p_cpu);
static int opcode8_CCF(cpu_t *p_cpu);
static int opcode8_LD_D_D(cpu_t *p_cpu);
static int opcode8_LD_D_HL(cpu_t *p_cpu);
static int opcode8_LD_HL_D(cpu_t *p_cpu);
static int opcode8_HALT(cpu_t *p_cpu);
static int opcode8_ADD_A_D(cpu_t *p_cpu);
static int opcode8_ADC_A_D(cpu_t *p_cpu);
static int opcode8_SUB_A_D(cpu_t *p_cpu);
static int opcode8_SBC_A_D(cpu_t *p_cpu);
static int opcode8_AND_A_D(cpu_t *p_cpu);
static int opcode8_XOR_A_D(cpu_t *p_cpu);
static int opcode8_OR_A_D(cpu_t *p_cpu);
static int opcode8_CP_A_D(cpu_t *p_cpu);
static int opcode8_ADD_A_HL(cpu_t *p_cpu);
static int opcode8_ADC_A_HL(cpu_t *p_cpu);
static int opcode8_SUB_A_HL(cpu_t *p_cpu);
static int opcode8_SBC_A_HL(cpu_t *p_cpu);
static int opcode8_AND_A_HL(cpu_t *p_cpu);
static int opcode8_XOR_A_HL(cpu_t *p_cpu);
static int opcode8_OR_A_HL(cpu_t *p_cpu);
static int opcode8_CP_A_HL(cpu_t *p_cpu);
static int opcode8_ADD_A_N(cpu_t *p_cpu);
static int opcode8_ADC_A_N(cpu_t *p_cpu);
static int opcode8_SUB_A_N(cpu_t *p_cpu);
static int opcode8_SBC_A_N(cpu_t *p_cpu);
static int opcode8_AND_A_N(cpu_t *p_cpu);
static int opcode8_XOR_A_N(cpu_t *p_cpu);
static int opcode8_OR_A_N(cpu_t *p_cpu);
static int opcode8_CP_A_N(cpu_t *p_cpu);
static int opcode8_POP_R(cpu_t *p_cpu);
static int opcode8_PUSH_R(cpu_t *p_cpu);
static int opcode8_RST_N(cpu_t *p_cpu);
static int opcode8_RET(cpu_t *p_cpu);
static int opcode8_RET_I(cpu_t *p_cpu);
static int opcode8_RET_F(cpu_t *p_cpu);
static int opcode8_JP_N(cpu_t *p_cpu);
static int opcode8_JP_F_N(cpu_t *p_cpu);
static int opcode8_CALL_N(cpu_t *p_cpu);
static int opcode8_CALL_F_N(cpu_t *p_cpu);
static int opcode8_ADD_SP_N(cpu_t *p_cpu);
static int opcode8_LD_HL_SP_N(cpu_t *p_cpu);
static int opcode8_LD_FF00_N_A(cpu_t *p_cpu);
static int opcode8_LD_A_FF00_N(cpu_t *p_cpu);
static int opcode8_LD_C_A(cpu_t *p_cpu);
static int opcode8_LD_A_C(cpu_t *p_cpu);
static int opcode8_LD_N_A(cpu_t *p_cpu);
static int opcode8_LD_A_N(cpu_t *p_cpu);
static int opcode8_JP_HL(cpu_t *p_cpu);
static int opcode8_LD_SP_HL(cpu_t *p_cpu);
static int opcode8_DI(cpu_t *p_cpu);
static int opcode8_EI(cpu_t *p_cpu);

static const opcode_entry_t opcode8_entries[] = {
	{0xFF, 0x00, opcode8_NOP},		   // NOP
	{0xFF, 0x07, opcode8_RLCA},		   // RLCA
	{0xFF, 0x08, opcode8_LD_N_SP},	 // LD (N), SP
	{0xFF, 0x0F, opcode8_RRCA},		   // RRCA
	{0xFF, 0x10, opcode8_STOP},		   // STOP
	{0xFF, 0x17, opcode8_RLA},		   // RLA
	{0xFF, 0x18, opcode8_JR_N},		   // JR N
	{0xFF, 0x1F, opcode8_RRA},		   // RRA
	{0xFF, 0x22, opcode8_LDI_HL_A},	// LDI (HL), A
	{0xFF, 0x27, opcode8_DAA},		   // DAA
	{0xFF, 0x2A, opcode8_LDI_A_HL},	// LDI A, (HL)
	{0xFF, 0x2F, opcode8_CPL},		   // CPL
	{0xFF, 0x32, opcode8_LDD_HL_A},	// LDD (HL), A
	{0xFF, 0x34, opcode8_INC_HL},	  // INC (HL)
	{0xFF, 0x35, opcode8_DEC_HL},	  // DEC (HL)
	{0xFF, 0x36, opcode8_LD_HL_N},	 // LD (HL), N
	{0xFF, 0x37, opcode8_SCF},		   // SCF
	{0xFF, 0x3A, opcode8_LDD_A_HL},	// LDD A, (HL)
	{0xFF, 0x3F, opcode8_CCF},		   // CCF
	{0xFF, 0x76, opcode8_HALT},		   // HALT
	{0xFF, 0x86, opcode8_ADD_A_HL},	// ADD A, (HL)
	{0xFF, 0x8E, opcode8_ADC_A_HL},	// ADC A, (HL)
	{0xFF, 0x96, opcode8_SUB_A_HL},	// SUB A, (HL)
	{0xFF, 0x9E, opcode8_SBC_A_HL},	// SBC A, (HL)
	{0xFF, 0xA6, opcode8_AND_A_HL},	// AND A, (HL)
	{0xFF, 0xAE, opcode8_XOR_A_HL},	// XOR A, (HL)
	{0xFF, 0xB6, opcode8_OR_A_HL},	 // OR A, (HL)
	{0xFF, 0xBE, opcode8_CP_A_HL},	 // CP A, (HL)
	{0xFF, 0xC3, opcode8_JP_N},		   // JP N
	{0xFF, 0xC6, opcode8_ADD_A_N},	 // ADD A, N
	{0xFF, 0xC9, opcode8_RET},		   // RET
	{0xFF, 0xCB, opcode16_handler},	// 16bits opcodes
	{0xFF, 0xCD, opcode8_CALL_N},	  // CALL N
	{0xFF, 0xCE, opcode8_ADC_A_N},	 // ADC A, N
	{0xFF, 0xD6, opcode8_SUB_A_N},	 // SUB A, N
	{0xFF, 0xD9, opcode8_RET_I},	   // RETI
	{0xFF, 0xDE, opcode8_SBC_A_N},	 // SBC A, N
	{0xFF, 0xE0, opcode8_LD_FF00_N_A}, // LD (FF00+N), A
	{0xFF, 0xE2, opcode8_LD_C_A},	  // LD (C), A
	{0xFF, 0xE6, opcode8_AND_A_N},	 // AND A, N
	{0xFF, 0xE8, opcode8_ADD_SP_N},	// ADD SP, N
	{0xFF, 0xE9, opcode8_JP_HL},	   // JP HL
	{0xFF, 0xEA, opcode8_LD_N_A},	  // LD (N), A
	{0xFF, 0xEE, opcode8_XOR_A_N},	 // XOR A, N
	{0xFF, 0xF0, opcode8_LD_A_FF00_N}, // LD A, (FF00+N)
	{0xFF, 0xF2, opcode8_LD_A_C},	  // LD A, (C)
	{0xFF, 0xF3, opcode8_DI},		   // DI
	{0xFF, 0xF6, opcode8_OR_A_N},	  // OR A, N
	{0xFF, 0xF8, opcode8_LD_HL_SP_N},  // LD HL, SP+N
	{0xFF, 0xF9, opcode8_LD_SP_HL},	// LD SP, HL
	{0xFF, 0xFA, opcode8_LD_A_N},	  // LD A, (N)
	{0xFF, 0xFB, opcode8_EI},		   // EI
	{0xFF, 0xFE, opcode8_CP_A_N},	  // CP A, N
	{0xF8, 0x70, opcode8_LD_HL_D},	 // LD (HL), D
	{0xF8, 0x80, opcode8_ADD_A_D},	 // ADD A, D
	{0xF8, 0x88, opcode8_ADC_A_D},	 // ADC A, D
	{0xF8, 0x90, opcode8_SUB_A_D},	 // SUB A, D
	{0xF8, 0x98, opcode8_SBC_A_D},	 // SBC A, D
	{0xF8, 0xA0, opcode8_AND_A_D},	 // AND A, D
	{0xF8, 0xA8, opcode8_XOR_A_D},	 // XOR A, D
	{0xF8, 0xB0, opcode8_OR_A_D},	  // OR A, D
	{0xF8, 0xB8, opcode8_CP_A_D},	  // CP A, D
	{0xEF, 0x02, opcode8_LD_R_A},	  // LD (R), A
	{0xEF, 0x0A, opcode8_LD_A_R},	  // LD A, (R)
	{0xE7, 0xC0, opcode8_RET_F},	   // RET F
	{0xE7, 0xC2, opcode8_JP_F_N},	  // JP F, N
	{0xE7, 0xC4, opcode8_CALL_F_N},	// CALL F, N
	{0xE7, 0x20, opcode8_JR_F_N},	  // JR F, N
	{0xCF, 0x01, opcode8_LD_R_N},	  // LD R, N
	{0xCF, 0x03, opcode8_INC_R},	   // INC R
	{0xCF, 0x09, opcode8_ADD_HL_R},	// ADD HL, R
	{0xCF, 0x0B, opcode8_DEC_R},	   // DEC R
	{0xCF, 0xC1, opcode8_POP_R},	   // POP R
	{0xCF, 0xC5, opcode8_PUSH_R},	  // PUSH R
	{0xC7, 0x04, opcode8_INC_D},	   // INC D
	{0xC7, 0x05, opcode8_DEC_D},	   // DEC D
	{0xC7, 0x06, opcode8_LD_D_N},	  // LD D, N
	{0xC7, 0x46, opcode8_LD_D_HL},	 // LD D, (HL)
	{0xC7, 0xC7, opcode8_RST_N},	   // RST N
	{0xC0, 0x40, opcode8_LD_D_D},	  // LD D, D
	{0x00, 0x00, opcode_UNHANDLED}};

/* Inlined private function definitions */

/* Public function definitions */

void opcode8_init(void)
{
	opcode_init(opcode8_entries, ENTRY_COUNT, opcode8_handlers, OPCODE_COUNT);
	opcode16_init();
}

int opcode8_handler(cpu_t *p_cpu)
{
	// Check whether opcode16 module was initialized.
	assert(opcode8_handlers[0] != NULL);

#ifdef DBG_ENABLED
	if (p_cpu->pc > 0xFF)
		debug_enabled = 1;
#endif

	uint8_t opcode;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &opcode);

	return opcode8_handlers[opcode](p_cpu);
}

/* Private function definitions */

//No operation.
static int opcode8_NOP(cpu_t *p_cpu)
{
	DEBUG_PRINT("%04x:NOP\n", p_cpu->pc);
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_LD_N_SP(cpu_t *p_cpu)
{
	uint16_t n;
	(void)mmu_read_u16(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	(void)mmu_write_u16(p_cpu->p_mmu, n, p_cpu->sp);

	DEBUG_PRINT("%04x:LD (%04x) SP [%04x]\n", p_cpu->pc, n, p_cpu->sp);
	p_cpu->pc += 3;
	return 20;
}

static int opcode8_LD_R_N(cpu_t *p_cpu)
{
	uint8_t r;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &r);
	r = (r >> 4) & 0x03;

	uint16_t n;
	(void)mmu_read_u16(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	set_reg2(p_cpu, r, n);

	DEBUG_PRINT("%04x:LD %s %04x\n", p_cpu->pc, str_reg2(r), n);
	p_cpu->pc += 3;
	return 12;
}

static int opcode8_ADD_HL_R(cpu_t *p_cpu)
{
	uint8_t r;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &r);
	r >>= 4;
	r &= 0x03;

	uint16_t rv = get_reg2(p_cpu, r);
	uint32_t sum = p_cpu->reg_HL + rv;

	set_flag_N(p_cpu, 0);
	set_flag_H(p_cpu, (((p_cpu->reg_HL & 0x0FFF) + (rv & 0x0FFF)) & 0x1000) > 0x0FFF);
	set_flag_C(p_cpu, (sum > 0xFFFF));

	p_cpu->reg_HL = sum;

	DEBUG_PRINT("%04x:ADD HL %s [%04x]\n", p_cpu->pc, str_reg2(r), rv);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LD_R_A(cpu_t *p_cpu)
{
	uint8_t r;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &r);
	r >>= 4;
	r &= 0x03;

	uint16_t rv = get_reg2(p_cpu, r);
	uint8_t a = get_msb(p_cpu->reg_AF);

	(void)mmu_write_u8(p_cpu->p_mmu, rv, a);

	DEBUG_PRINT("%04x:LD (%s) [%04x] A [%02x]\n", p_cpu->pc, str_reg2(r), rv, a);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LD_A_R(cpu_t *p_cpu)
{
	uint8_t r;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &r);
	r >>= 4;
	r &= 0x03;

	uint16_t rv = get_reg2(p_cpu, r);

	uint8_t a;
	(void)mmu_read_u8(p_cpu->p_mmu, rv, &a);

	set_msb(&(p_cpu->reg_AF), a);

	DEBUG_PRINT("%04x:LD A (%s) [%04x:%02x]\n", p_cpu->pc, str_reg2(r), rv, a);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_INC_R(cpu_t *p_cpu)
{
	uint8_t r;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &r);
	r >>= 4;
	r &= 0x03;

	set_reg2(p_cpu, r, get_reg2(p_cpu, r) + 1);

	DEBUG_PRINT("%04x:INC %s\n", p_cpu->pc, str_reg2(r));
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_DEC_R(cpu_t *p_cpu)
{
	uint8_t r;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &r);
	r >>= 4;
	r &= 0x03;

	set_reg2(p_cpu, r, get_reg2(p_cpu, r) - 1);

	DEBUG_PRINT("%04x:DEC %s\n", p_cpu->pc, str_reg2(r));
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_INC_D(cpu_t *p_cpu)
{
	uint8_t d;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &d);
	d >>= 3;
	d &= 0x07;

	uint8_t dv = get_reg3(p_cpu, d);

	dv += 1;

	set_reg3(p_cpu, d, dv);

	set_flag_Z(p_cpu, (0 == dv));
	set_flag_N(p_cpu, 0);
	set_flag_H(p_cpu, (0 == (dv & 0x0F)));

	DEBUG_PRINT("%04x:INC %s\n", p_cpu->pc, str_reg3(d));
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_INC_HL(cpu_t *p_cpu)
{
	uint8_t value;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->reg_HL, &value);

	value += 1;

	(void)mmu_write_u8(p_cpu->p_mmu, p_cpu->reg_HL, value);

	set_flag_Z(p_cpu, (0 == value));
	set_flag_N(p_cpu, 0);
	set_flag_H(p_cpu, (0 == (value & 0x0F)));

	DEBUG_PRINT("%04x:INC (HL)\n", p_cpu->pc);
	p_cpu->pc += 1;
	return 12;
}

static int opcode8_DEC_D(cpu_t *p_cpu)
{
	uint8_t d;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &d);
	d >>= 3;
	d &= 0x07;

	uint8_t dv = get_reg3(p_cpu, d);

	dv -= 1;

	set_reg3(p_cpu, d, dv);

	set_flag_Z(p_cpu, (0 == dv));
	set_flag_N(p_cpu, 1);
	set_flag_H(p_cpu, (0x0F == (dv & 0x0F)));

	DEBUG_PRINT("%04x:DEC %s\n", p_cpu->pc, str_reg3(d));
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_DEC_HL(cpu_t *p_cpu)
{
	uint8_t value;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->reg_HL, &value);

	value -= 1;

	(void)mmu_write_u8(p_cpu->p_mmu, p_cpu->reg_HL, value);

	set_flag_Z(p_cpu, (0 == value));
	set_flag_N(p_cpu, 1);
	set_flag_H(p_cpu, (0x0F == (value & 0x0F)));

	DEBUG_PRINT("%04x:DEC (HL)\n", p_cpu->pc);
	p_cpu->pc += 1;
	return 12;
}

static int opcode8_LD_D_N(cpu_t *p_cpu)
{
	uint8_t d;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &d);
	d >>= 3;
	d &= 0x07;

	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	set_reg3(p_cpu, d, n);

	DEBUG_PRINT("%04x:LD %s %02x\n", p_cpu->pc, str_reg3(d), n);
	p_cpu->pc += 2;
	return 8;
}

static int opcode8_LD_HL_N(cpu_t *p_cpu)
{
	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	(void)mmu_write_u8(p_cpu->p_mmu, p_cpu->reg_HL, n);

	DEBUG_PRINT("%04x:LD (HL) %02x\n", p_cpu->pc, n);
	p_cpu->pc += 2;
	return 12;
}

static int opcode8_RLCA(cpu_t *p_cpu)
{
	uint8_t reg_a = get_msb(p_cpu->reg_AF);
	uint8_t flag_c = (reg_a >> 7) & 0x01;
	reg_a = (reg_a << 1) | flag_c;

	set_msb(&(p_cpu->reg_AF), reg_a);

	set_flag_Z(p_cpu, 0);
	set_flag_N(p_cpu, 0);
	set_flag_H(p_cpu, 0);
	set_flag_C(p_cpu, flag_c);

	DEBUG_PRINT("%04x:RLCA\n", p_cpu->pc);
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_RRCA(cpu_t *p_cpu)
{
	uint8_t reg_a = get_msb(p_cpu->reg_AF);
	uint8_t flag_c = reg_a & 0x01;
	reg_a = (reg_a >> 1) | (flag_c << 7);

	set_msb(&(p_cpu->reg_AF), reg_a);

	set_flag_Z(p_cpu, 0);
	set_flag_N(p_cpu, 0);
	set_flag_H(p_cpu, 0);
	set_flag_C(p_cpu, flag_c);

	DEBUG_PRINT("%04x:RCCA\n", p_cpu->pc);
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_RLA(cpu_t *p_cpu)
{
	uint8_t reg_a = get_msb(p_cpu->reg_AF);
	uint8_t flag_c = (reg_a >> 7) & 0x01;
	reg_a = (reg_a << 1) | get_flag_C(p_cpu);

	set_msb(&(p_cpu->reg_AF), reg_a);

	set_flag_Z(p_cpu, 0);
	set_flag_N(p_cpu, 0);
	set_flag_H(p_cpu, 0);
	set_flag_C(p_cpu, flag_c);

	DEBUG_PRINT("%04x:RLA\n", p_cpu->pc);
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_RRA(cpu_t *p_cpu)
{
	uint8_t reg_a = get_msb(p_cpu->reg_AF);
	uint8_t flag_c = reg_a & 0x01;
	reg_a = (reg_a >> 1) | (get_flag_C(p_cpu) << 7);

	set_msb(&(p_cpu->reg_AF), reg_a);

	set_flag_Z(p_cpu, 0);
	set_flag_N(p_cpu, 0);
	set_flag_H(p_cpu, 0);
	set_flag_C(p_cpu, flag_c);

	DEBUG_PRINT("%04x:RRA\n", p_cpu->pc);
	p_cpu->pc += 1;
	return 4;
}

//Halt CPU & LCD display until button pressed.
static int opcode8_STOP(cpu_t *p_cpu)
{
	DEBUG_PRINT("%04x:STOP\n", p_cpu->pc);
	p_cpu->pc += 1;
	return 4;
}

//Add n to current address and jump to it.
static int opcode8_JR_N(cpu_t *p_cpu)
{
	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	uint16_t newPC = p_cpu->pc + 2 + (int8_t)n;

	DEBUG_PRINT("%04x:JR %04x\n", p_cpu->pc, newPC);
	p_cpu->pc = newPC;
	return 8;
}

//If following condition is true then add n to current address and jump to it.
static int opcode8_JR_F_N(cpu_t *p_cpu)
{
	uint8_t m;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &m);
	m >>= 3;
	m &= 0x03;

	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	uint16_t newPC = p_cpu->pc + 2 + (int8_t)n;

	DEBUG_PRINT("%04x:JR %s[%01x] %04x\n", p_cpu->pc, str_mnemonic(m), get_lsb(p_cpu->reg_AF), newPC);
	if (get_mnemonic(p_cpu, m))
	{
		p_cpu->pc = newPC;
	}
	else
	{
		p_cpu->pc += 2;
	}
	return 8;
}

static int opcode8_LDI_HL_A(cpu_t *p_cpu)
{
	uint8_t a = get_msb(p_cpu->reg_AF);

	(void)mmu_write_u8(p_cpu->p_mmu, p_cpu->reg_HL, a);

	p_cpu->reg_HL += 1;

	DEBUG_PRINT("%04x:LDI (HL) A [%02x]\n", p_cpu->pc, a);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LDI_A_HL(cpu_t *p_cpu)
{
	uint8_t a;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->reg_HL, &a);

	p_cpu->reg_HL += 1;

	set_msb(&(p_cpu->reg_AF), a);

	DEBUG_PRINT("%04x:LDI A (HL) [%02x]\n", p_cpu->pc, a);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LDD_HL_A(cpu_t *p_cpu)
{
	uint8_t a = get_msb(p_cpu->reg_AF);

	(void)mmu_write_u8(p_cpu->p_mmu, p_cpu->reg_HL, a);

	p_cpu->reg_HL -= 1;

	DEBUG_PRINT("%04x:LDD (HL) A [%02x]\n", p_cpu->pc, a);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LDD_A_HL(cpu_t *p_cpu)
{
	uint8_t a;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->reg_HL, &a);

	p_cpu->reg_HL -= 1;

	set_msb(&(p_cpu->reg_AF), a);

	DEBUG_PRINT("%04x:LDD A (HL) [%02x]\n", p_cpu->pc, a);
	p_cpu->pc += 1;
	return 8;
}

//Decimal adjust register A. This instruction adjusts register A so that the
//correct representation of Binary Coded Decimal (BCD) is obtained.
static int opcode8_DAA(cpu_t *p_cpu)
{
	uint8_t reg_a = get_msb(p_cpu->reg_AF);

	if (!get_flag_N(p_cpu))
	{
		/* After addition. */
		if (get_flag_C(p_cpu) || (reg_a > 0x99))
		{
			reg_a += 0x60;
			set_flag_C(p_cpu, 1);
		}
		if (get_flag_H(p_cpu) || ((reg_a & 0x0F) > 0x09))
		{
			reg_a += 0x6;
		}
	}
	else
	{
		/* After substraction. */
		if (get_flag_C(p_cpu))
		{
			reg_a -= 0x60;
		}
		if (get_flag_H(p_cpu))
		{
			reg_a -= 0x6;
		}
	}

	set_msb(&p_cpu->reg_AF, reg_a);

	/* Update z and h flags. */
	set_flag_Z(p_cpu, (reg_a == 0));
	set_flag_H(p_cpu, 0);

	DEBUG_PRINT("%04x:DAA\n", p_cpu->pc, get_msb(p_cpu->reg_AF), reg_a);
	p_cpu->pc += 1;

	return 4;
}

//Complement A register. (Flip all bits.)
static int opcode8_CPL(cpu_t *p_cpu)
{
	uint8_t reg_a = ~get_msb(p_cpu->reg_AF);
	set_msb(&(p_cpu->reg_AF), reg_a);

	set_flag_N(p_cpu, 1);
	set_flag_H(p_cpu, 1);

	DEBUG_PRINT("%04x:CPL\n", p_cpu->pc);
	p_cpu->pc += 1;
	return 4;
}

//Set Carry flag
static int opcode8_SCF(cpu_t *p_cpu)
{
	set_flag_N(p_cpu, 0);
	set_flag_H(p_cpu, 0);
	set_flag_C(p_cpu, 1);

	DEBUG_PRINT("%04x:SCF\n", p_cpu->pc);
	p_cpu->pc += 1;
	return 4;
}

//Complement carry flag.
static int opcode8_CCF(cpu_t *p_cpu)
{
	set_flag_N(p_cpu, 0);
	set_flag_H(p_cpu, 0);
	set_flag_C(p_cpu, !get_flag_C(p_cpu));

	DEBUG_PRINT("%04x:CCF\n", p_cpu->pc);
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_LD_D_D(cpu_t *p_cpu)
{
	uint8_t opcode;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &opcode);

	uint8_t d0 = (opcode >> 3) & 0x07;
	uint8_t d1 = (opcode >> 0) & 0x07;

	uint8_t dv = get_reg3(p_cpu, d1);

	set_reg3(p_cpu, d0, dv);

	DEBUG_PRINT("%04x:LD %s %s [%02x]\n", p_cpu->pc, str_reg3(d0), str_reg3(d1), dv);
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_LD_D_HL(cpu_t *p_cpu)
{
	uint8_t d;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &d);
	d >>= 3;
	d &= 0x07;

	uint8_t value;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->reg_HL, &value);

	set_reg3(p_cpu, d, value);

	DEBUG_PRINT("%04x:LD %s (HL) [%02x]\n", p_cpu->pc, str_reg3(d), value);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LD_HL_D(cpu_t *p_cpu)
{
	uint8_t d;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &d);
	d &= 0x07;

	uint8_t dv = get_reg3(p_cpu, d);

	(void)mmu_write_u8(p_cpu->p_mmu, p_cpu->reg_HL, dv);

	DEBUG_PRINT("%04x:LD (HL) %s [%02x]\n", p_cpu->pc, str_reg3(d), dv);
	p_cpu->pc += 1;
	return 8;
}

//Power down CPU until an interrupt occurs.
static int opcode8_HALT(cpu_t *p_cpu)
{
	p_cpu->halted = 1;

	DEBUG_PRINT("%04x:HALT\n", p_cpu->pc);
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_ADD_A_D(cpu_t *p_cpu)
{
	uint8_t d;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &d);
	d &= 0x07;

	uint8_t dv = get_reg3(p_cpu, d);

	alu_ADD(p_cpu, dv);

	DEBUG_PRINT("%04x:ADD A %s [%02x]\n", p_cpu->pc, str_reg3(d), dv);
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_ADC_A_D(cpu_t *p_cpu)
{
	uint8_t d;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &d);
	d &= 0x07;

	uint8_t dv = get_reg3(p_cpu, d);

	alu_ADC(p_cpu, dv);

	DEBUG_PRINT("%04x:ADC A %s [%02x]\n", p_cpu->pc, str_reg3(d), dv);
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_SUB_A_D(cpu_t *p_cpu)
{
	uint8_t d;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &d);
	d &= 0x07;

	uint8_t dv = get_reg3(p_cpu, d);

	alu_SUB(p_cpu, dv);

	DEBUG_PRINT("%04x:SUB A %s [%02x]\n", p_cpu->pc, str_reg3(d), dv);
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_SBC_A_D(cpu_t *p_cpu)
{
	uint8_t d;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &d);
	d &= 0x07;

	uint8_t dv = get_reg3(p_cpu, d);

	alu_SBC(p_cpu, dv);

	DEBUG_PRINT("%04x:SBC A %s [%02x]\n", p_cpu->pc, str_reg3(d), dv);
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_AND_A_D(cpu_t *p_cpu)
{
	uint8_t d;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &d);
	d &= 0x07;

	uint8_t dv = get_reg3(p_cpu, d);

	alu_AND(p_cpu, dv);

	DEBUG_PRINT("%04x:AND A %s [%02x]\n", p_cpu->pc, str_reg3(d), dv);
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_XOR_A_D(cpu_t *p_cpu)
{
	uint8_t d;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &d);
	d &= 0x07;

	uint8_t dv = get_reg3(p_cpu, d);

	alu_XOR(p_cpu, dv);

	DEBUG_PRINT("%04x:XOR A %s [%02x]\n", p_cpu->pc, str_reg3(d), dv);
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_OR_A_D(cpu_t *p_cpu)
{
	uint8_t d;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &d);
	d &= 0x07;

	uint8_t dv = get_reg3(p_cpu, d);

	alu_OR(p_cpu, dv);

	DEBUG_PRINT("%04x:OR A %s [%02x]\n", p_cpu->pc, str_reg3(d), dv);
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_CP_A_D(cpu_t *p_cpu)
{
	uint8_t d;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &d);
	d &= 0x07;

	uint8_t dv = get_reg3(p_cpu, d);

	alu_CP(p_cpu, dv);

	DEBUG_PRINT("%04x:CP [%02x] A [%02x] %s [%02x]\n", p_cpu->pc, get_lsb(p_cpu->reg_AF), get_msb(p_cpu->reg_AF), str_reg3(d), dv);
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_ADD_A_HL(cpu_t *p_cpu)
{
	uint8_t value;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->reg_HL, &value);

	alu_ADD(p_cpu, value);

	DEBUG_PRINT("%04x:ADD A (HL) [%02x]\n", p_cpu->pc, value);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_ADC_A_HL(cpu_t *p_cpu)
{
	uint8_t value;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->reg_HL, &value);

	alu_ADC(p_cpu, value);

	DEBUG_PRINT("%04x:ADC A (HL) [%02x]\n", p_cpu->pc, value);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_SUB_A_HL(cpu_t *p_cpu)
{
	uint8_t value;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->reg_HL, &value);

	alu_SUB(p_cpu, value);

	DEBUG_PRINT("%04x:SUB A (HL) [%02x]\n", p_cpu->pc, value);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_SBC_A_HL(cpu_t *p_cpu)
{
	uint8_t value;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->reg_HL, &value);

	alu_SBC(p_cpu, value);

	DEBUG_PRINT("%04x:SBC A (HL) [%02x]\n", p_cpu->pc, value);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_AND_A_HL(cpu_t *p_cpu)
{
	uint8_t value;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->reg_HL, &value);

	alu_AND(p_cpu, value);

	DEBUG_PRINT("%04x:AND A (HL) [%02x]\n", p_cpu->pc, value);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_XOR_A_HL(cpu_t *p_cpu)
{
	uint8_t value;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->reg_HL, &value);

	alu_XOR(p_cpu, value);

	DEBUG_PRINT("%04x:XOR A (HL) [%02x]\n", p_cpu->pc, value);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_OR_A_HL(cpu_t *p_cpu)
{
	uint8_t value;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->reg_HL, &value);

	alu_OR(p_cpu, value);

	DEBUG_PRINT("%04x:OR A (HL) [%02x]\n", p_cpu->pc, value);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_CP_A_HL(cpu_t *p_cpu)
{
	uint8_t value;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->reg_HL, &value);

	alu_CP(p_cpu, value);

	DEBUG_PRINT("%04x:CP A (HL) [%02x]\n", p_cpu->pc, value);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_ADD_A_N(cpu_t *p_cpu)
{
	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	alu_ADD(p_cpu, n);

	DEBUG_PRINT("%04x:ADD A %02x\n", p_cpu->pc, n);
	p_cpu->pc += 2;
	return 8;
}

static int opcode8_ADC_A_N(cpu_t *p_cpu)
{
	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	alu_ADC(p_cpu, n);

	DEBUG_PRINT("%04x:ADC A %02x\n", p_cpu->pc, n);
	p_cpu->pc += 2;
	return 8;
}

static int opcode8_SUB_A_N(cpu_t *p_cpu)
{
	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	alu_SUB(p_cpu, n);

	DEBUG_PRINT("%04x:SUB A %02x\n", p_cpu->pc, n);
	p_cpu->pc += 2;
	return 8;
}

static int opcode8_SBC_A_N(cpu_t *p_cpu)
{
	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	alu_SBC(p_cpu, n);

	DEBUG_PRINT("%04x:SBC A %02x\n", p_cpu->pc, n);
	p_cpu->pc += 2;
	return 8;
}

static int opcode8_AND_A_N(cpu_t *p_cpu)
{
	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	alu_AND(p_cpu, n);

	DEBUG_PRINT("%04x:AND A %02x\n", p_cpu->pc, n);
	p_cpu->pc += 2;
	return 8;
}

static int opcode8_XOR_A_N(cpu_t *p_cpu)
{
	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	alu_XOR(p_cpu, n);

	DEBUG_PRINT("%04x:XOR A %02x\n", p_cpu->pc, n);
	p_cpu->pc += 2;
	return 8;
}

static int opcode8_OR_A_N(cpu_t *p_cpu)
{
	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	alu_OR(p_cpu, n);

	DEBUG_PRINT("%04x:OR A %02x\n", p_cpu->pc, n);
	p_cpu->pc += 2;
	return 8;
}

static int opcode8_CP_A_N(cpu_t *p_cpu)
{
	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	alu_CP(p_cpu, n);

	DEBUG_PRINT("%04x:CP A %02x\n", p_cpu->pc, n);
	p_cpu->pc += 2;
	return 8;
}

static int opcode8_POP_R(cpu_t *p_cpu)
{
	uint8_t r;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &r);
	r >>= 4;
	r &= 0x03;

	uint16_t value = pop_u16(p_cpu);

	set_reg1(p_cpu, r, value);

	DEBUG_PRINT("%04x:POP %s [%04x]\n", p_cpu->pc, str_reg1(r), value);
	p_cpu->pc += 1;
	return 12;
}

static int opcode8_PUSH_R(cpu_t *p_cpu)
{
	uint8_t r;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &r);
	r >>= 4;
	r &= 0x03;

	uint16_t value = get_reg1(p_cpu, r);

	push_u16(p_cpu, value);

	DEBUG_PRINT("%04x:PUSH %s [%04x]\n", p_cpu->pc, str_reg1(r), value);
	p_cpu->pc += 1;
	return 16;
}

//Push present address onto stack.
//Jump to address $0000 + n.
static int opcode8_RST_N(cpu_t *p_cpu)
{
	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &n);
	n &= 0x38;

	DEBUG_PRINT("%04x:RST %04x\n", p_cpu->pc, (uint16_t)n);

	p_cpu->pc += 1;
	push_pc(p_cpu);
	jump(p_cpu, n);
	return 32;
}

static int opcode8_RET(cpu_t *p_cpu)
{
	DEBUG_PRINT("%04x:RET\n", p_cpu->pc);
	pop_pc(p_cpu);
	return 8;
}

static int opcode8_RET_I(cpu_t *p_cpu)
{
	DEBUG_PRINT("%04x:RET I\n", p_cpu->pc);
	pop_pc(p_cpu);
	p_cpu->irq_master_enable = 1;
	return 8;
}

static int opcode8_RET_F(cpu_t *p_cpu)
{
	uint8_t m;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &m);
	m >>= 3;
	m &= 0x03;

	DEBUG_PRINT("%04x:RET %s [%01x]\n", p_cpu->pc, str_mnemonic(m), get_lsb(p_cpu->reg_AF));
	if (get_mnemonic(p_cpu, m))
	{
		pop_pc(p_cpu);
	}
	else
	{
		p_cpu->pc += 1;
	}
	return 8;
}

static int opcode8_JP_N(cpu_t *p_cpu)
{
	uint16_t n;
	(void)mmu_read_u16(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	DEBUG_PRINT("%04x:JP %04x\n", p_cpu->pc, n);
	jump(p_cpu, n);
	return 12;
}

static int opcode8_JP_F_N(cpu_t *p_cpu)
{
	uint8_t m;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &m);
	m >>= 3;
	m &= 0x03;

	uint16_t n;
	(void)mmu_read_u16(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	DEBUG_PRINT("%04x:JP %s [%02x] %04x\n", p_cpu->pc, str_mnemonic(m), get_lsb(p_cpu->reg_AF), n);
	if (get_mnemonic(p_cpu, m))
	{
		jump(p_cpu, n);
	}
	else
	{
		p_cpu->pc += 3;
	}

	return 12;
}

static int opcode8_CALL_N(cpu_t *p_cpu)
{
	uint16_t n;
	(void)mmu_read_u16(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	DEBUG_PRINT("%04x:CALL %04x\n", p_cpu->pc, n);

	p_cpu->pc += 3;
	push_pc(p_cpu);

	jump(p_cpu, n);
	return 12;
}

static int opcode8_CALL_F_N(cpu_t *p_cpu)
{
	uint8_t m;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc, &m);
	m >>= 3;
	m &= 0x03;

	uint16_t n;
	(void)mmu_read_u16(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	DEBUG_PRINT("%04x:CALL %s [%01x] %04x\n", p_cpu->pc, str_mnemonic(m), get_lsb(p_cpu->reg_AF), n);

	p_cpu->pc += 3;
	if (get_mnemonic(p_cpu, m))
	{
		push_pc(p_cpu);

		jump(p_cpu, n);
	}
	return 12;
}

static int opcode8_ADD_SP_N(cpu_t *p_cpu)
{
	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	set_flag_Z(p_cpu, 0);
	set_flag_N(p_cpu, 0);
	set_flag_C(p_cpu, ((p_cpu->sp & 0xFF) + n) > 0xFF);
	set_flag_H(p_cpu, ((p_cpu->sp & 0x0F) + (n & 0x0F)) > 0x0F);

	p_cpu->sp = p_cpu->sp + (int8_t)n;

	DEBUG_PRINT("%04x:ADD SP %02x\n", p_cpu->pc, n);
	p_cpu->pc += 2;
	return 16;
}

static int opcode8_LD_HL_SP_N(cpu_t *p_cpu)
{
	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	p_cpu->reg_HL = p_cpu->sp + (int8_t)n;

	set_flag_Z(p_cpu, 0);
	set_flag_N(p_cpu, 0);
	set_flag_H(p_cpu, ((p_cpu->sp & 0x0F) + (n & 0x0F)) > 0x0F);
	set_flag_C(p_cpu, ((p_cpu->sp & 0xFF) + n) > 0xFF);

	DEBUG_PRINT("%04x:LD HL SP+%02x [%04x]\n", p_cpu->pc, n, p_cpu->reg_HL);
	p_cpu->pc += 2;
	return 12;
}

static int opcode8_LD_FF00_N_A(cpu_t *p_cpu)
{
	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	uint8_t a = get_msb(p_cpu->reg_AF);

	(void)mmu_write_u8(p_cpu->p_mmu, (uint16_t)0xFF00 + n, a);

	DEBUG_PRINT("%04x:LD (%04x) A\n", p_cpu->pc, 0xFF00 + n, a);
	p_cpu->pc += 2;
	return 12;
}

static int opcode8_LD_A_FF00_N(cpu_t *p_cpu)
{
	uint8_t n;
	(void)mmu_read_u8(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	uint8_t a;
	(void)mmu_read_u8(p_cpu->p_mmu, (uint16_t)0xFF00 + n, &a);

	set_msb(&(p_cpu->reg_AF), a);

	DEBUG_PRINT("%04x:LD A (%04x) [%02x]\n", p_cpu->pc, 0xFF00 + n, a);
	p_cpu->pc += 2;
	return 12;
}

static int opcode8_LD_C_A(cpu_t *p_cpu)
{
	uint16_t addr = (uint16_t)0xFF00 + get_lsb(p_cpu->reg_BC);

	uint8_t a = get_msb(p_cpu->reg_AF);

	(void)mmu_write_u8(p_cpu->p_mmu, addr, a);

	DEBUG_PRINT("%04x:LD (%04x) A [%02x]\n", p_cpu->pc, addr, a);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LD_A_C(cpu_t *p_cpu)
{
	uint16_t addr = (uint16_t)0xFF00 + get_lsb(p_cpu->reg_BC);

	uint8_t a;
	(void)mmu_read_u8(p_cpu->p_mmu, addr, &a);

	set_msb(&(p_cpu->reg_AF), a);

	DEBUG_PRINT("%04x:LD A (%04x) [%02x]\n", p_cpu->pc, addr, a);
	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LD_N_A(cpu_t *p_cpu)
{
	uint16_t n;
	(void)mmu_read_u16(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	uint8_t a = get_msb(p_cpu->reg_AF);
	(void)mmu_write_u8(p_cpu->p_mmu, n, a);

	DEBUG_PRINT("%04x:LD (%04x) A [%02x]\n", p_cpu->pc, n, a);
	p_cpu->pc += 3;
	return 8;
}

static int opcode8_LD_A_N(cpu_t *p_cpu)
{
	uint16_t n;
	(void)mmu_read_u16(p_cpu->p_mmu, p_cpu->pc + 1, &n);

	uint8_t a;
	(void)mmu_read_u8(p_cpu->p_mmu, n, &a);

	set_msb(&(p_cpu->reg_AF), a);

	DEBUG_PRINT("%04x:LD A (%04x) [%02x]\n", p_cpu->pc, n, a);
	p_cpu->pc += 3;
	return 8;
}

static int opcode8_JP_HL(cpu_t *p_cpu)
{
	DEBUG_PRINT("%04x:JP HL\n", p_cpu->pc);
	jump(p_cpu, p_cpu->reg_HL);
	return 4;
}

static int opcode8_LD_SP_HL(cpu_t *p_cpu)
{
	p_cpu->sp = p_cpu->reg_HL;

	DEBUG_PRINT("%04x:LD SP HL\n", p_cpu->pc);
	p_cpu->pc += 1;
	return 8;
}

// This instruction disables interrupts but not
// immediately. Interrupts are disabled after
// instruction after DI is executed.
static int opcode8_DI(cpu_t *p_cpu)
{
	p_cpu->di_counter = 2;

	DEBUG_PRINT("%04x:DI\n", p_cpu->pc);
	p_cpu->pc += 1;
	return 4;
}

// This instruction disables interrupts but not
// immediately. Interrupts are disabled after
// instruction after DI is executed.
static int opcode8_EI(cpu_t *p_cpu)
{
	p_cpu->ei_counter = 2;

	DEBUG_PRINT("%04x:EI\n", p_cpu->pc);
	p_cpu->pc += 1;
	return 4;
}
