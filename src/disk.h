#ifndef DISK_H
#define DISK_H

#define FILE_NAME_LEN 8 
#define FILE_FIRST_SIZE 35
#define FILE_SIZE 990
#define FILE_FIRST 0

#include "defines.h"

//ENDIANESS: little i.e int is saved as [lsb msb]

//BIG FIXME: this is tied to eeprom for now...

/*USAGE:
  fileInit(4096);
  fileSeekBegin();

  while(true)
  {
     if (!fileSeekNext(&fd))break;

     if (fd == FILE_FIRST)
     { 
       Option options;
       if (fileRead(fd, &options, sizeof(Option)))
       {
           loadOptions(&options);
       }
       else
       {
           error("file load");
       }
     }
     else
     {
         File file;
         ... same as above
     }
  }
  fileSeekBegin(); //leave file in good state
*/

//FIXME:
//struct Disk{};

struct FileHeader
{
    bool erased;              //formated or eraded: 1
    int usedBytes;            //formated or eraded: 0
    char name[FILE_NAME_LEN]; //kept
    //then byte[FILE_SIZE or FIRST_FILE_SIZE]; //formated or erased: junk
}; // 1+2+8 = 11
typedef struct FileHeader FileHeader;
#define OFF_TO_ERASED 0
#define OFF_TO_SIZE 1
#define OFF_TO_NAME 3
#define OFF_TO_DATA sizeof(FileHeader)

void fileInit(uint diskSize);
void fileFormatDisk();

//remember first comes FirstFile at fd=0
//returns false when no more files exist
bool fileSeekNext(int* nextFd);
void fileSeekBegin();
bool fileReadErased(int fd);
void fileReadName(int fd, char* name);
void fileWriteName(int fd, const char* name);
uint fileReadSize(int fd);
void fileErase(int fd);

//returns false if trying to write to much
bool fileWriteData(int fd, byte* data, uint len);

//returns -1 if file is unreadable, erased, to big etc
//otherwise returns file size
int fileReadData(int fd, byte* data, uint len);

#endif
