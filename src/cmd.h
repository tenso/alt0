#ifndef CMD_H
#define CMD_H

#include "defines.h"

#include "alt0cmd.h"

#define CMD_MAX_PARAM 10

//
enum CmdCode {CMD_FAILED, CMD_SET_OK, CMD_GET_OK, CMD_GET_FAILED, CMD_SET_FAILED, CMD_NOT_FOUND};

typedef enum CmdCode CmdCode;

typedef bool (*cmdSetFunc)(byte id, uint val);
typedef bool (*cmdGetFunc)(byte id, uint* retVal);

void cmdAdd(CmdGroupId group, byte param, cmdSetFunc setFunc, cmdGetFunc getFunc);
CmdCode cmdExec(byte* cmd, uint* retVal);

//void cmdMakePrintStr(CmdGroupId group, byte paramIndex, byte id, char* ret);


#endif
