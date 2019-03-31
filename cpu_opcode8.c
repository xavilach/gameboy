#include "cpu_opcode8.h"

#include "cpu_opcode16.h"
#include "cpu_opcode.h"
#include "cpu_registers.h"

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

	return opcode8_handlers[*(p_cpu->pc)](p_cpu);
}

/* Private function definitions */

//No operation.
static int opcode8_NOP(cpu_t *p_cpu)
{
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_LD_N_SP(cpu_t *p_cpu)
{
	uint16_t n = *(p_cpu->pc + 2);
	n <<= 8;
	n |= *(p_cpu->pc + 1);

	*(p_cpu->mem + n) = (uint16_t)(p_cpu->sp - p_cpu->mem);
	*(p_cpu->mem + n + 1) = (uint16_t)(p_cpu->sp - p_cpu->mem) >> 8;

	p_cpu->pc += 3;
	return 20;
}

static int opcode8_LD_R_N(cpu_t *p_cpu)
{
	uint8_t r = (*(p_cpu->pc) >> 4) & 0x03;
	uint16_t n = *(p_cpu->pc + 2);
	n <<= 8;
	n |= *(p_cpu->pc + 1);

	set_reg2(p_cpu, r, n);

	p_cpu->pc += 3;
	return 12;
}

static int opcode8_ADD_HL_R(cpu_t *p_cpu)
{
	uint8_t r = (*(p_cpu->pc) >> 4) & 0x03;
	uint16_t rv = get_reg2(p_cpu, r);
	uint32_t sum = p_cpu->reg_HL + rv;

	set_flag_N(p_cpu, 0);
	set_flag_H(p_cpu, (((p_cpu->reg_HL & 0x0FFF) + (rv & 0x0FFF)) & 0x1000) >> 12);
	set_flag_C(p_cpu, (sum >> 16) & 0x0001);

	p_cpu->reg_HL = sum;

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LD_R_A(cpu_t *p_cpu)
{
	uint8_t r = (*(p_cpu->pc) >> 4) & 0x03;
	uint16_t rv = get_reg2(p_cpu, r);

	*(p_cpu->mem + rv) = get_msb(p_cpu->reg_AF);

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LD_A_R(cpu_t *p_cpu)
{
	uint8_t r = (*(p_cpu->pc) >> 4) & 0x03;
	uint16_t rv = get_reg2(p_cpu, r);

	set_msb(p_cpu->reg_AF, *(p_cpu->mem + rv));

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_INC_R(cpu_t *p_cpu)
{
	uint8_t r = (*(p_cpu->pc) >> 4) & 0x03;

	set_reg2(p_cpu, r, get_reg2(p_cpu, r) + 1);

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_DEC_R(cpu_t *p_cpu)
{
	uint8_t r = (*(p_cpu->pc) >> 4) & 0x03;

	set_reg2(p_cpu, r, get_reg2(p_cpu, r) - 1);

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_INC_D(cpu_t *p_cpu)
{
	uint8_t d = (*(p_cpu->pc) >> 3) & 0x07;
	uint16_t dv = get_reg3(p_cpu, d);

	set_flag_H(p_cpu, (((dv & 0x0F) + 1) & 0x10) >> 4);

	dv += 1;
	set_reg3(p_cpu, d, dv);

	set_flag_Z(p_cpu, 0 == dv);
	set_flag_N(p_cpu, 1);

	p_cpu->pc += 1;
	return 4;
}

static int opcode8_INC_HL(cpu_t *p_cpu)
{
	uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

	set_flag_H(p_cpu, ((*p_value & 0x0F) + 1) >> 4);

	*p_value += 1;

	set_flag_Z(p_cpu, 0 == *p_value);
	set_flag_N(p_cpu, 1);

	p_cpu->pc += 1;
	return 12;
}

static int opcode8_DEC_D(cpu_t *p_cpu)
{
	uint8_t d = (*(p_cpu->pc) >> 3) & 0x07;
	uint16_t dv = get_reg3(p_cpu, d);

	set_flag_H(p_cpu, 0 == dv);

	dv -= 1;

	set_reg3(p_cpu, d, dv);

	set_flag_Z(p_cpu, 0 == dv);
	set_flag_N(p_cpu, 1);

	p_cpu->pc += 1;
	return 4;
}

static int opcode8_DEC_HL(cpu_t *p_cpu)
{
	uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

	set_flag_H(p_cpu, 0 == *p_value);

	*p_value -= 1;

	set_flag_Z(p_cpu, 0 == *p_value);
	set_flag_N(p_cpu, 1);

	p_cpu->pc += 1;
	return 12;
}

static int opcode8_LD_D_N(cpu_t *p_cpu)
{
	uint8_t d = (*(p_cpu->pc) >> 3) & 0x07;

	set_reg3(p_cpu, d, *(p_cpu->pc + 1));

	p_cpu->pc += 2;
	return 8;
}

static int opcode8_LD_HL_N(cpu_t *p_cpu)
{
	uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

	*p_value = *(p_cpu->pc + 1);

	p_cpu->pc += 2;
	return 12;
}

static int opcode8_RLCA(cpu_t *p_cpu)
{
	uint8_t reg_a = get_msb(p_cpu->reg_AF);
	uint8_t flag_c = (reg_a >> 7) & 0x01;
	reg_a = (reg_a << 1) | flag_c;

	set_msb(p_cpu->reg_AF, reg_a);

	set_flag_Z(p_cpu, reg_a == 0);
	set_flag_N(p_cpu, 0);
	set_flag_H(p_cpu, 0);
	set_flag_C(p_cpu, flag_c);

	p_cpu->pc += 1;
	return 4;
}

static int opcode8_RRCA(cpu_t *p_cpu)
{
	uint8_t reg_a = get_msb(p_cpu->reg_AF);
	uint8_t flag_c = reg_a & 0x01;
	reg_a = (reg_a >> 1) | (flag_c << 7);

	set_msb(p_cpu->reg_AF, reg_a);

	set_flag_Z(p_cpu, reg_a == 0);
	set_flag_N(p_cpu, 0);
	set_flag_H(p_cpu, 0);
	set_flag_C(p_cpu, flag_c);

	p_cpu->pc += 1;
	return 4;
}

static int opcode8_RLA(cpu_t *p_cpu)
{
	uint8_t reg_a = get_msb(p_cpu->reg_AF);
	uint8_t flag_c = (reg_a >> 7) & 0x01;
	reg_a = (reg_a << 1) | get_flag_C(p_cpu);

	set_msb(p_cpu->reg_AF, reg_a);

	set_flag_Z(p_cpu, reg_a == 0);
	set_flag_N(p_cpu, 0);
	set_flag_H(p_cpu, 0);
	set_flag_C(p_cpu, flag_c);

	p_cpu->pc += 1;
	return 4;
}

static int opcode8_RRA(cpu_t *p_cpu)
{
	uint8_t reg_a = get_msb(p_cpu->reg_AF);
	uint8_t flag_c = reg_a & 0x01;
	reg_a = (reg_a >> 1) | (get_flag_C(p_cpu) << 7);

	set_msb(p_cpu->reg_AF, reg_a);

	set_flag_Z(p_cpu, reg_a == 0);
	set_flag_N(p_cpu, 0);
	set_flag_H(p_cpu, 0);
	set_flag_C(p_cpu, flag_c);

	p_cpu->pc += 1;
	return 4;
}

//Halt CPU & LCD display until button pressed.
static int opcode8_STOP(cpu_t *p_cpu)
{
	p_cpu->pc += 1;
	return 4;
}

//Add n to current address and jump to it.
static int opcode8_JR_N(cpu_t *p_cpu)
{
	p_cpu->pc += *(p_cpu->pc + 1);
	return 8;
}

//If following condition is true then add n to current address and jump to it.
static int opcode8_JR_F_N(cpu_t *p_cpu)
{
	uint8_t m = (*p_cpu->pc >> 3) & 0x03;

	if (get_mnemonic(p_cpu, m))
	{
		p_cpu->pc += *(p_cpu->pc + 1);
	}

	return 8;
}

static int opcode8_LDI_HL_A(cpu_t *p_cpu)
{
	*(p_cpu->mem + p_cpu->reg_HL) = get_msb(p_cpu->reg_AF);
	p_cpu->reg_HL += 1;

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LDI_A_HL(cpu_t *p_cpu)
{
	set_msb(&p_cpu->reg_AF, *(p_cpu->mem + p_cpu->reg_HL));
	p_cpu->reg_HL += 1;

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LDD_HL_A(cpu_t *p_cpu)
{
	*(p_cpu->mem + p_cpu->reg_HL) = get_msb(p_cpu->reg_AF);
	p_cpu->reg_HL -= 1;

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LDD_A_HL(cpu_t *p_cpu)
{
	set_msb(&p_cpu->reg_AF, *(p_cpu->mem + p_cpu->reg_HL));
	p_cpu->reg_HL -= 1;

	p_cpu->pc += 1;
	return 8;
}

//Decimal adjust register A. This instruction adjusts register A so that the
//correct representation of Binary Coded Decimal (BCD) is obtained.
static int opcode8_DAA(cpu_t *p_cpu)
{
	uint8_t flag_c = get_flag_C(p_cpu);
	uint8_t flag_h = get_flag_H(p_cpu);
	uint8_t flag_n = get_flag_N(p_cpu);
	uint8_t reg_a = get_msb(p_cpu->reg_AF);

	set_flag_C(p_cpu, 0);

	if (!flag_n)
	{
		if (flag_c || reg_a > 0x99)
		{
			reg_a += 0x60;
			set_flag_C(p_cpu, 1);
		}
		if (flag_h || (reg_a & 0x0F) > 0x09)
		{
			reg_a += 0x6;
		}
	}
	else
	{
		if (flag_c)
		{
			reg_a -= 0x60;
		}
		if (flag_h)
		{
			reg_a -= 0x6;
		}
	}

	set_flag_Z(p_cpu, reg_a == 0);
	set_flag_H(p_cpu, 0);

	p_cpu->pc += 1;

	return 4;
}

//Complement A register. (Flip all bits.)
static int opcode8_CPL(cpu_t *p_cpu)
{
	uint8_t reg_a = ~get_msb(p_cpu->reg_AF);
	set_msb(&p_cpu->reg_AF, reg_a);

	set_flag_N(p_cpu, 1);
	set_flag_H(p_cpu, 1);

	p_cpu->pc += 1;
	return 4;
}

//Set Carry flag
static int opcode8_SCF(cpu_t *p_cpu)
{
	set_flag_C(p_cpu, 1);

	p_cpu->pc += 1;
	return 4;
}

//Complement carry flag.
static int opcode8_CCF(cpu_t *p_cpu)
{
	set_flag_C(p_cpu, ~get_flag_C(p_cpu));

	p_cpu->pc += 1;
	return 4;
}

static int opcode8_LD_D_D(cpu_t *p_cpu)
{
	uint8_t d0 = (*(p_cpu->pc) >> 3) & 0x07;
	uint8_t d1 = (*(p_cpu->pc) >> 0) & 0x07;

	set_reg3(p_cpu, d0, get_reg3(p_cpu, d1));

	p_cpu->pc += 1;
	return 4;
}

static int opcode8_LD_D_HL(cpu_t *p_cpu)
{
	uint8_t d = (*(p_cpu->pc) >> 3) & 0x07;

	uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

	set_reg3(p_cpu, d, *p_value);

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LD_HL_D(cpu_t *p_cpu)
{
	uint8_t d = *(p_cpu->pc) & 0x07;

	uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

	*p_value = get_reg3(p_cpu, d);

	p_cpu->pc += 1;
	return 8;
}

//Power down CPU until an interrupt occurs.
static int opcode8_HALT(cpu_t *p_cpu)
{
	p_cpu->pc += 1;
	return 4;
}

static int opcode8_ADD_A_D(cpu_t *p_cpu)
{
	uint8_t d = (*(p_cpu->pc) >> 0) & 0x07;
	uint8_t dv = get_reg3(p_cpu, d);

	alu_ADD(p_cpu, dv);

	p_cpu->pc += 1;
	return 4;
}

static int opcode8_ADC_A_D(cpu_t *p_cpu)
{
	uint8_t d = (*(p_cpu->pc) >> 0) & 0x07;
	uint8_t dv = get_reg3(p_cpu, d);

	alu_ADC(p_cpu, dv);

	p_cpu->pc += 1;
	return 4;
}

static int opcode8_SUB_A_D(cpu_t *p_cpu)
{
	uint8_t d = (*(p_cpu->pc) >> 0) & 0x07;
	uint8_t dv = get_reg3(p_cpu, d);

	alu_SUB(p_cpu, dv);

	p_cpu->pc += 1;
	return 4;
}

static int opcode8_SBC_A_D(cpu_t *p_cpu)
{
	uint8_t d = (*(p_cpu->pc) >> 0) & 0x07;
	uint8_t dv = get_reg3(p_cpu, d);

	alu_SBC(p_cpu, dv);

	p_cpu->pc += 1;
	return 4;
}

static int opcode8_AND_A_D(cpu_t *p_cpu)
{
	uint8_t d = (*(p_cpu->pc) >> 0) & 0x07;
	uint8_t dv = get_reg3(p_cpu, d);

	alu_AND(p_cpu, dv);

	p_cpu->pc += 1;
	return 4;
}

static int opcode8_XOR_A_D(cpu_t *p_cpu)
{
	uint8_t d = (*(p_cpu->pc) >> 0) & 0x07;
	uint8_t dv = get_reg3(p_cpu, d);

	alu_XOR(p_cpu, dv);

	p_cpu->pc += 1;
	return 4;
}

static int opcode8_OR_A_D(cpu_t *p_cpu)
{
	uint8_t d = (*(p_cpu->pc) >> 0) & 0x07;
	uint8_t dv = get_reg3(p_cpu, d);

	alu_OR(p_cpu, dv);

	p_cpu->pc += 1;
	return 4;
}

static int opcode8_CP_A_D(cpu_t *p_cpu)
{
	uint8_t d = (*(p_cpu->pc) >> 0) & 0x07;
	uint8_t dv = get_reg3(p_cpu, d);

	alu_CP(p_cpu, dv);

	p_cpu->pc += 1;
	return 4;
}

static int opcode8_ADD_A_HL(cpu_t *p_cpu)
{
	uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

	alu_ADD(p_cpu, *p_value);

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_ADC_A_HL(cpu_t *p_cpu)
{
	uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

	alu_ADC(p_cpu, *p_value);

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_SUB_A_HL(cpu_t *p_cpu)
{
	uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

	alu_SUB(p_cpu, *p_value);

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_SBC_A_HL(cpu_t *p_cpu)
{
	uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

	alu_SBC(p_cpu, *p_value);

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_AND_A_HL(cpu_t *p_cpu)
{
	uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

	alu_AND(p_cpu, *p_value);

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_XOR_A_HL(cpu_t *p_cpu)
{
	uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

	alu_XOR(p_cpu, *p_value);

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_OR_A_HL(cpu_t *p_cpu)
{
	uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

	alu_OR(p_cpu, *p_value);

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_CP_A_HL(cpu_t *p_cpu)
{
	uint8_t *p_value = p_cpu->mem + p_cpu->reg_HL;

	alu_CP(p_cpu, *p_value);

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_ADD_A_N(cpu_t *p_cpu)
{
	uint8_t n = *(p_cpu->pc + 1);

	alu_ADD(p_cpu, n);

	p_cpu->pc += 2;
	return 8;
}

static int opcode8_ADC_A_N(cpu_t *p_cpu)
{
	uint8_t n = *(p_cpu->pc + 1);

	alu_ADC(p_cpu, n);

	p_cpu->pc += 2;
	return 8;
}

static int opcode8_SUB_A_N(cpu_t *p_cpu)
{
	uint8_t n = *(p_cpu->pc + 1);

	alu_SUB(p_cpu, n);

	p_cpu->pc += 2;
	return 8;
}

static int opcode8_SBC_A_N(cpu_t *p_cpu)
{
	uint8_t n = *(p_cpu->pc + 1);

	alu_SBC(p_cpu, n);

	p_cpu->pc += 2;
	return 8;
}

static int opcode8_AND_A_N(cpu_t *p_cpu)
{
	uint8_t n = *(p_cpu->pc + 1);

	alu_AND(p_cpu, n);

	p_cpu->pc += 2;
	return 8;
}

static int opcode8_XOR_A_N(cpu_t *p_cpu)
{
	uint8_t n = *(p_cpu->pc + 1);

	alu_XOR(p_cpu, n);

	p_cpu->pc += 2;
	return 8;
}

static int opcode8_OR_A_N(cpu_t *p_cpu)
{
	uint8_t n = *(p_cpu->pc + 1);

	alu_OR(p_cpu, n);

	p_cpu->pc += 2;
	return 8;
}

static int opcode8_CP_A_N(cpu_t *p_cpu)
{
	uint8_t n = *(p_cpu->pc + 1);

	alu_CP(p_cpu, n);

	p_cpu->pc += 2;
	return 8;
}

static int opcode8_POP_R(cpu_t *p_cpu)
{
	uint8_t r = ((*p_cpu->pc) >> 4) & 0x03;
	uint16_t value = stack_pop(p_cpu);
	value <<= 8;
	value |= stack_pop(p_cpu);

	set_reg1(p_cpu, r, value);

	p_cpu->pc += 1;
	return 12;
}

static int opcode8_PUSH_R(cpu_t *p_cpu)
{
	uint8_t r = ((*p_cpu->pc) >> 4) & 0x03;

	push_u16(p_cpu, get_reg1(p_cpu, r));

	p_cpu->pc += 1;
	return 16;
}

//Push present address onto stack.
//Jump to address $0000 + n.
static int opcode8_RST_N(cpu_t *p_cpu)
{
	uint16_t addr = (*p_cpu->pc) & 0x38;

	push_pc(p_cpu);

	jump(p_cpu, addr);
	return 32;
}

static int opcode8_RET(cpu_t *p_cpu)
{
	pop_pc(p_cpu);
	return 8;
}

static int opcode8_RET_I(cpu_t *p_cpu)
{
	pop_pc(p_cpu);
	p_cpu->irq_mask = 0xFF;
	return 8;
}

static int opcode8_RET_F(cpu_t *p_cpu)
{
	uint8_t m = (*p_cpu->pc >> 3) & 0x03;

	if (get_mnemonic(p_cpu, m))
	{
		pop_pc(p_cpu);
	}

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_JP_N(cpu_t *p_cpu)
{
	uint16_t addr = *(p_cpu->pc + 2);
	addr <<= 8;
	addr |= *(p_cpu->pc + 1);

	jump(p_cpu, addr);
	return 12;
}

static int opcode8_JP_F_N(cpu_t *p_cpu)
{
	uint8_t m = (*p_cpu->pc >> 3) & 0x03;

	uint16_t addr = *(p_cpu->pc + 2);
	addr <<= 8;
	addr |= *(p_cpu->pc + 1);

	if (get_mnemonic(p_cpu, m))
	{
		jump(p_cpu, addr);
	}
	else
	{
		p_cpu->pc += 3;
	}

	return 12;
}

static int opcode8_CALL_N(cpu_t *p_cpu)
{
	uint16_t addr = *(p_cpu->pc + 2);
	addr <<= 8;
	addr |= *(p_cpu->pc + 1);

	p_cpu->pc += 3;
	push_pc(p_cpu);

	jump(p_cpu, addr);
	return 12;
}

static int opcode8_CALL_F_N(cpu_t *p_cpu)
{
	uint8_t m = (*p_cpu->pc >> 3) & 0x03;

	uint16_t addr = *(p_cpu->pc + 2);
	addr <<= 8;
	addr |= *(p_cpu->pc + 1);

	p_cpu->pc += 3;

	if (get_mnemonic(p_cpu, m))
	{
		push_pc(p_cpu);

		jump(p_cpu, addr);
	}

	return 12;
}

static int opcode8_ADD_SP_N(cpu_t *p_cpu)
{
	uint8_t n = *(p_cpu->pc + 1);

	uint16_t reg_sp = p_cpu->sp - p_cpu->mem;
	uint32_t result = reg_sp + n;

	set_flag_N(p_cpu, 0);
	set_flag_Z(p_cpu, 0);
	set_flag_H(p_cpu, ((reg_sp & 0x0FFF) + n) >> 12);
	set_flag_C(p_cpu, result >> 16);

	p_cpu->sp = p_cpu->mem + (result & 0xFFFF);

	p_cpu->pc += 2;
	return 16;
}

static int opcode8_LD_HL_SP_N(cpu_t *p_cpu)
{
	uint8_t n = *(p_cpu->pc + 1);

	uint16_t reg_sp = p_cpu->sp - p_cpu->mem;
	uint32_t result = reg_sp + n;

	set_flag_N(p_cpu, 0);
	set_flag_Z(p_cpu, 0);
	set_flag_H(p_cpu, ((reg_sp & 0x0FFF) + n) >> 12);
	set_flag_C(p_cpu, result >> 16);

	p_cpu->reg_HL = result;

	p_cpu->pc += 2;
	return 12;
}

static int opcode8_LD_FF00_N_A(cpu_t *p_cpu)
{
	uint8_t n = *(p_cpu->pc + 1);

	*(p_cpu->mem + 0xFF00 + n) = get_msb(p_cpu->reg_AF);

	p_cpu->pc += 2;
	return 12;
}

static int opcode8_LD_A_FF00_N(cpu_t *p_cpu)
{
	uint8_t n = *(p_cpu->pc + 1);

	set_msb(&(p_cpu->reg_AF), *(p_cpu->mem + 0xFF00 + n));

	p_cpu->pc += 2;
	return 12;
}

static int opcode8_LD_C_A(cpu_t *p_cpu)
{
	*(p_cpu->mem + 0xFF00 + get_lsb(p_cpu->reg_BC)) = get_msb(p_cpu->reg_AF);

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LD_A_C(cpu_t *p_cpu)
{
	set_msb(&(p_cpu->reg_AF), *(p_cpu->mem + 0xFF00 + get_lsb(p_cpu->reg_BC)));

	p_cpu->pc += 1;
	return 8;
}

static int opcode8_LD_N_A(cpu_t *p_cpu)
{
	uint16_t n = *(p_cpu->pc + 2);
	n <<= 8;
	n |= *(p_cpu->pc + 1);

	*(p_cpu->mem + n) = get_msb(p_cpu->reg_AF);

	p_cpu->pc += 2;
	return 8;
}

static int opcode8_LD_A_N(cpu_t *p_cpu)
{
	uint16_t n = *(p_cpu->pc + 2);
	n <<= 8;
	n |= *(p_cpu->pc + 1);

	set_msb(&(p_cpu->reg_AF), *(p_cpu->mem + n));

	p_cpu->pc += 2;
	return 8;
}

static int opcode8_JP_HL(cpu_t *p_cpu)
{
	jump(p_cpu, p_cpu->reg_HL);
	return 4;
}

static int opcode8_LD_SP_HL(cpu_t *p_cpu)
{
	p_cpu->sp = p_cpu->mem + p_cpu->reg_HL;

	p_cpu->pc += 2;
	return 8;
}

// This instruction disables interrupts but not
// immediately. Interrupts are disabled after
// instruction after DI is executed.
static int opcode8_DI(cpu_t *p_cpu)
{
	p_cpu->di_counter = 2;

	p_cpu->pc += 1;
	return 4;
}

// This instruction disables interrupts but not
// immediately. Interrupts are disabled after
// instruction after DI is executed.
static int opcode8_EI(cpu_t *p_cpu)
{
	p_cpu->ei_counter = 2;

	p_cpu->pc += 1;
	return 4;
}
