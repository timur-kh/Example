/******************************************************Library******************************************************/

#include "SysInit.h"

/******************************************************Use Function*************************************************/

void RCC_Init(void) {						// Quartz 8 MHz

	RCC->CFGR |= RCC_CFGR_PLLXTPRE;			// PLLXTPRE set divider (/2) = 1 MHz

	RCC->CR |= RCC_CR_HSEON;				// On HSE
	while (!(RCC->CR & RCC_CR_HSERDY)) {};

	RCC->CFGR |= (RCC_CFGR_PLLMULL12);		// Setup PLLMULL set multiplier (x12) = 48 MHz

	RCC->CFGR |= RCC_CFGR_PLLSRC;			// PLLSRC set HSE

	RCC->CR |= RCC_CR_PLLON;				// ON PLL
	while (!(RCC->CR & RCC_CR_PLLRDY)) {};

	FLASH->ACR &= ~FLASH_ACR_LATENCY;		// Setup FLASH
	FLASH->ACR |= FLASH_ACR_LATENCY_1;

	RCC->CFGR |= RCC_CFGR_HPRE_DIV1; 		// Set not divider (AHB) = 48 MHz
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; 		// Set divider (/2) (APB1) = 24 MHz
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV4; 		// Set divider (/4) (APB2) = 12 MHz // for SPI1

	RCC->CFGR &= ~RCC_CFGR_SW; 				// Setup SW, select PLL
	RCC->CFGR |= RCC_CFGR_SW_PLL;

}

void GPIO_Init(void) {

	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN); // Enable clock portA, portB and Alternative function

	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;		// Disable JTAG for pins B3,B4 and A15

	//--SPI1: (A4 to A7); Output mode: Alternative function, PWM - Push Pull, max 10 MHz--//

	GPIOA->CRL &= ~GPIO_CRL_CNF5;			// Setup A5 pins PP, AF, 50MHz (SCK)
	GPIOA->CRL |= (GPIO_CRL_CNF5_1 | GPIO_CRL_MODE5);

	GPIOA->CRL &= ~(GPIO_CRL_CNF6 | GPIO_CRL_MODE6);	// Setup A6 pins input floating (MISO)
	GPIOA->CRL |=   GPIO_CRL_CNF6_0;

	GPIOA->CRL &= ~GPIO_CRL_CNF7;			// Setup A7 pins PP, AF, 50MHz (MOSI)
	GPIOA->CRL |= (GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7);

	GPIOA->CRL &= ~GPIO_CRL_CNF4;			// (SC)
	GPIOA->CRL |=  GPIO_CRL_MODE4;
	GPIOA->BSRR =  GPIO_BSRR_BR4;			// CS_LOW

	//--SPI2: (B9, B12 to B15); Output mode: Alternative function, PWM - Push Pull, max 10 MHz--//

	GPIOB->CRH &= ~GPIO_CRH_CNF13;			// Setup B13 pins PP, AF, 50MHz (SCK)
	GPIOB->CRH |= (GPIO_CRH_CNF13_1 | GPIO_CRH_MODE13);

	GPIOB->CRH &= ~(GPIO_CRH_CNF14 | GPIO_CRH_MODE14);// Setup B14 pins input floating (MISO)
	GPIOB->CRH |=   GPIO_CRH_CNF14_0;

	GPIOB->CRH &= ~GPIO_CRH_CNF15;			// Setup B15 pins PP, AF, 50MHz (MOSI)
	GPIOB->CRH |= (GPIO_CRH_CNF15_1 | GPIO_CRH_MODE15);

	GPIOB->CRH &= ~GPIO_CRH_CNF12;			// B12 (SC)
	GPIOB->CRH |=  GPIO_CRH_MODE12;
	GPIOB->BSRR |=  GPIO_BSRR_BR12;			// CS_LOW

	GPIOB->CRH &= ~GPIO_CRH_CNF9;			// B9 (PS)
	GPIOB->CRH |=  GPIO_CRH_MODE9;
	GPIOB->BSRR |=  GPIO_BSRR_BR9;			// PS_LOW

	//==============================================================//

	GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);	// Setup A0 pins input floating (INT1)
	GPIOA->CRL |= GPIO_CRL_CNF0_0;

	GPIOB->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);	// Setup B0 pins input floating (INT2)
	GPIOB->CRL |= GPIO_CRL_CNF0_0;

	//--UART3: (B10 - TX; B11 - RX); Output mode: Alternative function, UART - Push Pull, max 10 MHz--//

	GPIOB->CRH &= ~(GPIO_CRH_CNF10 | GPIO_CRH_MODE10);	// Setup B10 pins PP, AF, 10MHz
	GPIOB->CRH |= (GPIO_CRH_CNF10_1 | GPIO_CRH_MODE10_0);

	GPIOB->CRH &= ~(GPIO_CRH_CNF11 | GPIO_CRH_MODE11);	// Setup B11 pins PP, AF, 10MHz
	GPIOB->CRH |= (GPIO_CRH_CNF11_1 | GPIO_CRH_MODE11_0);

	//=======================================================//

	GPIOC->CRH &= ~GPIO_CRH_CNF13;
	GPIOC->CRH |=  GPIO_CRH_MODE13;
}
