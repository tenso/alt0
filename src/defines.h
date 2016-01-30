#ifndef DEFINES_H
#define DEFINES_H

#define VERSION 1
#define F_CPU 20000000
#define F_CPU_MHZ 20
#define BAUD 38400
#define USART_BUFF_SIZE 6
#define EEPROM_SIZE 2048

typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned char bool;
typedef unsigned int uint;
typedef signed int sint;
#define true 1
#define false 0

typedef unsigned char smp_t;
#define SMP_MIN 0x00
#define SMP_MAX 0xff
#define GEN_DEF_AMP 75 //can have all osc going at same freq at same time


#define SMP_FREQ 12000U //8000U
#define SEQ_FREQ 1000U
#define TIM1_DIV 256
#define TIM0_DIV 1024

#define BYTE_MIN 0x00
#define BYTE_MAX 0xff //this is only the max value for generators, give some headroom for mixing etc...
#define INT_MAX 0xffff

#define NUM_CHANNELS 3
#define NUM_CHAN_LFO 2

#define MIN_LP_CUT 30
#define MAX_LP_RES 150
#define DAC_LPRES 0
#define DAC_LPCUT 1


//lfos are update at rate/LUL
//this means that hz=129 is hz'=1.0hz
//this means that hz=64 is hz'=0.5hz
//this means that hz=32 is hz'=0.25hz
//this means that hz=16 is hz'=0.125hz
//this means that hz=8  is hz'=0.0625hz
//this means that hz=4  is hz'=0.03125hz
//this means that hz=2  is hz'=0.0156hz
//this means that hz=1  is hz'=0.0078hz
//maximum speed is SMP_FREQ/LUL (for rate=12000hz ==> 90hz)
//this way lfo can be stored in just one byte

//this must be set to a value > (generator buffer length) /2
#define LFO_UPDATE_LAG 129 //200

#define LAST_NOTE 72

#endif
