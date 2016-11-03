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

#define period 10000
#define period_in_sec 0.010
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

		MadgwickAHRSupdateIMU(gyro.x, gyro.y, gyro.z, accel.x, accel.y, accel.z);

		vector3 tmp = quaternion_decomposition(q);

		stab_algorithm(q, gyro, &rotor4_thrust, average_thrust);

		send((int16_t)(rotor4_thrust.RFC*50.0),'A');
		send((int16_t)(rotor4_thrust.LFW*50.0),'B');
		send((int16_t)(rotor4_thrust.RBW*50.0),'C');
		send((int16_t)(rotor4_thrust.LBC*50.0),'G');
		send(0,'W');

	}

}




void setup(){
	init_timer();
	USART_init(USART3, 921600);
	I2C_LowLevel_Init(I2C1);
	MPU6050_initialize();
	MPU6050_setDLPFMode(0x03);
	MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_1000);
	MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
	MPU6050_setSampleRateDiv(2);

	BMP085_begin(BMP085_ULTRAHIGHRES);
	BMP085_set_zero_pressure(BMP085_meagure_press());
}

void send(int32_t val, char symb)
{
	uint8_t buf[9];
	itoa(val, buf,10);

	uint8_t l = 0;
	while (buf[l] != 0 && l < 9)
		l++;
	USART_send(USART3,&symb,1);
	USART_send(USART3,buf,l);
	USART_send(USART3,"\n",1);
}
