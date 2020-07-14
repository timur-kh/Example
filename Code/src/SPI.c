/******************************************************Library******************************************************/

#include "SPI.h"

/******************************************************Use Function*************************************************/


void SPI1_Init(void)
{

	RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN | RCC_APB2ENR_AFIOEN);
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	SPI1->CR1 &= ~SPI_CR1_SPE;                	// Disable SPI1

	SPI1->CR1 &= ~SPI_CR1_DFF;                	// 8 bit data
	SPI1->CR1 &= ~SPI_CR1_BIDIMODE;           	// 4 wire
	SPI1->CR1 &= ~SPI_CR1_RXONLY;             	// write and read, else only read
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;           	// MSB will be first
	SPI1->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);	// Software slave management & Internal slave select

	SPI1->CR1 &= ~SPI_CR1_BR;                	//Baud rate = Fpclk/32 = 12 MHz / 4 = 3 MHz
	SPI1->CR1 |=  SPI_CR1_BR_0;

	SPI1->CR1 |=  SPI_CR1_MSTR;               	// Mode Master
	SPI1->CR1 |=  SPI_CR1_CPOL;              	// Polarity clk signal CPOL = 1;
	SPI1->CR1 |=  SPI_CR1_CPHA;               	// Phase clk signal    CPHA = 1;

	SPI1->CR2 |= (SPI_CR2_TXEIE | SPI_CR2_RXNEIE);

	SPI1->CR2 |= 1<<SPI_CR2_TXDMAEN_Pos;
	SPI1->CR2 |= 1<<SPI_CR2_RXDMAEN_Pos;

	SPI1->CR1 |= SPI_CR1_SPE;                	//Enable SPI1
}

void SPI2_Init(void)
{

	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	RCC->AHBENR  |= RCC_AHBENR_DMA1EN;

	SPI2->CR1 &= ~SPI_CR1_SPE;                	// Disable SPI2

	SPI2->CR1 &= ~SPI_CR1_DFF;                	// 8 bit data
	SPI2->CR1 &= ~SPI_CR1_BIDIMODE;           	// 4 wire
	SPI2->CR1 &= ~SPI_CR1_RXONLY;             	// write and read, else only read
	SPI2->CR1 &= ~SPI_CR1_LSBFIRST;           	// MSB will be first
	SPI2->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);	// Software slave management & Internal slave select
	SPI2->CR1 &= ~SPI_CR1_BR;                	// Baud rate = Fpclk/32 = 24 MHz / 8 = 3 MHz
	SPI2->CR1 |=  SPI_CR1_BR_1;
	SPI2->CR1 |=  SPI_CR1_MSTR;               	// Mode Master
	SPI2->CR1 |=  SPI_CR1_CPOL;              	// Polarity clk signal CPOL = 1;
	SPI2->CR1 |=  SPI_CR1_CPHA;               	// Phase clk signal    CPHA = 1;

	SPI2->CR2 |= (SPI_CR2_TXEIE | SPI_CR2_RXNEIE);

	SPI2->CR2 |= 1<<SPI_CR2_TXDMAEN_Pos;
	SPI2->CR2 |= 1<<SPI_CR2_RXDMAEN_Pos;

	SPI2->CR1 |= SPI_CR1_SPE;                //Enable SPI2
}

void SPI1_DMA_Send(uint8_t *data, uint16_t len)
{
  //отключаем канал DMA после предыдущей передачи данных
  DMA1_Channel3->CCR &= ~(1 << DMA_CCR_EN_Pos);

  DMA1_Channel3->CPAR = (uint32_t)(&SPI1->DR); 		//заносим адрес регистра DR в CPAR
  DMA1_Channel3->CMAR = (uint32_t)data; 			//заносим адрес данных в регистр CMAR
  DMA1_Channel3->CNDTR = len; 						//количество передаваемых данных

  //Настройка канала DMA
  DMA1_Channel3->CCR = 0 << DMA_CCR_MEM2MEM_Pos 	//режим MEM2MEM отключен
    | 0x00 << DMA_CCR_PL_Pos 						//приоритет низкий
    | 0x00 << DMA_CCR_MSIZE_Pos 					//разрядность данных в памяти 8 бит
    | 0x00 << DMA_CCR_PSIZE_Pos 					//разрядность регистра данных 8 бит
    | 1 << DMA_CCR_MINC_Pos 						//Включить инкремент адреса памяти
    | 0 << DMA_CCR_PINC_Pos 						//Инкремент адреса периферии отключен
    | 0 << DMA_CCR_CIRC_Pos 						//кольцевой режим отключен
    | 1 << DMA_CCR_DIR_Pos;  						//1 - из памяти в периферию

  DMA1_Channel3->CCR |= 1 << DMA_CCR_EN_Pos; 		//включаем передачу данных
}

