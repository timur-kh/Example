/******************************************************Library******************************************************/

#include "UART.h"

/******************************************************Use Function*************************************************/
void UART3_Init(void) {

	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	USART3->BRR = 0xD1;						// Speed = 115200; (24 000 000 + (115200 / 2)) / 115200 = 208 -> 0xD1
	USART3->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
//	USART3->CR1 |= USART_CR1_RXNEIE;
//	NVIC_EnableIRQ(USART3_IRQn);
}

void UART3_Send(char chr) {
	while (!(USART3->SR & USART_SR_TC));
	USART3->DR = chr;
}

void UART3_Send_String(char* str) {
	uint8_t i = 0;

	while (str[i])
		UART3_Send(str[i++]);
}

void UART3_Send_Number_Float(float data){

	char str[100];

	char *tmpSign = (data < 0) ? "-" : "";
	float tmpVal = (data < 0) ? -data : data;

	int tmpInt1 = tmpVal;                  	// Get the integer (678).
	float tmpFrac = tmpVal - tmpInt1;      	// Get fraction (0.0123).
	int tmpInt2 = trunc(tmpFrac * 100);  	// Turn into integer (123).	int tmpInt2 = trunc(tmpFrac * 10000)

	// Print as parts, note that you need 0-padding for fractional bit.

	sprintf (str, "%s%d.%02d", tmpSign, tmpInt1, tmpInt2);

	UART3_Send_String(str);

}
