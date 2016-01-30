#include "xio.h"

//enum _XOPIN_IN_ADDR_ { XOP_IN_ADDR_A=XOP_NUM, XOP_IN_ADDR_B, XOP_IN_ADDR_C};
#define XIO_ADDR_MASK  ( _BV(XIO_PIN_A) | _BV(XIO_PIN_B) | _BV(XIO_PIN_C) )
//#define XIO_ADDR_SHIFT  XIO_PIN_A

void xioInit()
{
    //enable outputs
    XIO_DDR |= XIO_ADDR_MASK | _BV(XIO_PIN_5V); // | _BV(XIO_PIN_OUT);
    XIO_PORT |= _BV(XIO_PIN_5V);

    //enable inputs
    XIO_DDR &= ~_BV(XIO_PIN_IN_A) | _BV(XIO_PIN_IN_B);

    /*xOut(XOP0, false);
    xOut(XOP1, false);
    xOut(XOP2, false);
    xOut(XOP3, false);
    xOut(XOP4, false);*/
    
    /*xOut(XOP_IN_ADDR_A, false);
    xOut(XOP_IN_ADDR_B, false);
    xOut(XOP_IN_ADDR_C, false);*/
}

/*
void xOut(XOPIN pin, bool val)
{
    //set address 
    byte pinVal = (pin<<XIO_ADDR_SHIFT)&XIO_ADDR_MASK;
    XIO_PORT |= pinVal; //set on bits
    XIO_PORT &= ~XIO_ADDR_MASK | pinVal; //clear off bits, keeping all other pins as is

    //output data on pin
    if(val)XIO_PORT |= _BV(XIO_PIN_OUT);
    else XIO_PORT &= ~_BV(XIO_PIN_OUT);
}*/

void xIoSetAddr(byte pin)
{
    if (pin&0x01)XIO_PORT |= _BV(XIO_PIN_A);
    else XIO_PORT &= ~_BV(XIO_PIN_A);
    
    if (pin&0x02)XIO_PORT |= _BV(XIO_PIN_B);
    else XIO_PORT &= ~_BV(XIO_PIN_B);

    if (pin&0x04)XIO_PORT |= _BV(XIO_PIN_C);
    else XIO_PORT &= ~_BV(XIO_PIN_C);
}

bool xIn(byte pin)
{
    if (pin < 8)
    {
        xIoSetAddr(pin);
        
        //read data
        return (XIO_PIN & _BV(XIO_PIN_IN_A)) >> XIO_PIN_IN_A;
    }
    else
    {
        pin -=8;
        xIoSetAddr(pin);

        //read data
        return (XIO_PIN & _BV(XIO_PIN_IN_B)) >> XIO_PIN_IN_B;
    }
}
