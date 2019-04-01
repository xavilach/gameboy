#ifndef CPU_BASE_H_
#define CPU_BASE_H_

#include "mmu.h"

#include <stdint.h>
#include <stdlib.h>

/* Typedefs */

typedef struct cpu_s
{
	uint16_t reg_AF;
	uint16_t reg_BC;
	uint16_t reg_DE;
	uint16_t reg_HL;

	uint16_t sp;
	uint16_t pc;

	mmu_t *p_mmu;

	uint8_t irq_master_enable;

	int di_counter;
	int ei_counter;
} cpu_t;

/* Inlined function definitions */

static inline void set_msb(uint16_t *p_word, uint8_t value)
{
	*p_word &= (uint16_t)0x00FF;
	*p_word |= ((uint16_t)value) << 8;
}

static inline void set_lsb(uint16_t *p_word, uint8_t value)
{
	*p_word &= (uint16_t)0xFF00;
	*p_word |= (uint16_t)value;
}

static inline uint8_t get_msb(uint16_t word)
{
	return (uint8_t)(word >> 8);
}

static inline uint8_t get_lsb(uint16_t word)
{
	return (uint8_t)word;
}

#endif /*CPU_BASE_H_*/
