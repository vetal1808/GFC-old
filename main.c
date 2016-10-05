#include "stm32f10x.h"
#include "stdint.h"
#include "easy_uart.h"
#include "mpu6050.h"
#include "MadgwickAHRS.h"
#include "timer.h"
#include "BMP085_timer.h"
#include "gps_parser.h"

#define period 4000
#define gyro1000_to_radians 1920.0


int main()
{




	USART_init(USART1);
	init_timer();
	I2C_LowLevel_Init(I2C1);
	BMP085_begin(BMP085_ULTRAHIGHRES);

	uint32_t start = micros();
	int32_t alt = 0;
	BMP085_set_zero_pressure(BMP085_meagure_press());
	uint8_t skip = 0;
	while(1)
	{
		BMP085_update();
		alt = BMP085_get_altitude();

		if(skip == 6){
			send(alt, 'A');
			skip = 0;
		}
		else {
			skip++;
		}

		while(micros()-start < period);
		start += period;
	}



	/*
	init_timer();
	USART_init(USART1);
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
		send(pitch, 'A');
		send(roll, 'B');
		send(yaw, 'C');
		send((micros()-sync_time), 'M');
		USART_send(USART1,"W\n",2);


	}
	*/
}
void send(int32_t val, char symb)
{
	uint8_t buf[9];
	itoa(val, buf,10);

	uint8_t l = 0;
	while (buf[l] != 0 && l < 9)
		l++;
	USART_send(USART1,&symb,1);
	USART_send(USART1,buf,l);
	USART_send(USART1,"\n",1);
}
