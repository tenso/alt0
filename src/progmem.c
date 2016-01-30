#include "progmem.h"

#include <avr/pgmspace.h>

///////////
//STRINGS//
///////////

const char csBootHello[] PROGMEM = "\n\r\n\rAlt Zero booting...\n\r";
const char csBootGen[] PROGMEM = "init generators...";
const char csBootCommands[] PROGMEM = "register commands...";
const char csOk[] PROGMEM = "ok\n\r";
const char csAllDone[] PROGMEM = "all done\n\r\n\r";
const char csCmdFail[] PROGMEM = "<!> command failed:\n\r";
const char csCmdSetFail[] PROGMEM = "<!> group found, set failed:\n\r";
const char csCmdGetFail[] PROGMEM = "<!> group found, get failed:\n\r";
const char csCmdNotFound[] PROGMEM = "<!> no such command:\n\r";
const char csLt[] PROGMEM = "\n\r"; //line terminate
const char csFirmWare[] PROGMEM = "\n\rAlt Zero, firmware v.";
const char csLine[] PROGMEM = "\n\r------------------------\n\r";
const char csCpuClock[] PROGMEM = "CPU clock: ";
const char csSampleRate[] PROGMEM = "Sample Rate: ";
const char csBits[] PROGMEM = "Bits: ";
const char csBaud[] PROGMEM = "Baud Rate: ";
const char csEchoOn[] PROGMEM = "echo on\n\r";
const char csEchoOff[] PROGMEM = "echo off\n\r";

PGM_P _constStrings[] PROGMEM = {
    csBootHello,
    csBootGen,
    csBootCommands,
    csOk,
    csAllDone,
    csCmdFail,
    csCmdSetFail,
    csCmdGetFail,
    csCmdNotFound,
    csLt,
    csFirmWare,
    csLine,
    csCpuClock,
    csSampleRate,
    csBits,
    csBaud,
    csEchoOn,
    csEchoOff,
};

const char* constStr(uint i)
{
    //const str longer than this will be truncated
    static char constStrData[64];
    //get pointer
    PGM_P p;
    memcpy_P(&p, &_constStrings[i], sizeof(PGM_P));
    
    //read str
    strncpy_P(constStrData, p, 64);
    return constStrData;
}

////////
//KEYS//
////////

//starts at sub A (A0) goes to C8

const uint _keyToHzData[] PROGMEM = {27, 29, 31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 
                                55, 58, 62, 65, 69, 73, 78, 82, 87, 92, 98, 104, 
                                110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 
                                220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 
                                440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 
                                880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 
                                1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 
                                3520, 3729, 3951, 4186};


uint keyToHz(uint i)
{
    return pgm_read_word(&_keyToHzData[i]);
}
