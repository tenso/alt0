#include "eeprom.h"

#include <avr/io.h>

void eeInit(byte mode)
{
    //if (mode == EE_ATOMIC)
    {
        EECR &= ~_BV(EEPM1);
        EECR &= ~_BV(EEPM0);
    }
}
void eeWrite(uint addr, byte data)
{
    while ( EECR & _BV(EEPE));

    EEAR = addr;
    EEDR = data;
    EECR |= _BV(EEMPE);
    EECR |= _BV(EEPE);
}

byte eeRead(uint addr)
{
    while ( EECR & _BV(EEPE));

    EEAR = addr;
    EECR |= _BV(EERE);
    
    return EEDR;
}

void eeWriteWord(uint addr, int data)
{
    eeWrite(addr, data&0xff);
    eeWrite(addr+1, (data>>8)&0xff);
}

int eeReadWord(uint addr)
{
    byte ret[2];
    ret[0] = eeRead(addr);
    ret[1] = eeRead(addr+1);

    int val = (int)(ret[1])<<8 | ret[0];
    return val;
}
