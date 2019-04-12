#ifndef CPU_UTILS_H_
#define CPU_UTILS_H_

#include <stdint.h>

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

#endif /*CPU_UTILS_H_*/
