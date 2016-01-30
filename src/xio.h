#ifndef XIO_H
#define XIO_H

#include <avr/io.h>
#include "defines.h"

//eXtended I/O (muxes/latched)

#define XIO_DDR DDRA
#define XIO_PORT PORTA
#define XIO_PIN PINA

//adress pins:
#define XIO_PIN_A PA2
#define XIO_PIN_B PA3
#define XIO_PIN_C PA4

#define XIO_PIN_5V PA7

//#define XIO_PIN_OUT PD5
#define XIO_PIN_IN_A PA0
#define XIO_PIN_IN_B PA1

void xioInit();
bool xIn(byte pin);
//void xOut(XOPIN pin, bool val);

#endif
