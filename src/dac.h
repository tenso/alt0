#ifndef DAC_H
#define DAC_H

#include <avr/io.h>

#include "defines.h"

/*
#define DAC_DATA_PORT PORTA
#define DAC_DATA_DDR  DDRA
#define DAC_CTRL_PORT PORTB
#define DAC_CTRL_DDR  DDRB

#define DAC_CS_0_PIN PB0
#define DAC_WRE_PIN PB1 //write enable
#define DAC_SEL_PIN PB2 //dac A/B
#define DAC_CS_1_PIN PB3
*/

#define DAC_DATA_PORT PORTC
#define DAC_DATA_DDR  DDRC

#define DAC_CTRL_PORT PORTB
#define DAC_CTRL_DDR  DDRB
#define DAC_CTRL_PORT_CS_0 PORTD
#define DAC_CTRL_DDR_CS_0  DDRD

#define DAC_CS_0_PIN PD2
#define DAC_WRE_PIN PB2 //write enable
#define DAC_SEL_PIN PB3 //dac A/B
#define DAC_CS_1_PIN PB4

#define DAC_A 0 
#define DAC_B 1
#define DAC_CHIP_0 0
#define DAC_CHIP_1 1
#define DAC_CHIP_NONE 2


////////
//DACS//
////////
//chip0:A -> lpres 
//chip0:B -> lpcut 
//chip1:A -> audio 
//chip1:B -> unused 

//use this:
void dacInit();
void dacOut(byte chip, byte dac, byte data);

//dont use these:
void dacSetChip(byte chip);
void dacSetSelected(byte dac);
void dacSetWrite(bool on);
void dacWrite(byte data);

#endif
