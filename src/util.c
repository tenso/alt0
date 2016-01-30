#include "util.h"

#include "defines.h"

const char* itoa(int value)
{
    static char valOut[6]; //5 digits = 99999 > INT_MAX
    static char ret[7]; //5 digits + NULL
    int valIndex = 0;
    bool neg = false;
    if (value < 0)
    {
        neg = true;
        value = -value;
    }
    
    while(value > 0)
    {
        valOut[valIndex++] = (value % 10) + '0';
        value /= 10;
    }
    
    if(valIndex == 0)
    {
        valOut[0] = '0';
        valIndex = 1;
    }

    if ( neg)
    {
        valOut[valIndex] = '-';
        valIndex++;
    }
    
    uint i;
    for(i=0;i<valIndex;i++)
    {
        ret[valIndex-1-i] = valOut[i];
    }
    ret[valIndex] = 0;

    return ret;
}

const char* itoa_u(uint value)
{
    static char valOut[6]; //5 digits = 99999 > INT_MAX
    static char ret[7]; //5 digits + NULL
    int valIndex = 0;
    bool neg = false;
    if (value < 0)
    {
        neg = true;
        value = -value;
    }
    
    while(value > 0)
    {
        valOut[valIndex++] = (value % 10) + '0';
        value /= 10;
    }
    
    if(valIndex == 0)
    {
        valOut[0] = '0';
        valIndex = 1;
    }

    if ( neg)
    {
        valOut[valIndex] = '-';
        valIndex++;
    }
    
    uint i;
    for(i=0;i<valIndex;i++)
    {
        ret[valIndex-1-i] = valOut[i];
    }
    ret[valIndex] = 0;

    return ret;
}
