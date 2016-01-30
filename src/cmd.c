#include "cmd.h"
#include <string.h>

#include "util.h"

//these are nulled by c
cmdSetFunc _setCommands[CMD_GROUP_SIZE][CMD_MAX_PARAM];
cmdGetFunc _getCommands[CMD_GROUP_SIZE][CMD_MAX_PARAM];

void cmdAdd(CmdGroupId group, byte param, cmdSetFunc setFunc, cmdGetFunc getFunc)
{
    _setCommands[group][param] = setFunc;
    _getCommands[group][param] = getFunc;
}

CmdCode cmdExec(byte* cmd, uint* retVal)
{
    if (cmd[0] == CMD_SET)
    {
        if (_setCommands[cmd[1]][cmd[2]] == 0)
        {
            return CMD_NOT_FOUND;
        }
        
        int val = cmd[4] << 8 | cmd[5];
        bool ret = (_setCommands[cmd[1]][cmd[2]]) (cmd[3], val);
        
        if(ret)return CMD_SET_OK;
        return CMD_SET_FAILED;
    }
    if (cmd[0] == CMD_GET)
    {
        if (_getCommands[cmd[1]][cmd[2]] == 0)
        {
            return CMD_NOT_FOUND;
        }
        
        bool ret = (_getCommands[cmd[1]][cmd[2]]) (cmd[3], retVal);
        
        if(ret)return CMD_GET_OK;
        return CMD_GET_FAILED;
    }
    return CMD_FAILED;
}


/*void cmdMakePrintStr(CmdGroupId group, byte paramIndex, byte id, char* ret)
{
    ret[0] = 0;
    
    strcat(ret, cmdGroup->group);
    strcat(ret, "/");
    char idStr[2];
    idStr[0] = id + '0';
    idStr[1] = 0;
    strcat(ret, idStr);
    strcat(ret, "/");
    strcat(ret, cmdGroup->params[nameIndex].name);
    strcat(ret, "=");
    
    int value = 1; //some never change value, then '1' represents ok ... fixme
    cmdGroup->params[nameIndex].getFunc(id, &value);
    
    strcat(ret, itoa(value));
}*/
