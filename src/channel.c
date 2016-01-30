#include "channel.h"
#include "waveshape.h"
#include "ringbuffer.h"
#include "progmem.h"

smp_t _genBuff[RINGBUFF_AUDIO_SIZE];

void channelInit(Channel* chan)
{
    chan->lfoCount = 0;

    int j=0;
    for (;j<NUM_CHAN_LFO;j++)
    {
        genSetHz(&(chan->lfo[j]), 30000);
        genInit(&(chan->lfo[j]));
        genSetOn(&(chan->lfo[j]), true);
        chan->lfoMin[j] = 0;
    }

    genInit(&(chan->gen));
    genSetOn(&(chan->gen), true);
    genSetHz(&(chan->gen), keyToHz(12));
    genSetWave(&(chan->gen), 1);
    envInit(&(chan->env));
    envSetOn(&(chan->env), false);
    genSetGlide(&(chan->gen), 0xffff);
        
    //HZ ENVELOPES
    envInit(&(chan->ad_hz_env));
    chan->ad_hz_env.state = ENV_BYPASS;
    chan->ad_hz_min = 250;
    chan->ad_hz_max = 1900;
    envSetADSR(&(chan->ad_hz_env), 80,900,0,0);
}

void channelSynthesizeAndMix(Channel* chan, smp_t* mixBuff, int toDo)
{
    chan->lfoCount += toDo; //toDo can never be greater than 255

    while(chan->lfoCount >= LFO_UPDATE_LAG) //actually this only happens once
    {
        uint j=0;
        for(;j<NUM_CHAN_LFO;j++)
        {
            if(!chan->lfo[j].on)continue;
    
            if (chan->lfo[j].wave == OSC_SQU)genBuffSqu(&chan->lfo[j], &chan->lfoBuff[j], 1);
            else if (chan->lfo[j].wave == OSC_SAW)genBuffSaw(&chan->lfo[j], &chan->lfoBuff[j], 1);
            else if (chan->lfo[j].wave == OSC_TRI)genBuffTri(&chan->lfo[j], &chan->lfoBuff[j], 1);
            else if (chan->lfo[j].wave == OSC_NOISE)genBuffNoise(&chan->lfo[j], &chan->lfoBuff[j], 1);
            else if (chan->lfo[j].wave == OSC_DC)genBuffDC(&chan->lfo[j], &chan->lfoBuff[j], 1);
        }
        chan->lfoCount -= LFO_UPDATE_LAG;
    }
            
    if(!chan->gen.on)return;
            
    if (chan->ad_hz_env.state != ENV_BYPASS)
    {
        uint nextHz;
        envADJumpValue(&chan->ad_hz_env, &nextHz, toDo, chan->ad_hz_min, chan->ad_hz_max);
        genSetHz(&chan->gen, nextHz);
    }

    if (chan->gen.wave == OSC_SQU)genBuffSqu(&chan->gen, _genBuff, toDo);
    else if (chan->gen.wave == OSC_SAW)genBuffSaw(&chan->gen, _genBuff, toDo);
    else if (chan->gen.wave == OSC_TRI)genBuffTri(&chan->gen, _genBuff, toDo);
    else if (chan->gen.wave == OSC_NOISE)genBuffNoise(&chan->gen, _genBuff, toDo);
    else if (chan->gen.wave == OSC_DC)genBuffDC(&chan->gen, _genBuff, toDo);
            
    if (chan->gen.wsType == WS_NONE)
    {
    }
    else if (chan->gen.wsType == WS_AM)
    {
        wsBuffAM(_genBuff, chan->lfoBuff[0] + chan->lfoMin[0], 0, toDo);
    }
    else if (chan->gen.wsType == WS_HARDCLIP)
    {
        wsBuffHardClip(_genBuff, chan->lfoBuff[0] + chan->lfoMin[0], 0, toDo);
    }
    else if (chan->gen.wsType == WS_HARDCLIP_OFF)
    {
        wsBuffHardClipOffset(_genBuff, chan->lfoBuff[0] + chan->lfoMin[0], 0, toDo);
    }
    else if (chan->gen.wsType == WS_TOPINVERT)
    {
        wsBuffTopInvert(_genBuff, chan->lfoBuff[0] + chan->lfoMin[0], 0, toDo);
    }
    else if (chan->gen.wsType == WS_OVERDRIVE)
    {
        wsBuffOverdrive(_genBuff, chan->lfoBuff[0] + chan->lfoMin[0], 75, toDo);
    }
    else if (chan->gen.wsType == WS_DESTROY)
    {
        wsBuffDestroy(_genBuff, chan->lfoBuff[0] + chan->lfoMin[0], 0, toDo);
    }
            
    envBuffApply(&chan->env, _genBuff, toDo);
                
    genBuffMix(mixBuff, _genBuff, toDo);
}
