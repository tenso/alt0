#ifndef ERROR_H
#define ERROR_H

#include "defines.h"

#define ERROR_OK 0
#define ERROR_SND_ISR_LAG 1
#define ERROR_SEQ_ISR_LAG 2
#define ERROR_USART_OVERFLOW 3
#define ERROR_USART_PARSEFAIL 4
#define ERROR_SND_ISR_UNDERRUN 5
#define ERROR_DEBUG_1 6
#define ERROR_DEBUG_2 7
#define ERROR_LAST 7
#define ERROR_NUM 8

void errorInit();
void errorIncCount(byte error);
byte errorGetCount(byte error);

#endif