void SPI1_DMA_Receive(uint8_t reg, uint8_t *buf, uint16_t len)
{
  static uint8_t _filler = 0x00;
//  while (!(SPI1->SR & SPI_SR_TXE));
  GPIOA->BSRR = GPIO_BSRR_BS4;						//CS_HIGH

  //отключаем канал DMA после предыдущей передачи данных
  DMA1_Channel2->CCR &= ~(1 << DMA_CCR_EN_Pos);

  DMA1_Channel2->CPAR = (uint32_t)(&SPI1->DR); 		//заносим адрес регистра DR в CPAR
  DMA1_Channel2->CMAR = (uint32_t)buf; 				//заносим адрес данных в регистр CMAR
  DMA1_Channel2->CNDTR = len;						//количество передаваемых данных

  //Настройка канала DMA
  DMA1_Channel2->CCR = 0 << DMA_CCR_MEM2MEM_Pos 	//режим MEM2MEM отключен
    | 0x00 << DMA_CCR_PL_Pos 						//приоритет низкий
    | 0x00 << DMA_CCR_MSIZE_Pos 					//разрядность данных в памяти 8 бит
    | 0x00 << DMA_CCR_PSIZE_Pos 					//разрядность регистра данных 8 бит
    | 1 << DMA_CCR_MINC_Pos 						//Включить инкремент адреса памяти
    | 0 << DMA_CCR_PINC_Pos 						//Инкремент адреса периферии отключен
    | 0 << DMA_CCR_CIRC_Pos 						//кольцевой режим отключен
    | 0 << DMA_CCR_DIR_Pos;  						//0 - из периферии в память

  DMA1_Channel2->CCR |= 1 << DMA_CCR_EN_Pos; 		//включаем прием данных


  //////////////////////////////////////////////////////////////////////////////
  //нужно чтобы инициировать начало приема данных, иначе spi не работает
  //отключаем канал DMA после предыдущей передачи данных
  DMA1_Channel3->CCR &= ~(1 << DMA_CCR_EN_Pos);

  DMA1_Channel3->CPAR = (uint32_t)(&SPI1->DR); 		//заносим адрес регистра DR в CPAR
  DMA1_Channel3->CMAR = (uint32_t)(&_filler); 		//заносим адрес данных в регистр CMAR
  DMA1_Channel3->CNDTR = len; 						//количество передаваемых данных

  //Настройка канала DMA
  DMA1_Channel3->CCR = 0 << DMA_CCR_MEM2MEM_Pos 	//режим MEM2MEM отключен
    | 0x00 << DMA_CCR_PL_Pos 						//приоритет низкий
    | 0x00 << DMA_CCR_MSIZE_Pos 					//разрядность данных в памяти 8 бит
    | 0x00 << DMA_CCR_PSIZE_Pos 					//разрядность регистра данных 8 бит
    | 0 << DMA_CCR_MINC_Pos 						//Инкремент адреса памяти отключен
    | 0 << DMA_CCR_PINC_Pos 						//Инкремент адреса периферии отключен
    | 0 << DMA_CCR_CIRC_Pos 						//кольцевой режим отключен
    | 1 << DMA_CCR_DIR_Pos;  						//1 - из памяти в периферию

  	while (!(SPI1->SR & SPI_SR_TXE));
  	GPIOA->BSRR = GPIO_BSRR_BR4; //CS_LOW;
  	SPI1->DR = (reg | 0x80);
  	while ((SPI1->SR & SPI_SR_BSY));

  DMA1_Channel3->CCR |= 1 << DMA_CCR_EN_Pos; 		//Запускаем процесс
}


