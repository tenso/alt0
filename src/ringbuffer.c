#include "ringbuffer.h"

void ring256Init(RingBuffer256* ring256)
{
    ring256->read = 0;
    ring256->write = 0;
}

bool ring256Read(RingBuffer256* ring256, byte* out)
{
    if(ring256->read == ring256->write)return false;
    
    *out = ring256->buff[ring256->read];
    
    ring256->read++;
    if(ring256->read >= RINGBUFF_256_SIZE)ring256->read = 0;

    return true;
}

bool ring256Write(RingBuffer256* ring256, byte in)
{
    uint head = ring256->write + 1;
    if (head >= RINGBUFF_256_SIZE) head = 0;

    if ( head == ring256->read)return false;

    ring256->buff[ring256->write] = in;
    ring256->write++;
    if (ring256->write >= RINGBUFF_256_SIZE) ring256->write = 0;

    return true;
}

uint ring256NumFree(RingBuffer256* ring256)
{
    if(ring256->read == ring256->write)return RINGBUFF_256_SIZE-1;

    if(ring256->read <= ring256->write)
        return (RINGBUFF_256_SIZE - 1) - (ring256->write - ring256->read);
    
    return (ring256->read - ring256->write) - 1;
}

/*****/
/*128*/
/*****/


void ring128Init(RingBuffer128* ring128)
{
    ring128->read = 0;
    ring128->write = 0;
}


bool ring128Read(RingBuffer128* ring128, byte* out)
{
    if(ring128->read == ring128->write)return false;
    
    *out = ring128->buff[ring128->read];
    
    ring128->read++;
    if(ring128->read >= RINGBUFF_128_SIZE)ring128->read = 0;

    return true;
}

bool ring128Write(RingBuffer128* ring128, byte in)
{
    uint head = ring128->write + 1;
    if (head >= RINGBUFF_128_SIZE) head = 0;

    if ( head == ring128->read)return false;

    ring128->buff[ring128->write] = in;
    ring128->write++;
    if (ring128->write >= RINGBUFF_128_SIZE) ring128->write = 0;

    return true;
}

uint ring128NumFree(RingBuffer128* ring128)
{
    if(ring128->read == ring128->write)return RINGBUFF_128_SIZE-1;

    if(ring128->read <= ring128->write)
        return (RINGBUFF_128_SIZE - 1) - (ring128->write - ring128->read);
    
    return (ring128->read - ring128->write) - 1;
}

/*****/
/*64*/
/*****/


void ring64Init(RingBuffer64* ring64)
{
    ring64->read = 0;
    ring64->write = 0;
}


bool ring64Read(RingBuffer64* ring64, byte* out)
{
    if(ring64->read == ring64->write)return false;
    
    *out = ring64->buff[ring64->read];
    
    ring64->read++;
    if(ring64->read >= RINGBUFF_64_SIZE)ring64->read = 0;

    return true;
}

bool ring64Write(RingBuffer64* ring64, byte in)
{
    uint head = ring64->write + 1;
    if (head >= RINGBUFF_64_SIZE) head = 0;

    if ( head == ring64->read)return false;

    ring64->buff[ring64->write] = in;
    ring64->write++;
    if (ring64->write >= RINGBUFF_64_SIZE) ring64->write = 0;

    return true;
}

uint ring64NumFree(RingBuffer64* ring64)
{
    if(ring64->read == ring64->write)return RINGBUFF_64_SIZE-1;

    if(ring64->read <= ring64->write)
        return (RINGBUFF_64_SIZE - 1) - (ring64->write - ring64->read);
    
    return (ring64->read - ring64->write) - 1;
}


/****/
/*32*/
/****/


void ring32Init(RingBuffer32* ring32)
{
    ring32->read = 0;
    ring32->write = 0;
}


bool ring32Read(RingBuffer32* ring32, byte* out)
{
    if(ring32->read == ring32->write)return false;
    
    *out = ring32->buff[ring32->read];
    
    ring32->read++;
    if(ring32->read >= RINGBUFF_32_SIZE)ring32->read = 0;

    return true;
}

bool ring32Write(RingBuffer32* ring32, byte in)
{
    uint head = ring32->write + 1;
    if (head >= RINGBUFF_32_SIZE) head = 0;

    if ( head == ring32->read)return false;

    ring32->buff[ring32->write] = in;
    ring32->write++;
    if (ring32->write >= RINGBUFF_32_SIZE) ring32->write = 0;

    return true;
}

uint ring32NumFree(RingBuffer32* ring32)
{
    if(ring32->read == ring32->write)return RINGBUFF_32_SIZE-1;

    if(ring32->read <= ring32->write)
        return (RINGBUFF_32_SIZE - 1) - (ring32->write - ring32->read);
    
    return (ring32->read - ring32->write) - 1;
}

/*******/
/*AUDIO*/
/*******/
void ringAudioInit(RingBufferAudio* ringAudio)
{
    ringAudio->read = 0;
    ringAudio->write = 0;
}

bool ringAudioRead(RingBufferAudio* ringAudio, smp_t* out)
{
    if(ringAudio->read == ringAudio->write)return false;
    
    *out = ringAudio->buff[ringAudio->read];
    
    ringAudio->read++;
    if(ringAudio->read >= RINGBUFF_AUDIO_SIZE)ringAudio->read = 0;

    return true;
}

bool ringAudioWrite(RingBufferAudio* ringAudio, smp_t in)
{
    uint head = ringAudio->write + 1;
    if (head >= RINGBUFF_AUDIO_SIZE) head = 0;

    if ( head == ringAudio->read)return false;

    ringAudio->buff[ringAudio->write] = in;
    ringAudio->write++;
    if (ringAudio->write >= RINGBUFF_AUDIO_SIZE) ringAudio->write = 0;

    return true;
}

uint ringAudioNumFree(RingBufferAudio* ringAudio)
{
    if(ringAudio->read == ringAudio->write)return RINGBUFF_AUDIO_SIZE-1;

    if(ringAudio->read <= ringAudio->write)
        return (RINGBUFF_AUDIO_SIZE - 1) - (ringAudio->write - ringAudio->read);
    
    return (ringAudio->read - ringAudio->write) - 1;
}
