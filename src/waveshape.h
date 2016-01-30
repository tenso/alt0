#ifndef WAVESHAPE_H
#define WAVESHAPE_H

#include "defines.h"

//wave shaping

//result is buff*(lf0+lfo1) (scaled to 0-255)
void wsBuffAM(smp_t* buff, smp_t lfo0, smp_t lfo1, uint toDo);

//clips signal to max: lfo0  min: lfo1
void wsBuffHardClip(smp_t* buff, smp_t lfo0, smp_t lfo1, uint toDo);

//clips signal around 127 +/- lfo1 + modulated by lfo0
void wsBuffHardClipOffset(smp_t* buff, smp_t lfo0, smp_t lfo1, uint toDo);

//inverts everything over lfo0 and under lfo1
void wsBuffTopInvert(smp_t* buff, smp_t lfo0, smp_t lfo1, uint toDo);

//inverts everything over lfo0 and under lfo1
void wsBuffOverdrive(smp_t* buff, smp_t lfo0, smp_t lfo1, uint toDo);

void wsBuffDestroy(smp_t* buff, smp_t lfo0, smp_t lfo1, uint toDo);

#endif
