/****************************************************Library***************************************************/

#include <BMM150.h>
#include <BMI160.h>

/******************************************************Defines*************************************************/

#define		 CS12_LOW 	GPIOB->BSRR = GPIO_BSRR_BR12
#define 	 CS12_HIGH 	GPIOB->BSRR = GPIO_BSRR_BS12

void BMM150_Init(void)
{

	BMM150_Write8(BMM150_POWER_CONTROL_ADDR, 0x01);
	Sdelay(20);
	BMM150_Write8(BMM150_POWER_CONTROL_ADDR, 0x00);
	Sdelay(20);
	BMM150_Write8(BMM150_POWER_CONTROL_ADDR, 0x01);
	Sdelay(20);
	BMM150_Read8(BMM150_CHIP_ID_ADDR);
	Sdelay(20);
	BMM150_Write8(BMM150_OP_MODE_ADDR, 0x00);
	Sdelay(20);

}

uint8_t BMM150_Read8(uint8_t reg)
{

	uint8_t data = 0;

	while (!(SPI2->SR & SPI_SR_TXE));
	CS12_LOW;
	SPI2->DR = (reg | 0x80);

	while (!(SPI2->SR & SPI_SR_RXNE));
	SPI2->DR;	//фиктивное чтение,по идее не нужно, но без него не работает

	while (!(SPI2->SR & SPI_SR_TXE));
	SPI2->DR = 0x00;

	while (!(SPI2->SR & SPI_SR_RXNE));
	SPI2->DR;	//фиктивное чтение,по идее не нужно, но без него не работает

	while (!(SPI2->SR & SPI_SR_TXE));
	SPI2->DR = 0x00;

	while (!(SPI2->SR & SPI_SR_RXNE));
	data = SPI2->DR;


	while ((SPI2->SR & SPI_SR_BSY));
	CS12_HIGH;

	return data;

}

int8_t BMM150_Read8int(uint8_t reg)
{
	int8_t data = 0;

	while (!(SPI2->SR & SPI_SR_TXE));
	CS12_LOW;
	SPI2->DR = (reg | 0x80);

	while (!(SPI2->SR & SPI_SR_RXNE));
	SPI2->DR;	//фиктивное чтение,по идее не нужно, но без него не работает

	while (!(SPI2->SR & SPI_SR_TXE));
	SPI2->DR = 0x00;

	while (!(SPI2->SR & SPI_SR_RXNE));
	SPI2->DR;	//фиктивное чтение,по идее не нужно, но без него не работает

	while (!(SPI2->SR & SPI_SR_TXE));
	SPI2->DR = 0x00;

	while (!(SPI2->SR & SPI_SR_RXNE));
	data = SPI2->DR;


	while ((SPI2->SR & SPI_SR_BSY));
	CS12_HIGH;

	return data;
}


uint8_t BMM150_Write8(uint8_t reg, uint8_t value)
{

	while (!(SPI2->SR & SPI_SR_TXE));
	CS12_LOW;
	SPI2->DR = reg;

	while (!(SPI2->SR & SPI_SR_TXE));
	SPI2->DR = value;

	while ((SPI2->SR & SPI_SR_BSY));
	CS12_HIGH;

	return 1;
}
