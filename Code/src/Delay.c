/****************************************************Library***************************************************/

#include <Delay.h>

//https://themagicsmoke.ru/courses/stm32/systick.html
/******************************************************Defines*************************************************/

volatile uint32_t ticks_delay = 0;

void SysTick_Init(void){

	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/100000 - 1); //10 us

}

void delay(const uint32_t milliseconds) {
    uint32_t start = ticks_delay;
    while((ticks_delay - start) < milliseconds);
}
