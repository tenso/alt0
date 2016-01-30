#ifndef EFFECT_H
#define EFFECT_H

#include "ringbuffer.h"

/*DELAY FEEDBACK*/

struct DelayFb
{
    RingBufferAudio ring;
    byte inRing;
    byte delay;
    byte gain;
    bool bypass;
};

typedef struct DelayFb DelayFb;

void delayFbInit(DelayFb* dfb);
void delayFbBuffApply(DelayFb* dfb, smp_t* buff, uint num);

void delayFbSetDelay(DelayFb* dfb, byte val);
void delayFbSetGain(DelayFb* dfb, byte val);
void delayFbSetBypass(DelayFb* dfb, bool val);

/*WAVE SHAPER*/

#endif
