#include "commands.h"

#include "cmd.h"

#include "channel.h"
#include "main.h"

extern Channel channels[];

extern Generator lfo[];

extern Param param;
extern Envelope lpCutEnv;
extern DelayFb dfb;


//REG COMMANDS

void regCommands()
{
    cmdAdd(CMD_OSC, CMD_OSC_HZ, cmdSetOscHz, cmdGetOscHz);
    cmdAdd(CMD_OSC, CMD_OSC_ON, cmdSetOscOn, cmdGetOscOn);
    cmdAdd(CMD_OSC, CMD_OSC_AMP, cmdSetOscAmp, cmdGetOscAmp);
    cmdAdd(CMD_OSC, CMD_OSC_PHASE, cmdSetOscPhase, cmdGetOscPhase);
    cmdAdd(CMD_OSC, CMD_OSC_WAVE, cmdSetOscWave, cmdGetOscWave);
    cmdAdd(CMD_OSC, CMD_OSC_WS, cmdSetOscWsType, cmdGetOscWsType);
    cmdAdd(CMD_OSC, CMD_OSC_GLIDE, cmdSetOscGlide, cmdGetOscGlide);
    
    cmdAdd(CMD_ENV, CMD_ENV_ON, cmdSetEnvOn, cmdGetEnvOn);
    cmdAdd(CMD_ENV, CMD_ENV_PASS, cmdSetEnvBypass, cmdGetEnvBypass);
    cmdAdd(CMD_ENV, CMD_ENV_A, cmdSetEnvA, cmdGetEnvA);
    cmdAdd(CMD_ENV, CMD_ENV_D, cmdSetEnvD, cmdGetEnvD);
    cmdAdd(CMD_ENV, CMD_ENV_S, cmdSetEnvS, cmdGetEnvS);
    cmdAdd(CMD_ENV, CMD_ENV_R, cmdSetEnvR, cmdGetEnvR);
    
    cmdAdd(CMD_LP, CMD_LP_CUT, cmdSetLpCut, cmdGetLpCut);
    cmdAdd(CMD_LP, CMD_LP_RES, cmdSetLpRes, cmdGetLpRes);
    cmdAdd(CMD_LP, CMD_LP_ENV_ON, cmdSetLpEnvOn, cmdGetLpEnvOn);
    cmdAdd(CMD_LP, CMD_LP_ENV_PASS, cmdSetLpEnvBypass, cmdGetLpEnvBypass);
    cmdAdd(CMD_LP, CMD_LP_ENV_A, cmdSetLpEnvA, cmdGetLpEnvA);
    cmdAdd(CMD_LP, CMD_LP_ENV_D, cmdSetLpEnvD, cmdGetLpEnvD);
    cmdAdd(CMD_LP, CMD_LP_ENV_S, cmdSetLpEnvS, cmdGetLpEnvS);
    cmdAdd(CMD_LP, CMD_LP_ENV_R, cmdSetLpEnvR, cmdGetLpEnvR);

    cmdAdd(CMD_DFB, CMD_DFB_PASS, cmdSetDfbBypass, cmdGetDfbBypass);
    cmdAdd(CMD_DFB, CMD_DFB_GAIN, cmdSetDfbGain, cmdGetDfbGain);
    cmdAdd(CMD_DFB, CMD_DFB_DELAY, cmdSetDfbDelay, cmdGetDfbDelay);

    cmdAdd(CMD_SYS, CMD_SYS_LS, 0, cmdSysLs);
    cmdAdd(CMD_SYS, CMD_SYS_INFO, 0, cmdSysInfo);

    cmdAdd(CMD_LFO, CMD_LFO_HZ, cmdSetLfoHz, cmdGetLfoHz);
    cmdAdd(CMD_LFO, CMD_LFO_AMP, cmdSetLfoAmp, cmdGetLfoAmp);
    cmdAdd(CMD_LFO, CMD_LFO_WAVE, cmdSetLfoWave, cmdGetLfoWave);

    cmdAdd(CMD_ANALOG, CMD_ANALOG_OUT, cmdSetAnalogOut, cmdGetAnalogOut);
}

////COMMANDS

