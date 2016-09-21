

#include "stm32f10x.h"
#include "stdint.h"
#include "easy_uart.h"
#include "mpu6050.h"
#include "MadgwickAHRS.h"
#include "timer.h"
#include "BMP085_timer.h"


#define period 4000
#define gyro1000_to_radians 1920.0

void clear(char * b)
{
	uint8_t i;
	for(i=0;i<6;i++)
		b[i] = 0;
}
int main()
{
	init_timer();
	usart_init();
	I2C_LowLevel_Init(I2C1);
	MPU6050_initialize();
	MPU6050_setDLPFMode(0x03);
	MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_1000);
	MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
	MPU6050_setSampleRateDiv(3);

	int16_t ax = 0, ay = 0, az = 0;
	int16_t gx = 0, gy = 0, gz = 0;
	int16_t pitch, roll, yaw;
	uint8_t buff[6];
	uint32_t sync_time = micros();

	while(1)
	{
		while(micros()-sync_time<period);
		sync_time = micros();
		MPU6050_getMotion6(&ax, &ay, &az, &gx, &gy, &gz, 0);
		MadgwickAHRSupdateIMU((float)gx/gyro1000_to_radians, (float)gy/gyro1000_to_radians, (float)(gz)/(gyro1000_to_radians), ax, ay, az);
		get_euclid(&pitch, &roll, &yaw);
/*
		itoa(ax, buff,10);
		UARTSend("A",1);
		UARTSend(buff,6);
		UARTSend("\n",1);
		clear(buff);

		itoa(ay, buff,10);
		UARTSend("B",1);
		UARTSend(buff,6);
		UARTSend("\n",1);
		clear(buff);

		itoa(az, buff,10);
		UARTSend("C",1);
		UARTSend(buff,6);
		UARTSend("\n",1);
		clear(buff);

		itoa(gx, buff,10);
		UARTSend("D",1);
		UARTSend(buff,6);
		UARTSend("\n",1);
		clear(buff);

		itoa(gy, buff,10);
		UARTSend("E",1);
		UARTSend(buff,6);
		UARTSend("\n",1);
		clear(buff);

		itoa(gz, buff,10);
		UARTSend("F",1);
		UARTSend(buff,6);
		UARTSend("\n",1);
		clear(buff);
 	 	 */
		itoa(pitch, buff,10);
		UARTSend("G",1);
		UARTSend(buff,6);
		UARTSend("\n",1);
		clear(buff);

		itoa(roll, buff,10);
		UARTSend("H",1);
		UARTSend(buff,6);
		UARTSend("\n",1);
		clear(buff);

		itoa(yaw, buff,10);
		UARTSend("I",1);
		UARTSend(buff,6);
		UARTSend("\nW\n",3);
		clear(buff);
	}
}

