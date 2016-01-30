#ifndef MAIN_H
#define MAIN_H

#include "defines.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "channel.h"
#include "effect.h"

#include "ringbuffer.h"

#include "usart.h"
#include "cmd.h"
#include "commands.h"
#include "util.h"
#include "progmem.h"
#include "xio.h"
#include "dac.h"
#include "dogm.h"
#include "input.h"
#include "seq.h"
#include "az_menusys.h"
#include "error.h"
#include "disk.h"

//these would be better in a list but that takes sram
//defines for param:
/*#define AOUT_PORT  PORTC
#define AOUT_DDR   DDRC
#define AOUT_PIN_A PC0
#define AOUT_PIN_B PC1*/

#define AOUT_PORT  PORTB
#define AOUT_DDR   DDRB
#define AOUT_PIN_A PB0
#define AOUT_PIN_B PB1

struct Param
{
    byte selected;
    byte lpCutoff;
    byte lpRes;
    AnalogOut analogOut;
};
typedef struct Param Param;

enum ParamId{LP_CUTOFF=0 ,LP_RES, DFB_DELAY, DFB_GAIN, ANALOG_OUT, NUM_PARAM };
typedef enum ParamId ParamId;

//dont run this to often?
void paramInit(Param* param);
void paramUpdateDac(Param* param);
void paramUpdateAnalogOut(Param* param);

int main();
void synthesize();
void init();
void handleUsart();

void keyHandleDown(int id);
void keyHandleUp(int id);
void encHandleMove(int id, int speed);

void showParamSplash();

#endif
