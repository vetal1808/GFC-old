#include "I2CRoutines.h"
#include "BMP085_timer.h"
#include "mpu6050.h"
int main(void)
{
	I2C_LowLevel_Init(I2C1);

	int16_t ax =0 , ay=0, az =0, gx =0, gy=0, gz=0,i;
	MPU6050_initialize();
	MPU6050_setDLPFMode(0x03);
	MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_1000);
	MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
	MPU6050_setSampleRateDiv(3);
	for(i=0;i<20000;i++);
	MPU6050_getMotion6(&ax,&ay,&az,&gx,&gy,&gz,0);
	while(1)
	{
		MPU6050_getMotion6(&ax,&ay,&az,&gx,&gy,&gz,0);
		for(i=0;i<20000;i++);
	}

}
