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
#include "telemetry.h"
#include "ESC_control.h"
void setup();


uint32_t sync_time;

int main()
{
	setup();
	//
	const vector4 offset_q = {0.99823046, 0.0026365665, 0.012465559, 0.0};
	vector3 accel, gyro;
	rotor4 rotor4_thrust;
	float average_thrust = 0.0f;	
	sync_time = micros();
	while(1)
	{
		while(micros()-sync_time<update_period_in_us);
		sync_time += update_period_in_us;

		MPU6050_getFloatMotion6(&accel, &gyro);

		MadgwickAHRSupdateIMU(gyro.x, gyro.y, gyro.z, accel.x, accel.y, accel.z);

		vector4 RC_quaternion;
		rx_update(&RC_quaternion, &average_thrust);
		vector4 real_quaternion = quaterns_multiplication(q, offset_q);
		vector4 result_quaternion = quaterns_multiplication(RC_quaternion, real_quaternion);
	
		stab_algorithm(result_quaternion, gyro, &rotor4_thrust, average_thrust);

		update_rotors(rotor4_thrust, 0x0F);

		tx_update(result_quaternion, micros()-sync_time);

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
	MPU6050_setSampleRateDiv(4);
	MPU6050_calibration(3000); //accumulation gyroscope offset

//	BMP085_begin(BMP085_ULTRAHIGHRES);
//	BMP085_set_zero_pressure(BMP085_meagure_press());
	set_tx_mask(0xFF);
	ESC_init();
}
