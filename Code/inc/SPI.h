/******************************************************Library******************************************************/

#include "stm32f1xx.h"

/******************************************************Use Function*************************************************/
void SPI1_Init(void);
void SPI1_DMA_Receive(uint8_t reg, uint8_t *buf, uint16_t len);
void SPI1_DMA_Send(uint8_t *data, uint16_t len);

void SPI2_Init(void);
void SPI2_DMA_Receive(uint8_t reg, uint8_t *buf, uint16_t len);
void SPI2_DMA_Send(uint8_t *data, uint16_t len);
