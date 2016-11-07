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

#define period 20000
#define period_in_sec 0.020
#define gyro1000_to_radians 0.00052083333
#define int2G_to_float 0.059814453125 // convert 16bit int data 2G sens to sm/sec^2 in float

uint32_t sync_time;

int main()
{
	setup();
	//
	vector3 accel, gyro;
	rotor4 rotor4_thrust;
	float average_thrust = 200.f;
	int16_t tmp_array[14];
	
	sync_time = micros();//
	while(1)
	{
		while(micros()-sync_time<period);
		sync_time += period;

		MPU6050_getFloatMotion6(&accel, &gyro);

		MadgwickAHRSupdateIMU(gyro.x, gyro.y, gyro.z, accel.x, accel.y, accel.z);

		vector4 RC_quaternion;
		decode_rx_data(tmp_array, &RC_quaternion, &average_thrust);

		vector4 result_quaternion = quaterns_multiplication(RC_quaternion, q);
	
		stab_algorithm(result_quaternion, gyro, &rotor4_thrust, average_thrust);

		update_rotors(rotor4_thrust);

		collect_tx_data(tmp_array);
		load_tx_buffer(tmp_array);
		transmit_masked_channal();

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
	MPU6050_calibration();

	BMP085_begin(BMP085_ULTRAHIGHRES);
	BMP085_set_zero_pressure(BMP085_meagure_press());
	set_tx_mask(0xFF);
	ESC_init();
}
void collect_tx_data(int16_t * return_array){
	get_euclid(&return_array[0], &return_array[1], &return_array[2]);
	return_array[0]*=6;
	return_array[1]*=6;
	return_array[2]*=3;
	return_array[3] = (int16_t)(Ox.x*200.0);
	return_array[4] = (int16_t)(Ox.y*200.0);
	return_array[5] = (int16_t)(Ox.z*200.0);
	return_array[6] = (int16_t)(Oy.x*200.0);
	return_array[7] = (int16_t)(Oy.y*200.0);
	return_array[8] = (int16_t)(Oy.z*200.0);
	return_array[9] = (int16_t)(Oz.x*200.0);
	return_array[10] = (int16_t)(Oz.y*200.0);
	return_array[11] = (int16_t)(Oz.z*200.0);
	return_array[12] = micros()-sync_time;
}
void decode_rx_data(int16_t * rx_array, vector4 * RC_quaternion, float * avarage_thrust){
	receive_all_available();
	get_rx_buffer(rx_array);
	vector3 RC_angles;
	RC_angles.x = rx_array[0]*0.00174532925199432957692369076849;
	RC_angles.y = rx_array[1]*0.00174532925199432957692369076849;
	RC_angles.z = 0.0;
	*RC_quaternion = euclid_to_quaterion(RC_angles);
	set_P_gain((float)(rx_array[3])*2.0);
	set_I_gain((float)(rx_array[4])*2.0);
	set_D_gain((float)(rx_array[5]/2.0));
	set_P_limit((float)(rx_array[6]));
	set_I_limit((float)(rx_array[7]));
	set_D_limit((float)(rx_array[8]));
	*avarage_thrust = rx_array[9];
	
	//to be contined

}
void update_rotors(rotor4 thrust4){
	uint16_t tmp[4] = {thrust4.LFW, thrust4.RFC, thrust4.RBW, thrust4.LBC};
	ESC_set_power(tmp);
}

