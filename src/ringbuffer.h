#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "defines.h"

#define RINGBUFF_AUDIO_SIZE 256
#define RINGBUFF_256_SIZE 256
#define RINGBUFF_128_SIZE 128
#define RINGBUFF_64_SIZE 64
#define RINGBUFF_32_SIZE 32 //this is 32/6  5 cmds

/**\brief Super simple ring with size RINGBUFF_SIZE-1 (one left for boundary check) bytes
 **/

struct RingBuffer256
{
    byte buff[RINGBUFF_256_SIZE];
    uint read;
    uint write;
};

typedef struct RingBuffer256 RingBuffer256;

//this also acts like a clear
void ring256Init(RingBuffer256* ring);

/**
   \retval true  sucess
   \retval false  ring empty
**/

bool ring256Read(RingBuffer256* ring, byte* out);


/**
\retval true byte inserted
\retval false ring full
**/

bool ring256Write(RingBuffer256* ring, byte in);

uint ring256NumFree(RingBuffer256* ring);


//same as above but 128
struct RingBuffer128
{
    byte buff[RINGBUFF_128_SIZE];
    uint read;
    uint write;
};
typedef struct RingBuffer128 RingBuffer128;

void ring128Init(RingBuffer128* ring);
bool ring128Read(RingBuffer128* ring, byte* out);
bool ring128Write(RingBuffer128* ring, byte in);
uint ring128NumFree(RingBuffer128* ring);

//same as above but 64
struct RingBuffer64
{
    byte buff[RINGBUFF_64_SIZE];
    uint read;
    uint write;
};
typedef struct RingBuffer64 RingBuffer64;

void ring64Init(RingBuffer64* ring);
bool ring64Read(RingBuffer64* ring, byte* out);
bool ring64Write(RingBuffer64* ring, byte in);
uint ring64NumFree(RingBuffer64* ring);


//same as above but 32er

struct RingBuffer32
{
    byte buff[RINGBUFF_32_SIZE];
    uint read;
    uint write;
};
typedef struct RingBuffer32 RingBuffer32;

void ring32Init(RingBuffer32* ring);
bool ring32Read(RingBuffer32* ring, byte* out);
bool ring32Write(RingBuffer32* ring, byte in);
uint ring32NumFree(RingBuffer32* ring);


struct RingBufferAudio
{
    smp_t buff[RINGBUFF_AUDIO_SIZE];
    uint read;
    uint write;
};

typedef struct RingBufferAudio RingBufferAudio;

void ringAudioInit(RingBufferAudio* ring);
bool ringAudioRead(RingBufferAudio* ring, smp_t* out);
bool ringAudioWrite(RingBufferAudio* ring, smp_t in);
uint ringAudioNumFree(RingBufferAudio* ring);


#endif
