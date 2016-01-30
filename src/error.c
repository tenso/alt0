#include "error.h"
#include "menu.h"

byte _errors[ERROR_NUM];

void errorInit()
{
    int i=0;
    for (;i<ERROR_NUM;i++)
    {
        _errors[i] = 0;
    }
}

void errorIncCount(byte error)
{
    if (_errors[error]<0xff)
        _errors[error]++;

    //menuSystemPageRefresh(MENU_ERRORS, 0);
    //FIXME: jump to error page
}

byte errorGetCount(byte error)
{
    return _errors[error];
}
