#include "usart.h"

#include <avr/io.h>

void usartInit()
{
#ifndef UBRR_VALUE
#error "BAUD not defined"
#endif

    UBRR0 = UBRR_VALUE;
    UCSR0B |= _BV(RXEN0) | _BV(TXEN0);
}

void usartSetInt(bool rxdOn, bool txdOn)
{
    if (rxdOn)
    {
        UCSR0B |= _BV(RXCIE0);
    }
    if (txdOn)
    {
        UCSR0B |= _BV(TXCIE0);
    }
}

void usartSend(char data)
{
    while(! (UCSR0A & _BV(UDRE0)) );

    UDR0 = data;
}

void usartSendStr(const char* data)
{
    int i=0;
    while(1)
    {
        if (data[i] == 0)break;

        while(! (UCSR0A & _BV(UDRE0)) );

        UDR0 = data[i];
        i++;
    }
}

void usartSendBytes(const byte* data, uint dataLen)
{
    uint i=0;
    for(;i<dataLen;i++)
    {
        while(! (UCSR0A & _BV(UDRE0)) );

        UDR0 = data[i];
    }
}

bool usartSendOk()
{
    return (UCSR0A & _BV(UDRE0));
}

void usartRecv(char* data)
{
    while(! (UCSR0A & _BV(RXC0)) );
    
    *data = UDR0;
}

    
bool usartRecvOk()
{
    return UCSR0A & _BV(RXC0);
}

bool usartError()
{
    return (UCSR0A & FE0) | (UCSR0A & DOR0) | (UCSR0A & UPE0);
}

void usartFlush()
{
    char dummy;
    while(UCSR0A & _BV(RXC0))dummy = UDR0;
}
