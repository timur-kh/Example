/******************************************************Library******************************************************/

#include "stm32f1xx.h"
#include <stdio.h>
#include <math.h>

/******************************************************Use Function*************************************************/

void UART3_Init(void);
void UART3_Send(char chr);
void UART3_Send_String(char* str);
void UART3_Send_Number_Float(float data);
