#include "effect.h"

void delayFbInit(DelayFb* dfb)
{
    dfb->bypass = 0;
    dfb->delay = 100;
    dfb->gain = 0;
    dfb->inRing = 0;
}

void delayFbBuffApply(DelayFb* dfb, smp_t* buff, uint num)
{
    if(dfb->bypass)return;
    
    uint i=0;
    smp_t val = 0;

    for(;i<num;i++)
    {
        //overfull buffer, drop tail
        while (dfb->inRing > dfb->delay)
        {
            ringAudioRead(&dfb->ring, &val);
            dfb->inRing--;
        }

        if( dfb->inRing < dfb->delay) //fill buffer
        {
            dfb->inRing++;
            ringAudioWrite(&dfb->ring, buff[i]);
        }
        else //buffer full, start reading
        {
            ringAudioRead(&dfb->ring, &val);

            buff[i] += (smp_t) (((uint)val * (uint)dfb->gain)>>8);

            ringAudioWrite(&dfb->ring, buff[i]);
         }
    }
}

void delayFbSetDelay(DelayFb* dfb, byte val)
{
    dfb->delay = val;
}
void delayFbSetGain(DelayFb* dfb, byte val)
{
    dfb->gain = val;
}
void delayFbSetBypass(DelayFb* dfb, bool val)
{
    dfb->bypass = val;
}
