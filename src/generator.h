#ifndef GENERATOR_H
#define GENERATOR_H

#include "defines.h"
#include "alt0cmd.h"

#define GLIDE_DELAY 150 //0-0xffff

struct Generator
{
    uint index;
    uint hz;
    //uint per;
    //uint hPer;
    smp_t amp; //defaults to GEN_DEF_AMP after genInit();
    byte phase;
    OscWave wave;
    bool on;
    WsType wsType;
    uint step;
    
    uint glide;
    uint targetStep;
    uint glideIndex;
    bool mute;
};

typedef struct Generator Generator;

/*INIT/SETUP*/

void genInit(Generator* gen);
void genSetOn(Generator* gen, bool on);
void genSetWave(Generator* gen, OscWave wave); //OscWave in alt0cmd.h
void genSetWsType(Generator* gen, WsType wsType); //WsType in alt0cmd.h
void genSetMute(Generator* gen, bool on);

//HZ is devided by 10 to get resolution e.g 100 gives 10hz fixme!
void genSetHz(Generator* gen, uint hz);
void genSetAmp(Generator* gen, smp_t amp);
void genSetPhase(Generator* gen, uint phase); ///phase is period*[0-255]/255
void genSync(Generator* gen);
void genSetGlide(Generator* gen, uint glide); //glide is inverted 1 is max glide 0xffff is none
/*GENERATE*/

void genBuffDC(Generator* gen, smp_t* buff, uint num);
void genBuffSqu(Generator* gen, smp_t* buff, uint num);
void genBuffSaw(Generator* gen, smp_t* buff, uint num);
void genBuffTri(Generator* gen, smp_t* buff, uint num);
void genBuffNoise(Generator* gen, smp_t* buff, uint num);

/*UTIL*/

/**result in gen0
beacuse of this mechanics the proper way to mix several gen's are
genMix(0,1) genMix(2,3) genMix(01,23)
**/
void genBuffMix(smp_t* buff0, smp_t* buff1, uint num);
void genBuffSet(smp_t* buff0, uint num, smp_t val);
void genBuffRotateRight(smp_t* buff, byte steps, uint num);
/*PRIVATE*/
void genStepIndex(Generator* gen);

#endif
