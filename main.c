#include "stm32f10x.h"
#include "stdint.h"
#include "easy_uart.h"
#include "mpu6050.h"
#include "MadgwickAHRS.h"
#include "timer.h"
#include "BMP085_timer.h"
#include "gps_parser.h"

#define period 3000
#define period_in_sec 0.003
#define gyro1000_to_radians 1920.0
#define int2G_to_float 0.059814453125 // convert 16bit int data 2G sens to sm/sec^2 in float

int main()
{




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

	int16_t ax = 0, ay = 0, az = 0;
	int16_t gx = 0, gy = 0, gz = 0;
	int16_t pitch, roll, yaw;
	uint8_t buff[6];
	uint32_t sync_time = micros();

	float ax_bias=0, ay_bias=0, az_bias=15985;
	float vx = 0, vy = 0, vz = 0;
	uint32_t skip = 0;

	while(1)
	{
		while(micros()-sync_time<period);
		sync_time = micros();
		MPU6050_getMotion6(&ax, &ay, &az, &gx, &gy, &gz, 0);
		MadgwickAHRSupdateIMU((float)gx/gyro1000_to_radians, (float)gy/gyro1000_to_radians, (float)(gz)/(gyro1000_to_radians), ax, ay, az);
		BMP085_update();

		float _ax = (float)ax, _ay = (float)ay, _az = (float)az;
		rotate_by_quatern(&_ax, &_ay, &_az); // acceleration in global coordinate

		int32_t baro_alt_velocity, baro_alt;
		BMP085_get_data(&baro_alt, &baro_alt_velocity);

		_az = (_az - az_bias)*int2G_to_float;
		vz = ((_az*period_in_sec+vz)*0.9995)+((float)baro_alt_velocity*0.0005);
		az_bias+=(vz-(float)baro_alt_velocity)*0.0001;

		float new_alt = (vz*period_in_sec+new_alt)*0.999+ baro_alt*0.001;

		send((int32_t)(_az*100.), 'A');
		send((int32_t)(vz*300.), 'B');
		send((int32_t)((az_bias-15985.)*100.), 'C');
		send((int32_t)(new_alt*300.), 'D');
		USART_send(USART3,"W\n",2);





	}

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
