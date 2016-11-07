#include "mpu6050.h"

uint8_t mpu6050_dev_addr;
int16_t gx0=0, gy0=0, gz0=0;

float gyro_ratio = gyro250_to_radians;
float accel_ratio = int2G_to_float;

void MPU6050_initialize()
{
	mpu6050_dev_addr = MPU6050_DEFAULT_ADDRESS;
    MPU6050_setClockSource(MPU6050_CLOCK_PLL_XGYRO);

    MPU6050_setBypassMode();
    MPU6050_setSleepEnabled(0); // thanks to Jack Elston for pointing this one out!

}

void MPU6050_setClockSource(uint8_t source)
{
	I2C1_write_bits(mpu6050_dev_addr,
                  MPU6050_RA_PWR_MGMT_1,
                  MPU6050_PWR1_CLKSEL_BIT,
                  MPU6050_PWR1_CLKSEL_LENGTH,
                  source);
}

void MPU6050_setFullScaleGyroRange(uint8_t range)
{
	gyro_ratio = gyro250_to_radians * (range + 1);
	I2C1_write_bits(mpu6050_dev_addr,
                  MPU6050_RA_GYRO_CONFIG,
                  MPU6050_GCONFIG_FS_SEL_BIT,
                  MPU6050_GCONFIG_FS_SEL_LENGTH,
                  range);
}

void MPU6050_setFullScaleAccelRange(uint8_t range)
{
	accel_ratio = int2G_to_float * (range + 1);
	I2C1_write_bits(mpu6050_dev_addr,
                  MPU6050_RA_ACCEL_CONFIG,
                  MPU6050_ACONFIG_AFS_SEL_BIT,
                  MPU6050_ACONFIG_AFS_SEL_LENGTH,
                  range);
}

void MPU6050_setSleepEnabled(uint8_t enabled)
{
	I2C1_write_bit(mpu6050_dev_addr,
                 MPU6050_RA_PWR_MGMT_1,
                 MPU6050_PWR1_SLEEP_BIT,
                 enabled);
}

/*
 * Just for accessing HMC from MC
 * data got from http://forum.arduino.cc/index.php?topic=223345.msg2142479#msg2142479
 */
void MPU6050_setBypassMode()
{
    I2C1_read_bytes(mpu6050_dev_addr, MPU6050_RA_INT_PIN_CFG, 1, (uint8_t *)0x02);
    I2C1_read_bytes(mpu6050_dev_addr, MPU6050_RA_USER_CTRL, 1, (uint8_t *)0x00);
}

/*          |   ACCELEROMETER    |           GYROSCOPE
* DLPF_CFG | Bandwidth | Delay  | Bandwidth | Delay  | Sample Rate
* ---------+-----------+--------+-----------+--------+-------------
* 0        | 260Hz     | 0ms    | 256Hz     | 0.98ms | 8kHz
* 1        | 184Hz     | 2.0ms  | 188Hz     | 1.9ms  | 1kHz
* 2        | 94Hz      | 3.0ms  | 98Hz      | 2.8ms  | 1kHz
* 3        | 44Hz      | 4.9ms  | 42Hz      | 4.8ms  | 1kHz
* 4        | 21Hz      | 8.5ms  | 20Hz      | 8.3ms  | 1kHz
* 5        | 10Hz      | 13.8ms | 10Hz      | 13.4ms | 1kHz
* 6        | 5Hz       | 19.0ms | 5Hz       | 18.6ms | 1kHz
* 7        |   -- Reserved --   |   -- Reserved --   | Reserved
* </pre>
*/
void MPU6050_setDLPFMode(uint8_t mode)
{
	I2C1_write_bits(mpu6050_dev_addr,
                  MPU6050_RA_CONFIG,
                  MPU6050_CFG_DLPF_CFG_BIT,
                  MPU6050_CFG_DLPF_CFG_LENGTH,
                  mode);
}

