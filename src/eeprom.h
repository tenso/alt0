#ifndef EEPROM_H
#define EEPROM_H

#include "defines.h"

//FIXME: check for flash read in SPMCSR?

#define EE_ATOMIC 0 //i.e erase + write in one op

void eeInit(byte mode);
void eeWrite(uint addr, byte data);
byte eeRead(uint addr);

//little endian
void eeWriteWord(uint addr, int data);
int eeReadWord(uint addr);

#endif