bool cmdSysLs(byte id, uint* retVal)
{
    if(id != 0)return false;

    /*char strBuff[30];
    
    CmdGroup* group = cmdGetGroup("osc");     
  
    byte i;
    for(i=0;i<NUM_CHANNELS;i++)
    {
        byte j;
        for(j=0;j<group->numParam;j++)
        {   
            cmdGroupGetStr(group, i, j, strBuff);
            usartSendStr(strBuff);
            usartSendStr("\n\r");
        }
    }

    group = cmdGetGroup("env");     
  
    for(i=0;i<NUM_CHANNELS;i++)
    {
        byte j;
        for(j=0;j<group->numParam;j++)
        {   
            cmdGroupGetStr(group, i, j, strBuff);
            usartSendStr(strBuff);
            usartSendStr("\n\r");
        }
    }

    group = cmdGetGroup("lp");     
    
    byte j;
    for(j=0;j<group->numParam;j++)
    {   
        cmdGroupGetStr(group, 0, j, strBuff);
        usartSendStr(strBuff);
        usartSendStr("\n\r");
    }

    group = cmdGetGroup("dfb");     
    
    for(j=0;j<group->numParam;j++)
    {   
        cmdGroupGetStr(group, 0, j, strBuff);
        usartSendStr(strBuff);
        usartSendStr("\n\r");
    }
    */
    return true;
}

bool cmdSysInfo(byte id, uint* retVal)
{
    if(id != 0)return false;
    
    *retVal = VERSION<<8 | 123;

/*
    usartSendStr(constStr(CS_FIRMWARE));
    usartSendStr(itoa(VERSION));
    usartSendStr(constStr(CS_LINE));
    
    usartSendStr(constStr(CS_CPU_CLOCK));
    usartSendStr(itoa(F_CPU_MHZ));
    usartSendStr(" Mhz\n\r");
    
    usartSendStr(constStr(CS_SAMPLE_RATE));
    usartSendStr(itoa(SMP_FREQ));
    usartSendStr(" Hz\n\r");

    usartSendStr(constStr(CS_BITS));
    usartSendStr(itoa(sizeof(smp_t)*8));
    usartSendStr("\n\r");

    usartSendStr(constStr(CS_BAUD));
    usartSendStr(itoa(BAUD));
    usartSendStr(" Baud\n\r");*/

    return true;
}

bool cmdSetOscHz(byte id, uint val)
{
    if(id >= NUM_CHANNELS)return false;
        
    genSetHz(&channels[id].gen, val);

    return true;
}

bool cmdGetOscHz(byte id, uint* retVal)
{
    if(id >= NUM_CHANNELS)return false;
    
    *retVal = channels[id].gen.hz;
    
    return true;
}

bool cmdSetOscOn(byte id, uint val)
{
    if(id >= NUM_CHANNELS)return false;

    genSetOn(&channels[id].gen, val > 0);

    return true;
}

bool cmdGetOscOn(byte id, uint* retVal)
{
    if(id >= NUM_CHANNELS)return false;
    
    *retVal = channels[id].gen.on;

    return true;
}

bool cmdSetOscAmp(byte id, uint val)
{
    if(id >= NUM_CHANNELS)return false;

    genSetAmp(&channels[id].gen, val);

    return true;
}

bool cmdGetOscAmp(byte id, uint* retVal)
{
    if(id >= NUM_CHANNELS)return false;
    
    *retVal =  channels[id].gen.amp;

    return true;
}

bool cmdSetOscPhase(byte id, uint val)
{
    if(id >= NUM_CHANNELS)return false;

    genSetPhase(&channels[id].gen, val);

    return true;
}

bool cmdGetOscPhase(byte id, uint* retVal)
{
    if(id >= NUM_CHANNELS)return false;
    
    *retVal =  channels[id].gen.phase;

    return true;
}

bool cmdSetOscWave(byte id, uint val)
{
    if(id >= NUM_CHANNELS)return false;

    genSetWave(&channels[id].gen, val);
    return true;
}

bool cmdGetOscWave(byte id, uint* retVal)
{
    if(id >= NUM_CHANNELS)return false;
    
    *retVal = channels[id].gen.wave;

    return true;
}

bool cmdSetOscWsType(byte id, uint val)
{
    if(id >= NUM_CHANNELS)return false;

    genSetWsType(&channels[id].gen, val);
    return true;
}

bool cmdGetOscWsType(byte id, uint* retVal)
{
    if(id >= NUM_CHANNELS)return false;
    
    *retVal = channels[id].gen.wave;

    return true;
}

