/******************************************************Library******************************************************/

#include "stm32f1xx.h"

/******************************************************Use Function*************************************************/

extern volatile uint32_t ticks_delay;

void SysTick_Init(void);
void delay(const uint32_t milliseconds);
