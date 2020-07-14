#include "main.h"

/*------------BMI160----------*/

int16_t AccXraw = 0;	// Raw value acc (-32000 lbs; 32000 lbs)
int16_t AccYraw = 0;
int16_t AccZraw = 0;

float AccX = 0;			// Raw value acc to gravity (-2g; 2g)
float AccY = 0;
float AccZ = 0;

float AccXf = 0;		// Filter value acc
float AccYf = 0;
float AccZf = 0;

int8_t AccXoff = 0;		// Offset value acc
int8_t AccYoff = 0;
int8_t AccZoff = 0;

int16_t GyrXraw = 0;	// Raw value gyro (-131 lbs; 131 lbs)
int16_t GyrYraw = 0;
int16_t GyrZraw = 0;

float GyrX = 0;			// Raw value gyro to speed (-250 deg/s; 250 deg/s)
float GyrY = 0;
float GyrZ = 0;

float GyrXf = 0;		// Filter value gyro
float GyrYf = 0;
float GyrZf = 0;

int16_t GyrXoff = 0;	// Offset value gyro
int16_t GyrYoff = 0;
int16_t GyrZoff = 0;

uint8_t rxGyroAcc[15];

uint32_t timeBMI160 = 0;

/*------------BMM150----------*/

int16_t MagX = 0;
int16_t MagY = 0;
int16_t MagZ = 0;

int16_t rhall = 0;

float azimuthRaw = 0;
float azimuth = 0;

uint8_t rxMag[8];

/*-----------END-BMM150----------*/

/*-----------Angles----------*/

float gyroRoll = 0;
float gyroPitch = 0;
float gyroYaw = 0;

float AccelRoll = 0;
float AccelPitch = 0;

float RollRaw = 0;
float PitchRaw = 0;
float YawRaw = 0;

const float rad = 0.01745329251994329576923690768489;
const float deg = 57.295779513082320876798154814105;
float radians = 0;

/*-----------END-Angles----------*/

/*-----------Time----------*/

volatile uint32_t ticks_task1 = 0;
volatile uint32_t ticks_task2 = 0;

float elapsedTime = 0.0025;

/*-----------END-Time----------*/