bool cmdSetOscGlide(byte id, uint val)
{
    if(id >= NUM_CHANNELS)return false;
        
    genSetGlide(&channels[id].gen, val);

    return true;
}

bool cmdGetOscGlide(byte id, uint* retVal)
{
    if(id >= NUM_CHANNELS)return false;
    
    *retVal = channels[id].gen.glide;
    
    return true;
}

//ENVELOPE

bool cmdSetEnvA(byte id, uint val)
{
    if(id >= NUM_CHANNELS)return false;

    envSetA(&channels[id].env, val);
    
    return true;
}

bool cmdGetEnvA(byte id, uint* retVal)
{
    if(id >= NUM_CHANNELS)return false;
    
    *retVal = channels[id].env.a;

    return true;
}

bool cmdSetEnvD(byte id, uint val)
{
    if(id >= NUM_CHANNELS)return false;

    envSetD(&channels[id].env, val);
    
    return true;
}

bool cmdGetEnvD(byte id, uint* retVal)
{
    if(id >= NUM_CHANNELS)return false;
    
    *retVal = channels[id].env.d;

    return true;
}

bool cmdSetEnvS(byte id, uint val)
{
    if(id >= NUM_CHANNELS)return false;

    envSetS(&channels[id].env, val);
    
    return true;
}

bool cmdGetEnvS(byte id, uint* retVal)
{
    if(id >= NUM_CHANNELS)return false;
    
    *retVal = channels[id].env.s;

    return true;
}

bool cmdSetEnvR(byte id, uint val)
{
    if(id >= NUM_CHANNELS)return false;

    envSetR(&channels[id].env, val);
    
    return true;
}

bool cmdGetEnvR(byte id, uint* retVal)
{
    if(id >= NUM_CHANNELS)return false;
    
    *retVal = channels[id].env.r;

    return true;
}


bool cmdSetEnvOn(byte id, uint val)
{
    if(id >= NUM_CHANNELS)return false;

    envSetOn(&channels[id].env, val);
    
    return true;
}

bool cmdGetEnvOn(byte id, uint* retVal)
{
    if(id >= NUM_CHANNELS)return false;
    
    *retVal = channels[id].env.state != ENV_IDLE && channels[id].env.state != ENV_BYPASS;

    return true;
}

bool cmdSetEnvBypass(byte id, uint val)
{
    if(id >= NUM_CHANNELS)return false;

    if(val)envSetBypass(&channels[id].env);
    else
    {
        channels[id].env.state = ENV_IDLE;
    }
    
    return true;
}

bool cmdGetEnvBypass(byte id, uint* retVal)
{
    if(id >= NUM_CHANNELS)return false;
    
    *retVal = channels[id].env.state == ENV_BYPASS;

    return true;
}

bool cmdSetLpEnvOn(byte id, uint val)
{
    if ( id > 0) return false;

    envSetOn(&lpCutEnv, val);

    return true;
}
bool cmdGetLpEnvOn(byte id, uint* retVal)
{
    if ( id > 0) return false;

    *retVal = lpCutEnv.state != ENV_IDLE && lpCutEnv.state != ENV_BYPASS;

    return true;
}
bool cmdSetLpEnvBypass(byte id, uint val)
{
    if ( id > 0) return false;

    if(val)envSetBypass(&lpCutEnv);
    else
    {
        lpCutEnv.state = ENV_IDLE;
    }

    return true;
}
bool cmdGetLpEnvBypass(byte id, uint* retVal)
{
    if ( id > 0) return false;

    *retVal = lpCutEnv.state == ENV_BYPASS;

    return true;
}
bool cmdSetLpCut(byte id, uint val)
{
    if ( id > 0) return false;

    param.lpCutoff = val;
    dacOut(0, DAC_LPCUT, val);

    return true;
}
bool cmdGetLpCut(byte id, uint* retVal)
{
    if ( id > 0) return false;

    *retVal = param.lpCutoff;

    return true;
}
bool cmdSetLpRes(byte id, uint val)
{
    if ( id > 0) return false;

    param.lpRes = val;
    dacOut(0, DAC_LPRES, val);

    return true;
}
bool cmdGetLpRes(byte id, uint* retVal)
{
    if ( id > 0) return false;

    *retVal = param.lpRes;

    return true;
}