inline void MPU6050_getMotion6(int16_t* ax,
                        int16_t* ay,
                        int16_t* az,
                        int16_t* gx,
                        int16_t* gy,
                        int16_t* gz)
{

    uint8_t rx_buf[14];
    Status _status = I2C1_read_bytes(mpu6050_dev_addr, MPU6050_RA_ACCEL_XOUT_H, 14, rx_buf);

    *ax = (((int16_t)rx_buf[0]) << 8) | rx_buf[1];
    *ay = (((int16_t)rx_buf[2]) << 8) | rx_buf[3];
    *az = (((int16_t)rx_buf[4]) << 8) | rx_buf[5];
    *gx = (((int16_t)rx_buf[8]) << 8) | rx_buf[9];
    *gy = (((int16_t)rx_buf[10]) << 8) | rx_buf[11];
    *gz = (((int16_t)rx_buf[12]) << 8) | rx_buf[13];

	*gx -= gx0;
	*gy -= gy0;
	*gz -= gz0;

}
void MPU6050_calibration(){
	int i =0;
	int16_t tmp[6];
	int32_t x = 0,y = 0,z = 0;
	for(i=0; i<10000; i++)
	{
		MPU6050_getMotion6(&tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]);
		x += tmp[3];
		y += tmp[4];
		z += tmp[5];
	}

	gx0 = x / 10000;
	gy0 = y / 10000;
	gz0 = z / 10000;
}

void MPU6050_getFloatMotion6(vector3 * acc, vector3 * gyro){
	int16_t tmp[6];
	MPU6050_getMotion6(&tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]);

	acc->x = (float)tmp[0]*accel_ratio;
	acc->y = (float)tmp[1]*accel_ratio;
	acc->z = (float)tmp[2]*accel_ratio;

	gyro->x = (float)tmp[3]*gyro_ratio;
	gyro->y = (float)tmp[4]*gyro_ratio;
	gyro->z = (float)tmp[5]*gyro_ratio;
}

/** Get full-scale gyroscope range.
 * The FS_SEL parameter allows setting the full-scale range of the gyro sensors,
 * as described in the table below.
 *
 * <pre>
 * 0 = +/- 250 degrees/sec
 * 1 = +/- 500 degrees/sec
 * 2 = +/- 1000 degrees/sec
 * 3 = +/- 2000 degrees/sec
 * </pre>
 *

 */

/** Set full-scale gyroscope range.
 * @param range New full-scale gyroscope range value
 * @see getFullScaleRange()
 * @see MPU6050_GYRO_FS_250
 * @see MPU6050_RA_GYRO_CONFIG
 * @see MPU6050_GCONFIG_FS_SEL_BIT
 * @see MPU6050_GCONFIG_FS_SEL_LENGTH
 */


void MPU6050_setAccelFIFOEnabled(uint8_t flag){

	I2C1_write_bit(mpu6050_dev_addr, MPU6050_RA_FIFO_EN, MPU6050_ACCEL_FIFO_EN_BIT, flag);

}

void MPU6050_setFIFOEnabled(uint8_t enabled) {
	I2C1_write_bit(mpu6050_dev_addr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_FIFO_EN_BIT, enabled);
}
uint16_t MPU6050_getFIFOCount() {
    uint8_t rx_buf[2];
    I2C1_read_bytes(mpu6050_dev_addr, MPU6050_RA_FIFO_COUNTH, 2, rx_buf);
    return (((uint16_t)rx_buf[0]) << 8) | rx_buf[1];
}
void MPU6050_getFIFOBytes(uint8_t *data, uint8_t length) {

    I2C1_read_bytes(mpu6050_dev_addr, MPU6050_RA_FIFO_R_W, length, data);
}
void MPU6050_setGyrosFIFOEnabled(uint8_t enabled) {
	I2C1_write_bits(mpu6050_dev_addr, MPU6050_RA_FIFO_EN, MPU6050_XG_FIFO_EN_BIT, 3, enabled);

}

void MPU6050_setSampleRateDiv(uint8_t data) {
	I2C1_write_bytes(mpu6050_dev_addr, MPU6050_RA_SMPLRT_DIV, 1, &data);
}
