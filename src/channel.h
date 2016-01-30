#ifndef CHANNEL_H
#define CHANNEL_H

#include "generator.h"
#include "envelope.h"

struct Channel
{
    //BASIC
    Generator gen;
    Envelope env;
    
    //LFOs
    //the lfo's are really low res(for speed and space) they work best at < 20 hz
    //at higher freqs they take BIG steps
    //e.g     0 0  0  0   0 50 50 50 50 ...
    //and not 0 10 20 30 40 50 60 70 80 as you would like them to
    Generator lfo[NUM_CHAN_LFO]; //these are the per channel lfo's: 1,3,5 unused in seq, 
    byte lfoMin[NUM_CHAN_LFO];
    smp_t lfoBuff[NUM_CHAN_LFO];
    byte lfoCount;

    //HZ-AD envelope
    Envelope ad_hz_env;
    uint ad_hz_min;
    uint ad_hz_max;



    //SCRAP?:
    //Envelope freqEnv[NUM_GEN];
};
typedef struct Channel Channel;

void channelInit(Channel* chan);

//internal buffer is RINGBUFF_AUDIO_SIZE
//will update all internal states: e.g. lfo's etc
void channelSynthesizeAndMix(Channel* chan, smp_t* mixBuff, int toDo);

#endif
