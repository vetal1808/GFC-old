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
#include "quadcopter_config.h"

void setup();


int main()
{
	setup();
	//
	const vector4 offset_q = {0.99823046, 0.0026365665, 0.012465559, 0.0};
	vector3 accel, gyro;
	rotor4 rotor4_thrust;
	int16_t average_thrust = 0.0f;
	start_synchronization();
	while(1)
	{
		synchronous_delay(update_period_in_us);

		MPU6050_getFloatMotion6(&accel, &gyro);

		MadgwickAHRSupdateIMU(gyro.x, gyro.y, gyro.z, accel.x, accel.y, accel.z);

		vector4 RC_quaternion;
		vector3 RC_gyro_shift; //
		
		rx_update();

		update_PID_config();
		get_RC_state(&RC_quaternion, &RC_gyro_shift, &average_thrust);
		gyro = vector3_sub(gyro, RC_gyro_shift);
		
		vector4 real_quaternion = quaterns_multiplication(GetMadgwickAHRSQuaternion(), offset_q); //get quaternion to hotizont plane
		vector4 need_quaternion = quaterns_multiplication(RC_quaternion, real_quaternion);//get quaternion to needed plane
		
		//experement for global positioning
		BMP085_update();
		update_altitude(accel,real_quaternion);

		

		//end of place for exp
		stab_algorithm(need_quaternion, gyro, &rotor4_thrust, average_thrust);

		int16_t tmp;
		get_rx_buffer(&tmp, MOTOR_MASK, 1);
		update_rotors(rotor4_thrust, tmp);

		//loading telemetry data
		load_stab_algorithm_telemetry(); //PID summand
		load_euclid_angle_telemetry(&need_quaternion); //angles to needed plane
			
		
		tmp = (int16_t)(loop_time());
		load_tx_buffer(&tmp, LOOP_TIME, 1);
		tmp = (int16_t)BMP085_get_altitude();
		load_tx_buffer(&tmp, ALTITUDE, 1);
		
		tx_update();
	}

}

void setup(){
	init_timer();
	set_USARTn(USART3);
	I2C_LowLevel_Init(I2C1);
	MPU6050_initialize();
	MPU6050_setDLPFMode(0x02);
	MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_1000);
	MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
	MPU6050_setSampleRateDiv(4);
	delay_us(1000000);
	MPU6050_calibration(3000); //accumulation gyroscope offset

	BMP085_begin(BMP085_ULTRAHIGHRES);
	BMP085_set_zero_pressure(BMP085_meagure_press());
	set_tx_mask(0xFF);
	ESC_init();
}
