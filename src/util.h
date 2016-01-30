#ifndef UTIL_H
#define UTIL_H

#include "defines.h"

const char* itoa(int value);
const char* itoa_u(uint value);

//it is OK to use float here as long as argument is define, gcc will optimize it out
//min freq = 2hz
#define TIMER1_FREQ_TO_CNT(freq)                                \
    INT_MAX - (uint)( (F_CPU/(float)TIM1_DIV) / (float)(freq))

//min freq  76hz, watch resolotion though for higher freqs
#define TIMER0_FREQ_TO_CNT(freq)                                \
    BYTE_MAX - (byte)( (F_CPU/(float)TIM0_DIV) / (float)(freq))

#define nop() __asm__ __volatile__("nop")

#endif
