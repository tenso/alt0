#ifndef PROGMEM_H
#define PROGMEM_H

#include "defines.h"

////////////
//PROG MEM//
////////////

//static strings go into program memory(flash, no sram used)
//only problem is they need to be read into sram before read by program...
enum csId {CS_BOOT_HELLO, CS_BOOT_GEN, CS_BOOT_COMMANDS, CS_OK, CS_ALL_DONE, CS_CMD_FAIL, 
           CS_CMD_SET_FAIL, CS_CMD_GET_FAIL, CS_CMD_NOT_FOUND, CS_LT,
           CS_FIRMWARE, CS_LINE, CS_CPU_CLOCK, CS_SAMPLE_RATE, CS_BITS, CS_BAUD,
           CS_ECHO_ON, CS_ECHO_OFF};

const char* constStr(uint i);

/////////
//NOTES//
/////////

//starts: A0, A#0, B0, C1
//use keyToHz(octave*12 + A,B etc);
//e.g. C1 is keyToHz(0*12+C)
//e.g. C4 is keyToHz(3*12+C)

uint keyToHz(uint i);

#define A 0 
#define As 1
#define B 2
#define C 3
#define Cs 4
#define D 5
#define Ds 6
#define E 7
#define F 8
#define Fs 9
#define G 10
#define Gs 11

#endif
