#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "defines.h"

struct Envelope
{
    //these are between 0-0xffff
    uint a; 
    uint d;
    uint s; 
    uint r;
    
    uint dA;
    uint dD;
    uint dR;
    
    uint val; //current evelope value
    
    byte state;
};
typedef struct Envelope Envelope;


#define ENV_BYPASS 0
#define ENV_IDLE 1
#define ENV_A 2
#define ENV_D 3
#define ENV_S 4
#define ENV_R 5

void envInit(Envelope* env);
void envSetADSR(Envelope* env, uint a, uint d, uint s, uint r);
void envSetA(Envelope* env, uint a);
void envSetD(Envelope* env, uint d);
void envSetS(Envelope* env, uint s);
void envSetR(Envelope* env, uint r);
void envSetOn(Envelope* env, bool val);
bool envGetOn(Envelope* env);
//FIXME: envelope bypass should take arg
void envSetBypass(Envelope* env);
void envBuffApply(Envelope* env, smp_t* buffer, uint n);
//generates 0-SMP_MAX envelope for use in e.g. lp_cutoff
void envBuffGen(Envelope* env, smp_t* buffer, uint n, smp_t min, smp_t max);

//special functions to step special envelopes(e.g hz env) (optimized and low res)
void envADJumpValue(Envelope* env, uint* retval, uint steps, uint min, uint max);

#endif
