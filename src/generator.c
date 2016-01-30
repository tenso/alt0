#include "generator.h"

#include <string.h>
#include "util.h"

void genInit(Generator* gen)
{
    genSetAmp(gen, GEN_DEF_AMP);
    genSetOn(gen, false);
    genSetWave(gen, OSC_SQU);
    genSetWsType(gen, WS_NONE);
    genSetHz(gen, 110);
    genSetPhase(gen, 0);
    genSync(gen);
    genSetGlide(gen, 0xffff);
    gen->step = 0;
    gen->mute = false;
}

void genSync(Generator* gen)
{
    gen->index = 0;
}

void genSetOn(Generator* gen, bool on)
{
    gen->on = on;
}

void genSetWave(Generator* gen, OscWave wave)
{
    gen->wave = wave;
}

void genSetWsType(Generator* gen, WsType wsType)
{
    gen->wsType = wsType;
}

//without this gen is undefined, i.e this is init also
void genSetHz(Generator* gen, uint hz)
{
    if (gen->hz == hz)return;
    gen->hz = hz;
    
    //gen->step = (uint)( ((long)(hz)<<16) / ((long)(SMP_FREQ)*10) );
    gen->targetStep = (uint)( ((long)(hz)<<16) / ((long)(SMP_FREQ)*10) );
    //gen->step = (uint)( ((long)(hz)<<16) / ((long)(SMP_FREQ)) );
    
    gen->glideIndex = GLIDE_DELAY;
}

void genSetGlide(Generator* gen, uint glide)
{
    gen->glide = glide;
}

void genSetPhase(Generator* gen, uint phase)
{
    gen->index +=  phase;
}

void genSetAmp(Generator* gen, smp_t amp)
{
    gen->amp = amp;
}

void genStepIndex(Generator* gen)
{
    if(gen->step != gen->targetStep)
    {
        if(gen->glideIndex >= GLIDE_DELAY)
        {
            if(gen->targetStep > gen->step)
            {
                uint last = gen->step;
                gen->step += gen->glide;
                if(gen->step > gen->targetStep)gen->step = gen->targetStep;
                else if(gen->step < last)gen->step = gen->targetStep;
            }
            else 
            {
                if(gen->glide > gen->step)gen->step = gen->targetStep;
                else
                {
                    gen->step -= gen->glide;
                    if(gen->step < gen->targetStep)gen->step = gen->targetStep;
                }
            }
            gen->glideIndex = 0;
        }
        else gen->glideIndex++;
    }
        
    gen->index += gen->step;
}

void genBuffDC(Generator* gen, smp_t* buff, uint num)
{
    memset(buff, gen->amp, num);
}

void genBuffSqu(Generator* gen, smp_t* buff, uint num)
{
    uint i=0;
    for(; i < num; i++)
    {
        genStepIndex(gen);
        
        if (gen->mute)buff[i] = 0;
        else if (gen->index <= 0x7fff)buff[i] = SMP_MIN;
        else buff[i] =  gen->amp;
    }
}

void genBuffSaw(Generator* gen, smp_t* buff, uint num)
{
    uint i=0;
    for(; i < num ; i++)
    {
        genStepIndex(gen);

        if (gen->mute)buff[i] = 0;
        else buff[i] = (smp_t) ( ((long)(gen->amp) * (long)(gen->index)) >> 16);
    }
    
}

void genBuffTri(Generator* gen, smp_t* buff, uint num)
{
    uint i=0;
    for(; i < num;i++)
    {
        genStepIndex(gen);

        if (gen->mute)buff[i] = 0;
        else if (gen->index < 0x7fff)
        {
            buff[i] = (smp_t) ( ((long)(gen->amp) * ((long)(gen->index))) >> 15);
        }
        else
        {
            buff[i] = gen->amp - (smp_t) ( ((long)(gen->amp) * ((long)(gen->index-0x7fff) )) >> 15);
        }
    }
}

void genBuffNoise(Generator* gen, smp_t* buff, uint num)
{
    if(num == 0)return;
    static uint last = 0; //seed
    
    //poor mans noise
    uint i=0;
    for(;i<num;i++)
    {
        
        //simple LCG
        //Xn+1 = (aXn + c) % m
        //a is prime, c and m is coprime

        last = 251*last + 113; // % 0xffff;
        if (gen->mute)buff[i] = 0;
        else buff[i] = (gen->amp*(last>>8))>>8;
    }
}

void genBuffMix(smp_t* buff0, smp_t* buff1, uint num)
{
    uint i=0;
    for(;i<num;i++)
    {
        buff0[i] = buff0[i] + buff1[i];
    }
}

void genBuffSet(smp_t* buff0, uint num, smp_t val)
{
    memset(buff0, val, num);
}


void genSetMute(Generator* gen, bool on)
{
    gen->mute = on;
}

void genBuffRotateRight(smp_t* buff, byte steps, uint num)
{
    uint i=0;
    for(;i<num;i++)
    {
        buff[i] = buff[i]>>steps;
    }
}
