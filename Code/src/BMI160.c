/****************************************************Library***************************************************/

#include <BMI160.h>
#include <math.h>


extern int8_t AccXoff;
extern int8_t AccYoff;
extern int8_t AccZoff;

extern int16_t GyrXoff;
extern int16_t GyrYoff;
extern int16_t GyrZoff;

#define		 CS_LOW 	GPIOA->BSRR = GPIO_BSRR_BR4
#define 	 CS_HIGH 	GPIOA->BSRR = GPIO_BSRR_BS4


/******************************************************Defines*************************************************/
void BMI160_Init(void)
{

	BMI160_Read8(0x7F);
	Sdelay(1);
	BMI160_Write8(BMI160_CMD, BMI160_CMD_SOFT_RESET);
	Sdelay(1);

	BMI160_Write8(BMI160_CMD, BMI160_CMD_ACC_SET_PMU_MODE_NORMAL);
	Sdelay(20);
	BMI160_Write8(BMI160_CMD, BMI160_CMD_ACC_SET_PMU_MODE_NORMAL);
	Sdelay(20);

	BMI160_Write8(BMI160_CMD, BMI160_CMD_GYRO_SET_PMU_MODE_NORMAL);
	Sdelay(20);


	BMI160_Write8(BMI160_ACC_RANGE, BMI160_ACC_RANGE_ACC_RANGE_2G);
	Sdelay(10);
	BMI160_Write8(BMI160_ACC_CONF, 0x9);
	Sdelay(10);
	BMI160_Write8(BMI160_GYR_RANGE, BMI160_GYR_RANGE_GYR_RANGE_500);
	Sdelay(10);

	BMI160_Acc_Offset (&AccXoff, &AccYoff, &AccZoff);
	BMI160_Gyro_Offset(&GyrXoff, &GyrYoff, &GyrZoff);

}

void BMI160_SelfTest(void)
{
	BMI160_Read8(0x7F);
	Sdelay(1);
	BMI160_Write8(BMI160_CMD, BMI160_CMD_SOFT_RESET);
	Sdelay(1);

	BMI160_Write8(BMI160_CMD, BMI160_CMD_ACC_SET_PMU_MODE_NORMAL);
	Sdelay(20);
	BMI160_Write8(BMI160_CMD, BMI160_CMD_ACC_SET_PMU_MODE_NORMAL);
	Sdelay(20);

	BMI160_Write8(BMI160_CMD, BMI160_CMD_GYRO_SET_PMU_MODE_NORMAL);
	Sdelay(20);

	BMI160_Write8(BMI160_ACC_RANGE, BMI160_ACC_RANGE_ACC_RANGE_8G);
	Sdelay(10);

	BMI160_Write8(BMI160_ACC_CONF, 0x2C);

}
void BMI160_Acc_Raw(int16_t* Xraw, int16_t* Yraw, int16_t* Zraw)
{

	*Xraw = ((((int16_t)BMI160_Read8(BMI160_DATA_ACC_X_MSB) << 8) | (BMI160_Read8(BMI160_DATA_ACC_X_LSB))) + (int16_t)(64 * AccXoff)); // 0.0039g * 16384(for 2g) = 63.89
	*Yraw = ((((int16_t)BMI160_Read8(BMI160_DATA_ACC_Y_MSB) << 8) | (BMI160_Read8(BMI160_DATA_ACC_Y_LSB))) + (int16_t)(64 * AccYoff));
	*Zraw = ((((int16_t)BMI160_Read8(BMI160_DATA_ACC_Z_MSB) << 8) | (BMI160_Read8(BMI160_DATA_ACC_Z_LSB))) + (int16_t)(64 * AccZoff));

}

void BMI160_Gyr_Raw(int16_t* Xraw, int16_t* Yraw, int16_t* Zraw)
{

	*Xraw = ((((int16_t)BMI160_Read8(BMI160_DATA_GYRO_X_MSB) << 8) | (BMI160_Read8(BMI160_DATA_GYRO_X_LSB))));
	*Yraw = ((((int16_t)BMI160_Read8(BMI160_DATA_GYRO_Y_MSB) << 8) | (BMI160_Read8(BMI160_DATA_GYRO_Y_LSB))));
	*Zraw = ((((int16_t)BMI160_Read8(BMI160_DATA_GYRO_Z_MSB) << 8) | (BMI160_Read8(BMI160_DATA_GYRO_Z_LSB))));

}