bool cmdSetLpEnvA(byte id, uint val)
{
    if(id >0)return false;

    envSetA(&lpCutEnv, val);
    
    return true;
}
bool cmdGetLpEnvA(byte id, uint* retVal)
{
    if(id > 0)return false;
    
    *retVal = lpCutEnv.a;

    return true;
}

bool cmdSetLpEnvD(byte id, uint val)
{
    if(id >0)return false;

    envSetD(&lpCutEnv, val);
    
    return true;
}
bool cmdGetLpEnvD(byte id, uint* retVal)
{
    if(id > 0)return false;
    
    *retVal = lpCutEnv.d;

    return true;
}

bool cmdSetLpEnvS(byte id, uint val)
{
    if(id >0)return false;

    envSetS(&lpCutEnv, val);
    
    return true;
}
bool cmdGetLpEnvS(byte id, uint* retVal)
{
    if(id > 0)return false;
    
    *retVal = lpCutEnv.s;

    return true;
}

bool cmdSetLpEnvR(byte id, uint val)
{
    if(id >0)return false;

    envSetR(&lpCutEnv, val);
    
    return true;
}
bool cmdGetLpEnvR(byte id, uint* retVal)
{
    if(id > 0)return false;
    
    *retVal = lpCutEnv.r;

    return true;
}


bool cmdSetDfbBypass(byte id, uint val)
{
    if ( id > 0) return false;

    delayFbSetBypass(&dfb, val);
    
    return true;
}
bool cmdGetDfbBypass(byte id, uint* retVal)
{
    if ( id > 0) return false;

    *retVal = dfb.bypass;

    return true;
}

bool cmdSetDfbGain(byte id, uint val)
{
    if ( id > 0) return false;

    delayFbSetGain(&dfb, val);
    
    return true;
}
bool cmdGetDfbGain(byte id, uint* retVal)
{
    if ( id > 0) return false;

    *retVal = dfb.gain;

    return true;
}
bool cmdSetDfbDelay(byte id, uint val)
{
    if ( id > 0) return false;

    delayFbSetDelay(&dfb, val);
    
    return true;
}
bool cmdGetDfbDelay(byte id, uint* retVal)
{
    if ( id > 0) return false;

    *retVal = dfb.delay;

    return true;
}


/*****/
/*LFO*/
/*****/

bool cmdSetLfoHz(byte id, uint val)
{
    //if(id >= NUM_LFO)return false;

    genSetHz(&(channels[id/NUM_CHAN_LFO].lfo[id%NUM_CHAN_LFO]), val);

    return true;
}

bool cmdGetLfoHz(byte id, uint* retVal)
{
    //if(id >= NUM_LFO)return false;
    
    *retVal = channels[id/NUM_CHAN_LFO].lfo[id%NUM_CHAN_LFO].hz;
    
    return true;
}


bool cmdSetLfoAmp(byte id, uint val)
{
    //if(id >= NUM_LFO)return false;

    //FIXME: ugly setting lfos as gen*2q
    genSetAmp(&(channels[id/NUM_CHAN_LFO].lfo[id%NUM_CHAN_LFO]), val);

    return true;
}

bool cmdGetLfoAmp(byte id, uint* retVal)
{
    //if(id >= NUM_LFO)return false;
    
    *retVal =  channels[id/NUM_CHAN_LFO].lfo[id%NUM_CHAN_LFO].amp;

    return true;
}

bool cmdSetLfoWave(byte id, uint val)
{
    //if(id >= NUM_LFO)return false;

    genSetWave(&(channels[id/NUM_CHAN_LFO].lfo[id%NUM_CHAN_LFO]), val);
    return true;
}

bool cmdGetLfoWave(byte id, uint* retVal)
{
    //if(id >= NUM_LFO)return false;
    
    *retVal = channels[id/NUM_CHAN_LFO].lfo[id%NUM_CHAN_LFO].wave;

    return true;
}

bool cmdSetAnalogOut(byte id, uint val)
{
    if(id != 0)return false;
    
    param.analogOut = val;
    if (param.analogOut >= ANALOG_NUM_OUT)param.analogOut = ANALOG_NUM_OUT-1;
    
    paramUpdateAnalogOut(&param);

    return true;
}

bool cmdGetAnalogOut(byte id, uint* retVal)
{
    if(id != 0)return false;
    
    *retVal = param.analogOut;

    return true;
}