void SPI2_DMA_Send(uint8_t *data, uint16_t len)
{
  //отключаем канал DMA после предыдущей передачи данных
  DMA1_Channel5->CCR &= ~(1 << DMA_CCR_EN_Pos);

  DMA1_Channel5->CPAR = (uint32_t)(&SPI2->DR); 		//заносим адрес регистра DR в CPAR
  DMA1_Channel5->CMAR = (uint32_t)data; 			//заносим адрес данных в регистр CMAR
  DMA1_Channel5->CNDTR = len; 						//количество передаваемых данных

  //Настройка канала DMA
  DMA1_Channel5->CCR = 0 << DMA_CCR_MEM2MEM_Pos 	//режим MEM2MEM отключен
    | 0x00 << DMA_CCR_PL_Pos 						//приоритет низкий
    | 0x00 << DMA_CCR_MSIZE_Pos 					//разрядность данных в памяти 8 бит
    | 0x00 << DMA_CCR_PSIZE_Pos 					//разрядность регистра данных 8 бит
    | 1 << DMA_CCR_MINC_Pos 						//Включить инкремент адреса памяти
    | 0 << DMA_CCR_PINC_Pos 						//Инкремент адреса периферии отключен
    | 0 << DMA_CCR_CIRC_Pos 						//кольцевой режим отключен
    | 1 << DMA_CCR_DIR_Pos;  						//1 - из памяти в периферию

	  DMA1_Channel5->CCR |= 1 << DMA_CCR_EN_Pos; 	//включаем передачу данных
	}

void SPI2_DMA_Receive(uint8_t reg, uint8_t *buf, uint16_t len)
{
  static uint8_t _filler2 = 0x00;
//  while (!(SPI1->SR & SPI_SR_TXE));
  GPIOB->BSRR |= GPIO_BSRR_BS12;			//CS_HIGH

  	  	  	  	  	  	  	  	  	  	  	  	  	 //отключаем канал DMA после предыдущей передачи данных
  DMA1_Channel4->CCR &= ~(1 << DMA_CCR_EN_Pos);

  DMA1_Channel4->CPAR = (uint32_t)(&SPI2->DR); 		//заносим адрес регистра DR в CPAR
  DMA1_Channel4->CMAR = (uint32_t)buf; 				//заносим адрес данных в регистр CMAR
  DMA1_Channel4->CNDTR = len; 						//количество передаваемых данных

  //Настройка канала DMA
  DMA1_Channel4->CCR = 0 << DMA_CCR_MEM2MEM_Pos 	//режим MEM2MEM отключен
    | 0x00 << DMA_CCR_PL_Pos 						//приоритет низкий
    | 0x00 << DMA_CCR_MSIZE_Pos 					//разрядность данных в памяти 8 бит
    | 0x00 << DMA_CCR_PSIZE_Pos 					//разрядность регистра данных 8 бит
    | 1 << DMA_CCR_MINC_Pos 						//Включить инкремент адреса памяти
    | 0 << DMA_CCR_PINC_Pos 						//Инкремент адреса периферии отключен
    | 0 << DMA_CCR_CIRC_Pos 						//кольцевой режим отключен
    | 0 << DMA_CCR_DIR_Pos;  						//0 - из периферии в память

  DMA1_Channel4->CCR |= 1 << DMA_CCR_EN_Pos; 		//включаем прием данных


  //////////////////////////////////////////////////////////////////////////////
  //нужно чтобы инициировать начало приема данных, иначе spi не работает
  //отключаем канал DMA после предыдущей передачи данных
  DMA1_Channel5->CCR &= ~(1 << DMA_CCR_EN_Pos);

  DMA1_Channel5->CPAR = (uint32_t)(&SPI2->DR); 		//заносим адрес регистра DR в CPAR
  DMA1_Channel5->CMAR = (uint32_t)(&_filler2);		//заносим адрес данных в регистр CMAR
  DMA1_Channel5->CNDTR = len; 						//количество передаваемых данных

  //Настройка канала DMA
  DMA1_Channel5->CCR = 0 << DMA_CCR_MEM2MEM_Pos 	//режим MEM2MEM отключен
    | 0x00 << DMA_CCR_PL_Pos 						//приоритет низкий
    | 0x00 << DMA_CCR_MSIZE_Pos 					//разрядность данных в памяти 8 бит
    | 0x00 << DMA_CCR_PSIZE_Pos 					//разрядность регистра данных 8 бит
    | 0 << DMA_CCR_MINC_Pos 						//Инкремент адреса памяти отключен
    | 0 << DMA_CCR_PINC_Pos 						//Инкремент адреса периферии отключен
    | 0 << DMA_CCR_CIRC_Pos 						//кольцевой режим отключен
    | 1 << DMA_CCR_DIR_Pos;  						//1 - из памяти в периферию

  	while (!(SPI2->SR & SPI_SR_TXE));
  	GPIOB->BSRR |= GPIO_BSRR_BR12; 					//CS_LOW;
  	SPI2->DR = (reg | 0x80);
  	while ((SPI2->SR & SPI_SR_BSY));

  DMA1_Channel5->CCR |= 1 << DMA_CCR_EN_Pos; 		//Запускаем процесс
}