void BMI160_Gyr_deg_sek(float* gXd, float* gYd, float* gZd)
{
	int16_t gX = 0;
	int16_t gY = 0;
	int16_t gZ = 0;

	BMI160_Gyr_Raw(&gX, &gY, &gZ);

	*gXd = (gX + (0.061 * GyrXoff)) / 65.6;		// 0.061 (deg/s) * gx
	*gYd = (gY + (0.061 * GyrYoff)) / 65.6;
	*gZd = (gZ + (0.061 * GyrZoff)) / 65.6;
}

void BMI160_BMM150_Raw(int16_t* Xraw, int16_t* Yraw, int16_t* Zraw)
{

	*Xraw = (((int16_t)BMI160_Read8(BMI160_DATA_MAG_X_MSB) << 8) | (BMI160_Read8(BMI160_DATA_MAG_X_LSB)));
	*Yraw = (((int16_t)BMI160_Read8(BMI160_DATA_MAG_Y_MSB) << 8) | (BMI160_Read8(BMI160_DATA_MAG_Y_LSB)));
	*Zraw = (((int16_t)BMI160_Read8(BMI160_DATA_MAG_Z_MSB) << 8) | (BMI160_Read8(BMI160_DATA_MAG_Z_LSB)));

}

void BMI160_Roll_Pitch(float* roll, float* pitch)
{
	int16_t Xraw = 0;
	int16_t Yraw = 0;
	int16_t Zraw = 0;

	BMI160_Acc_Raw(&Xraw, &Yraw, &Zraw);

	*roll  = atan( Yraw       / (sqrt((Xraw * Xraw) + (Zraw * Zraw)))) * 57.2958;
	*pitch = atan((-1 * Xraw) / (sqrt((Yraw * Yraw) + (Zraw * Zraw)))) * 57.2958;
}

void BMI160_Acc_Offset(int8_t* Xoff, int8_t* Yoff, int8_t* Zoff)
{

	*Xoff = BMI160_Read8int(BMI160_OFFSET_0);
	*Yoff = BMI160_Read8int(BMI160_OFFSET_1);
	*Zoff = BMI160_Read8int(BMI160_OFFSET_2);

}

void BMI160_Gyro_Offset(int16_t* Xoff, int16_t* Yoff, int16_t* Zoff)
{

	*Xoff = (int16_t)BMI160_Read8int(BMI160_OFFSET_3) | ((BMI160_Read8int(BMI160_OFFSET_6) << 6) >> 6);
	*Yoff = (int16_t)BMI160_Read8int(BMI160_OFFSET_4) | ((BMI160_Read8int(BMI160_OFFSET_6) << 4) >> 6);
	*Zoff = (int16_t)BMI160_Read8int(BMI160_OFFSET_5) | ((BMI160_Read8int(BMI160_OFFSET_6) << 2) >> 6);

}

uint32_t BMI160_Time(void)
{

	uint32_t time = 0;
	time = (((uint32_t)BMI160_Read8(BMI160_SENSORTIME_2) << 16) | ((uint32_t)BMI160_Read8(BMI160_SENSORTIME_1) << 8) | (BMI160_Read8(BMI160_SENSORTIME_0)));
	return time;

}

