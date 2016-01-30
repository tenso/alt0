#ifndef USART_H
#define USART_H

#include "defines.h"
#include <util/setbaud.h>

void usartInit();

void usartSetInt(bool rxdOn, bool txdOn);

void usartSend(char data);
void usartSendStr(const char* data);
void usartSendBytes(const byte* data, uint dataLen);
bool usartSendOk();

void usartRecv(char* data);
bool usartRecvOk();

bool usartError();
void usartFlush();

#endif
