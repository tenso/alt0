#ifndef DOGM_H
#define DOGM_H

#include <avr/io.h>
#include "defines.h"

#define SPI 0
#define DM_MODE SPI
/*
#define DM_PORT PORTC
#define DM_DDR  DDRC
//PINS
#define DM_SI  PC2
#define DM_CLK PC3
#define DM_CSB PC4
#define DM_RS  PC5*/

#define DM_PORT PORTD
#define DM_DDR  DDRD
//PINS
#define DM_SI  PD4
#define DM_CLK PD6
#define DM_CSB PD7
#define DM_RS  PD5

bool dmInit();
bool dmInitSPI();

void dmPrint(byte x, byte y, const char* str);
void dmPutc(byte x, byte y, const char c);

void dmSendString(const char* str);
void dmSetCursor(byte x, byte y);
//make sure from is smaller or equal than to!!!
void dmClear(byte fromX, byte fromY, byte toX, byte toY);
void dmClearDisplay();
void dmEntryMode(bool moveCursorRight, bool shiftDisplay);
void dmDoubleFont(bool on, bool doubleOnFirstRow);
void dmDisplayMode(bool on, bool cursorOn, bool blinkOn);

void dmSendData(byte data);
void dmSendCommand(byte data);
void dmSendByte(byte data);

byte bits(bool b7, bool b6, bool b5, bool b4, bool b3, bool b2, bool b1, bool b0);

#endif