uint8_t BMI160_Read8(uint8_t reg)
{

	uint8_t data = 0;

	while (!(SPI1->SR & SPI_SR_TXE));
	CS_LOW;
	SPI1->DR = (reg | 0x80);

	while (!(SPI1->SR & SPI_SR_RXNE));
	SPI1->DR;	//фиктивное чтение,по идее не нужно, но без него не работает

	while (!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = 0x00;

	while (!(SPI1->SR & SPI_SR_RXNE));
	SPI1->DR;	//фиктивное чтение,по идее не нужно, но без него не работает

	while (!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = 0x00;

	while (!(SPI1->SR & SPI_SR_RXNE));
	data = SPI1->DR;


	while ((SPI1->SR & SPI_SR_BSY));
	CS_HIGH;

	return data;

}

int8_t BMI160_Read8int(uint8_t reg)
{
	int8_t data = 0;

	while (!(SPI1->SR & SPI_SR_TXE));
	CS_LOW;
	SPI1->DR = (reg | 0x80);

	while (!(SPI1->SR & SPI_SR_RXNE));
	SPI1->DR;	//фиктивное чтение,по идее не нужно, но без него не работает

	while (!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = 0x00;

	while (!(SPI1->SR & SPI_SR_RXNE));
	SPI1->DR;	//фиктивное чтение,по идее не нужно, но без него не работает

	while (!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = 0x00;

	while (!(SPI1->SR & SPI_SR_RXNE));
	data = SPI1->DR;


	while ((SPI1->SR & SPI_SR_BSY));
	CS_HIGH;

	return data;
}


uint8_t BMI160_Write8(uint8_t reg, uint8_t value)
{

	while (!(SPI1->SR & SPI_SR_TXE));
	CS_LOW;
	SPI1->DR = reg;

	while (!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = value;

	while ((SPI1->SR & SPI_SR_BSY));
	CS_HIGH;

	return 1;
}


uint8_t BMI160_getMagGyroAcc(int16_t* Xmag, int16_t* Ymag, int16_t* Zmag, int16_t* Xgyr, int16_t* Ygyr, int16_t* Zgyr, int16_t* Xacc, int16_t* Yacc, int16_t* Zacc)
{

	int16_t buffMagGyroAcc[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	while (!(SPI1->SR & SPI_SR_TXE));
	CS_LOW;
	SPI1->DR = ((uint16_t)(BMI160_DATA_MAG_X_LSB << 8) | 0x8000);

	while (!(SPI1->SR & SPI_SR_RXNE));
	SPI1->DR;	//фиктивное чтение

	while (!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = 0x00;

	for (int i = 0; i <= 10; i ++){

		while (!(SPI1->SR & SPI_SR_RXNE));
		buffMagGyroAcc[i] = SPI1->DR;

		while (!(SPI1->SR & SPI_SR_TXE));
		SPI1->DR = 0x00;
	}
	while ((SPI1->SR & SPI_SR_BSY));
	CS_HIGH;

	*Xmag = buffMagGyroAcc[0];
	*Ymag = buffMagGyroAcc[1];
	*Zmag = buffMagGyroAcc[2];

	*Xgyr = buffMagGyroAcc[3];
	*Ygyr = buffMagGyroAcc[4];
	*Zgyr = buffMagGyroAcc[5];

	*Xacc = buffMagGyroAcc[6];
	*Yacc = buffMagGyroAcc[7];
	*Zacc = buffMagGyroAcc[8];

	return 1;

}

uint8_t MSBtoLSB (uint8_t num)
{

	uint8_t y1 = 0;
	uint8_t y2 = 0;

	uint8_t z1 = 0;
	uint8_t z2 = 0;
	uint8_t z3 = 0;
	uint8_t z4 = 0;

	uint8_t q1 = 0;
	uint8_t q2 = 0;
	uint8_t q3 = 0;
	uint8_t q4 = 0;
	uint8_t q5 = 0;
	uint8_t q6 = 0;
	uint8_t q7 = 0;
	uint8_t q8 = 0;

	uint8_t result = 0;

	y1 = num << 4;
	z1 = (y1 << 2) & 0b11000000;
	z2 = (y1 >> 2) & 0b00110000;

	y2 = num >> 4;
	z3 = (y2 << 2) & 0b00001100;
	z4 = (y2 >> 2) & 0b00000011;

	q1 = (z1 << 1) & 0b10000000;
	q2 = (z1 >> 1) & 0b01000000;
	q3 = (z2 << 1) & 0b00100000;
	q4 = (z2 >> 1) & 0b00010000;

	q5 = (z3 << 1) & 0b00001000;
	q6 = (z3 >> 1) & 0b00000100;
	q7 = (z4 << 1) & 0b00000010;
	q8 = (z4 >> 1) & 0b00000001;

	result = q1 | q2  | q3  | q4 | q5 | q6  | q7  | q8;

	return result;
}

void Sdelay(uint32_t time)
{ // временная мера потом установим freeRTOS
	uint32_t i;
	time = time * 48000;
	for (i = 0; i < time; i++)
	{
	}
}