int main(void) {

	RCC_Init();
	GPIO_Init();
	UART3_Init();
	SPI1_Init();
	SPI2_Init();
	BMI160_Init();
	BMM150_Init();
	SysTick_Init();

	UART3_Send_String("Start program\r\n");

	while (1) {

		if (ticks_task1 > 250) {		//250 = 2500 us = 400 Hz
			ticks_task1 = 0;

			//==============================================Read BMI160 and BMM150==============================================//

			SPI1_DMA_Receive(BMI160_DATA_GYRO_X_LSB, rxGyroAcc, 15);
			SPI2_DMA_Receive(0x42, rxMag, 8);

			GyrXraw = (((int16_t) rxGyroAcc[3] << 8) | (rxGyroAcc[2]));
			GyrYraw = (((int16_t) rxGyroAcc[5] << 8) | (rxGyroAcc[4]));
			GyrZraw = (((int16_t) rxGyroAcc[7] << 8) | (rxGyroAcc[6]));

			GyrX = ((float) GyrXraw + (0.061 * (float) GyrXoff)) / 131.0;
			GyrY = ((float) GyrYraw + (0.061 * (float) GyrYoff)) / 131.0;
			GyrZ = ((float) GyrZraw + (0.061 * (float) GyrZoff)) / 131.0;

			AccXraw = (((int16_t) rxGyroAcc[9] << 8) | (rxGyroAcc[8]));	// 0.0039g * 16384(for 2g) = 63.89
			AccYraw = (((int16_t) rxGyroAcc[11] << 8) | (rxGyroAcc[10]));
			AccZraw = (((int16_t) rxGyroAcc[13] << 8) | (rxGyroAcc[12]));

//			AccX = ((float) AccXraw - (64.0 * AccXoff)) / 17039.0;
//			AccY = ((float) AccYraw - (64.0 * AccYoff)) / 17039.0;
//			AccZ = ((float) AccZraw - (64.0 * AccZoff)) / 17039.0;

			timeBMI160 = (((int32_t) rxGyroAcc[1] << 16) | ((int16_t) rxGyroAcc[0] << 8) | (rxGyroAcc[14]));	// внутреннее время датчика

			MagX = (((int16_t) rxMag[3] << 8) | (rxMag[2]));
			MagY = (((int16_t) rxMag[5] << 8) | (rxMag[4]));
			MagZ = (((int16_t) rxMag[7] << 8) | (rxMag[6]));

			rhall = (((int16_t) rxMag[1] << 8) | (rxMag[0]));			// датчик температуры на эффекте Холла

			//============================================END read BMI160 and BMM150============================================//

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Calibration~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

			//centering and scaling axis (value to g)
			AccX = (AccXraw - 93)  / 16607.0;		//Смещение и масштабирование осей
			AccY = (AccYraw - 126) / 16374.0;
			AccZ = (AccZraw - 560) / 16686.0;

			MagX = (MagX + 3)   * 0.237341772;
			MagY = (MagY + 131) * 0.229591837;
			MagZ = (MagZ + 102) * 1;

			//============================================Angles; Speed; Coordinates============================================//

			// low-pass filter
			AccXf = (0.95 * AccXf) + (0.05 * AccX);
			AccYf = (0.95 * AccYf) + (0.05 * AccY);
			AccZf = (0.95 * AccZf) + (0.05 * AccZ);

			// low-pass filter
			GyrXf = (0.95 * GyrXf) + (0.05 * GyrX);
			GyrYf = (0.95 * GyrYf) + (0.05 * GyrY);
			GyrZf = (0.95 * GyrZf) + (0.05 * GyrZ);

			//deg0 + ((deg / s) * s) = deg
			gyroRoll  = gyroRoll  + (GyrXf * elapsedTime);
			gyroPitch = gyroPitch + (GyrYf * elapsedTime);
			gyroYaw   = gyroYaw   + (GyrZf * elapsedTime);

			AccelRoll  = atan(AccY        / (sqrt((AccX * AccX) + (AccZ * AccZ)))) * deg;
			AccelPitch = atan((-1 * AccX) / (sqrt((AccY * AccY) + (AccZ * AccZ)))) * deg;

			RollRaw  = 0.9 * gyroRoll + 0.1 * AccelRoll;
			PitchRaw = 0.9 * gyroPitch + 0.1 * AccelPitch;

			float RollRawrad  = -RollRaw     * rad;
			float PitchRawrad = PitchRaw    * rad;

			azimuth= 11.7 + 180 + 57.295 * atan2((MagZ * sin(RollRawrad) - (MagY) * cos(RollRawrad)),
					((MagX) * cos(PitchRawrad) + (MagY) * sin(RollRawrad) * sin(PitchRawrad) + (MagZ) * sin(PitchRawrad) * cos(RollRawrad)));

			YawRaw   = 0.99 * gyroYaw + 0.01 * azimuth;

			// Feedback for gyro
			gyroRoll  = RollRaw;
			gyroPitch = PitchRaw;
			gyroYaw   = YawRaw;

		}

		if (ticks_task2 > 3000) {		// 300000 * 10 us = 0.3 Hz
			ticks_task2 = 0;

			//==============================================Print==============================================//


			//--------Angles--------//
			/*
			UART3_Send_Number_Float(gyroRoll);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(gyroPitch);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(gyroYaw);
			UART3_Send_String("\t\t");


			UART3_Send_Number_Float(AccelRoll);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(AccelPitch);
			UART3_Send_String("\r\n");
			*/

			UART3_Send_Number_Float(RollRaw);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(PitchRaw);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(YawRaw);
			UART3_Send_String("\r\n");


			//--------Compass--------//
			/*
			UART3_Send_Number_Float(YawRaw);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(azimuth);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(azimuthRaw);
			UART3_Send_String("\r\n");
			*/
			/*
			UART3_Send_Number_Float(MagX);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(MagY);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(MagZ);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(rhall);
			UART3_Send_String("\r\n");
			*/

			//------------Acc----------//
			/*
			UART3_Send_Number_Float(AccX);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(AccY);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(AccZ);
			UART3_Send_String("\t\t");


			UART3_Send_Number_Float(AccXrec);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(AccYrec);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(AccZrec);
			UART3_Send_String("\r\n");
			*/
			/*
			UART3_Send_Number_Float(VectorAcc);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(deltaVectorAcc);
			UART3_Send_String("\t");

			UART3_Send_Number_Float(wayX);
			UART3_Send_String("\r\n");
			*/

			/*
			//-----------Gyro----------//
			UART3_Send_Number_Float(GyrX);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(GyrY);
			UART3_Send_String("\t");
			UART3_Send_Number_Float(GyrZ);
			UART3_Send_String("\t\t");
			UART3_Send_String("\r\n");		// переход на новую строку
			*/

		}

	}
}

/*************************************IRQ***************************************/

void SysTick_Handler(void) {
	ticks_task1++;
	ticks_task2++;
}

