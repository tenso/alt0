#include "waveshape.h"

void wsBuffAM(smp_t* buff, smp_t lfo0, smp_t lfo1, uint toDo)
{
    uint i=0;
    for(;i<toDo;i++)
    {
        int result = buff[i];
        result *= ( (int)lfo0 + (int)lfo1);
        buff[i] = (smp_t)(result >> 8);
    }
}

void wsBuffHardClip(smp_t* buff, smp_t lfo0, smp_t lfo1, uint toDo)
{
    uint i=0;
    for(;i<toDo;i++)
    {
        if(buff[i] > lfo0)buff[i] = lfo0;
        if(buff[i] < lfo1)buff[i] = lfo1;
    }
}

void wsBuffHardClipOffset(smp_t* buff, smp_t lfo0, smp_t lfo1, uint toDo)
{
    byte lfoVal = lfo0 + (127 + lfo1);
    byte lfoVal2 = (127 - lfo1) - lfo0;
    
    uint i=0;
    for(;i<toDo;i++)
    {
        if(buff[i] > lfoVal)buff[i] = lfoVal;
        if(buff[i] < lfoVal2)buff[i] = lfoVal2;
    }
}

void wsBuffTopInvert(smp_t* buff, smp_t lfo0, smp_t lfo1, uint toDo)
{
    uint i=0;
    for(;i<toDo;i++)
    {
        if(buff[i] > lfo0)
        {
            int res = (int)lfo0 - (int)(buff[i] - lfo0); //lfo0<<1 - buff[i]; 
            if (res >= 0 && res < SMP_MAX)buff[i] = res;
            else if (res < 0)buff[i] = 0;
            else buff[i] = SMP_MAX;
        }
        else if(buff[i] < lfo1)buff[i] = lfo1 + (lfo1 - buff[i]);
    }
}

void wsBuffOverdrive(smp_t* buff, smp_t lfo0, smp_t lfo1, uint toDo)
{
    uint i=0;
    for(;i<toDo;i++)
    {
        buff[i] *= lfo0;
        if (buff[i] > lfo1)buff[i] = lfo1;
    }
}

void wsBuffDestroy(smp_t* buff, smp_t lfo0, smp_t lfo1, uint toDo)
{
    uint i=0;
    for(;i<toDo;i++)
    {
        //if(buff[i] > lfo0)
        {
            buff[i] = lfo0 - (buff[i] - lfo0);
        }
    }
}
