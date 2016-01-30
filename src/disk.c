#include "disk.h"
#include "eeprom.h"
#include <avr/io.h>
#include <avr/interrupt.h>

int _diskSize;
int _seekAt;

void fileInit(uint diskSize)
{
    byte sreg = SREG;
    cli();
    eeInit(EE_ATOMIC);
    SREG = sreg;
    
    _diskSize = diskSize;
    fileSeekBegin();
}

void fileFormatDisk()
{
    fileSeekBegin();
    
    int i=0;
    while(true)
    {
        int fd;
        if (!fileSeekNext(&fd))break;
        
        //fileErase(fd);
        if (i==0)fileWriteName(fd, "first   ");
        if (i==1)fileWriteName(fd, "data0   ");
        if (i==2)fileWriteName(fd, "data1   ");
        if (i==3)fileWriteName(fd, "data2   ");
        i++;
        if (i==3)break;
    }

    fileSeekBegin();
}

bool fileSeekNext(int* nextFd)
{
    if(_seekAt == -1)
    {
        _seekAt = 0;
    }
    else if(_seekAt == 0)
    {
        _seekAt += FILE_FIRST_SIZE + sizeof(FileHeader);
    }
    else
    {
        _seekAt += FILE_SIZE + sizeof(FileHeader);
    }
    
    *nextFd = _seekAt;
    if (*nextFd + FILE_SIZE + sizeof(FileHeader) <= _diskSize)return true;    
    return false;

}
void fileSeekBegin()
{
    _seekAt = -1;
}
bool fileReadErased(int fd)
{
    byte sreg = SREG;
    cli();
    bool er = eeRead(fd + OFF_TO_ERASED);
    SREG = sreg;
    return er;
}
void fileReadName(int fd, char* name)
{
    byte sreg = SREG;
    cli();

    byte i=0;
    for (;i<FILE_NAME_LEN;i++)
    {
        name[i] = eeRead(fd + OFF_TO_NAME + i);
    }
    name[FILE_NAME_LEN] = '\0';
    SREG = sreg;
}

void fileWriteName(int fd, const char* name)
{
    byte sreg = SREG;
    cli();
    int i=0;
    for (;i<FILE_NAME_LEN;i++)
    {
        eeWrite(fd + OFF_TO_NAME + i, name[i]);
    }
    SREG = sreg;
}
uint fileReadSize(int fd)
{
    byte sreg = SREG;
    cli();
    uint size = eeReadWord(fd + OFF_TO_SIZE);
    SREG = sreg;
    return size;
}

void fileErase(int fd)
{
    byte sreg = SREG;
    cli();
    eeWrite(fd + OFF_TO_ERASED, 1);
    eeWriteWord(fd + OFF_TO_SIZE, 0);
    SREG = sreg;

    //fileWriteName(fd, "**free**");
}

bool fileWriteData(int fd, byte* data, uint len)
{
    if (fd == 0)
    {
        if (len > FILE_FIRST_SIZE)return false;
    }
    else
    {
        if (fd + len > _diskSize || len > FILE_SIZE)return false;
    }

    byte sreg = SREG;
    cli();

    eeWrite(fd + OFF_TO_ERASED, 1); //toggle erased
    eeWriteWord(fd + OFF_TO_SIZE, len);

    int dataFd = fd + OFF_TO_DATA;
    int i=0;
    for (;i<len;i++)
    {
        eeWrite(dataFd, data[i]);
        dataFd++;
    }
    eeWrite(fd + OFF_TO_ERASED, 0); //toggle erased
    SREG = sreg;
    return true;
}

int fileReadData(int fd, byte* data, uint len)
{
    if (fd == 0)
    {
        if (len > FILE_FIRST_SIZE)return -1;
    }
    else
    {
        if (fd + len > _diskSize || len > FILE_SIZE)return -1;
    }

    if (fileReadErased(fd))return -1;
    int want = fileReadSize(fd);
    if (want > len)return -1;
    
    byte sreg = SREG;
    cli();

    int dataFd = fd + OFF_TO_DATA;
    int i=0;
    for (;i<want;i++)
    {
        data[i] = eeRead(dataFd);
        dataFd++;
    }
    SREG = sreg;

    return want;
}
