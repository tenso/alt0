#ifndef INPUT_H
#define INPUT_H

#include <avr/io.h>
#include "defines.h"

#define MAX_BUTTONS  3
#define MAX_ENCODERS 3

struct Button
{
    byte portNr;
    byte p;
    bool down;
};
typedef struct Button Button;

struct Encoder
{
    byte portNr;
    byte pA;
    byte pB;
    
    bool lastA;
    bool lastB;
    byte speed;
    byte delay;
};
typedef struct Encoder Encoder;


void inputAddButton(byte portNr, byte p);
void inputAddEncoder(byte portNr,byte pA, byte pB);
void inputSetCallbacks(void (*handleKeyDown)(int), void (*handleKeyUp)(int), void (*handleEncMove)(int, int));
void inputPoll();


//PRIVATE:
void inputEnable(byte portNr, byte pin);
bool inputRead(byte portNr, byte pin);
byte inputMapToPin(byte portNr, byte p);

#endif
