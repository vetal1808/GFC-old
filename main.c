#include "stm32f10x.h"
#include "stdint.h"
#include "easy_uart.h"
#include "mpu6050.h"
#include "MadgwickAHRS.h"
#include "timer.h"
#include "BMP085_timer.h"
#include "gps_parser.h"
#include "math.h"
#include "helpers.h"
#include "stab_alg.h"

#define period 20000
#define period_in_sec 0.020
#define gyro1000_to_radians 0.00052083333
#define int2G_to_float 0.059814453125 // convert 16bit int data 2G sens to sm/sec^2 in float

int main()
{

	setup();

	vector3 accel, gyro;
	rotor4 rotor4_thrust;
	float average_thrust = 200.f;

	uint32_t sync_time = micros();

	while(1)
	{
		while(micros()-sync_time<period);
		sync_time += period;
		MPU6050_getFloatMotion6(&accel, &gyro);

		int16_t tmp[6];
		tmp[0] = accel.x*16.0;
		tmp[1] = accel.y*16.0;
		tmp[2] = accel.z*16.0;
		tmp[3] = gyro.x*2000.0;
		tmp[4] = gyro.y*2000.0;
		tmp[5] = gyro.z*2000.0;
//		load_tx_buffer(tmp);
//		transmit_masked_channal();
		receive_all_available();
		/*
		MadgwickAHRSupdateIMU(gyro.x, gyro.y, gyro.z, accel.x, accel.y, accel.z);

		vector3 tmp = quaternion_decomposition(q);

		stab_algorithm(q, gyro, &rotor4_thrust, average_thrust);

*/
	}

}




void setup(){
	init_timer();
	set_USARTn(USART3);
	I2C_LowLevel_Init(I2C1);
	MPU6050_initialize();
	MPU6050_setDLPFMode(0x03);
	MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_1000);
	MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
	MPU6050_setSampleRateDiv(2);

	BMP085_begin(BMP085_ULTRAHIGHRES);
	BMP085_set_zero_pressure(BMP085_meagure_press());
	set_mask(0xFF);
}


