#ifndef COMMANDS_H
#define COMMANDS_H

#include "defines.h"
#include "progmem.h"

//---
void regCommands();

//SYS
bool cmdSysLs(byte id, uint* retVal);
bool cmdSysInfo(byte id, uint* retVal);

//OSC
bool cmdSetOscHz(byte id, uint val);
bool cmdGetOscHz(byte id, uint* retVal);
bool cmdSetOscOn(byte id, uint val);
bool cmdGetOscOn(byte id, uint* retVal);
bool cmdSetOscAmp(byte id, uint val);
bool cmdGetOscAmp(byte id, uint* retVal);
bool cmdSetOscPhase(byte id, uint val);
bool cmdGetOscPhase(byte id, uint* retVal);
bool cmdSetOscWave(byte id, uint val);
bool cmdGetOscWave(byte id, uint* retVal);
bool cmdSetOscWsType(byte id, uint val);
bool cmdGetOscWsType(byte id, uint* retVal);
bool cmdSetOscGlide(byte id, uint val);
bool cmdGetOscGlide(byte id, uint* retVal);

//ENV
bool cmdSetEnvA(byte id, uint val);
bool cmdGetEnvA(byte id, uint* retVal);
bool cmdSetEnvD(byte id, uint val);
bool cmdGetEnvD(byte id, uint* retVal);
bool cmdSetEnvS(byte id, uint val);
bool cmdGetEnvS(byte id, uint* retVal);
bool cmdSetEnvR(byte id, uint val);
bool cmdGetEnvR(byte id, uint* retVal);
bool cmdSetEnvOn(byte id, uint val);
bool cmdGetEnvOn(byte id, uint* retVal);
bool cmdSetEnvBypass(byte id, uint val);
bool cmdGetEnvBypass(byte id, uint* retVal);

//LP
bool cmdSetLpCut(byte id, uint val);
bool cmdGetLpCut(byte id, uint* retVal);
bool cmdSetLpRes(byte id, uint val);
bool cmdGetLpRes(byte id, uint* retVal);
bool cmdSetLpEnvOn(byte id, uint val);
bool cmdGetLpEnvOn(byte id, uint* retVal);
bool cmdSetLpEnvBypass(byte id, uint val);
bool cmdGetLpEnvBypass(byte id, uint* retVal);
bool cmdSetLpEnvA(byte id, uint val);
bool cmdGetLpEnvA(byte id, uint* retVal);
bool cmdSetLpEnvD(byte id, uint val);
bool cmdGetLpEnvD(byte id, uint* retVal);
bool cmdSetLpEnvS(byte id, uint val);
bool cmdGetLpEnvS(byte id, uint* retVal);
bool cmdSetLpEnvR(byte id, uint val);
bool cmdGetLpEnvR(byte id, uint* retVal);

//delay fb

bool cmdSetDfbBypass(byte id, uint val);
bool cmdGetDfbBypass(byte id, uint* retVal);
bool cmdSetDfbGain(byte id, uint val);
bool cmdGetDfbGain(byte id, uint* retVal);
bool cmdSetDfbDelay(byte id, uint val);
bool cmdGetDfbDelay(byte id, uint* retVal);

//lfos

bool cmdSetLfoAmp(byte id, uint val);
bool cmdGetLfoAmp(byte id, uint* retVal);
bool cmdSetLfoHz(byte id, uint val);
bool cmdGetLfoHz(byte id, uint* retVal);
bool cmdSetLfoWave(byte id, uint val);
bool cmdGetLfoWave(byte id, uint* retVal);

bool cmdSetAnalogOut(byte id, uint val);
bool cmdGetAnalogOut(byte id, uint* retVal);

#endif
