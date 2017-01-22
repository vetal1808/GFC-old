/*
 * BMP085.c
 *
 *  Created on: 09.02.2016
 *      Author: vetal
 */
#include "BMP085_low.h"
#include <math.h>
#include "timer.h"









uint8_t BMP085_begin(uint8_t mode) {
  if (mode > BMP085_ULTRAHIGHRES)
    mode = BMP085_ULTRAHIGHRES;
  oversampling = mode;



  if (BMP085_read8(0xD0) != 0x55)
	  return 0;

  /* read calibration data */
  ac1 = BMP085_read16(BMP085_CAL_AC1);
  ac2 = BMP085_read16(BMP085_CAL_AC2);
  ac3 = BMP085_read16(BMP085_CAL_AC3);
  ac4 = BMP085_read16(BMP085_CAL_AC4);
  ac5 = BMP085_read16(BMP085_CAL_AC5);
  ac6 = BMP085_read16(BMP085_CAL_AC6);

  b1 = BMP085_read16(BMP085_CAL_B1);
  b2 = BMP085_read16(BMP085_CAL_B2);

  mb = BMP085_read16(BMP085_CAL_MB);
  mc = BMP085_read16(BMP085_CAL_MC);
  md = BMP085_read16(BMP085_CAL_MD);


  return 255;
}

int32_t BMP085_computeB5(int32_t UT) {
  int32_t X1 = (UT - (int32_t)ac6) * ((int32_t)ac5) >> 15;
  int32_t X2 = ((int32_t)mc << 11) / (X1+(int32_t)md);
  return X1 + X2;
}

uint8_t BMP085_read8(uint8_t a) {
	uint8_t ret;
	Status _status = I2C1_read_bytes(BMP085_I2CADDR, a, 1, &ret);

	return ret;
}

uint16_t BMP085_read16(uint8_t a) {

	uint8_t rx_buf[2];
	Status _status = I2C1_read_bytes(BMP085_I2CADDR, a, 2, rx_buf);
	return (((int16_t)rx_buf[0]) << 8) | rx_buf[1];
}

void BMP085_write8(uint8_t a, uint8_t d) {
	Status _status = I2C1_write_bytes(BMP085_I2CADDR, a, 1, &d);
}




