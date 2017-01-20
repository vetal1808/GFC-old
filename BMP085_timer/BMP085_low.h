/*
 * BMP085.h
 *
 *  Created on: 09.02.2016
 *      Author: vetal
 */

#ifndef BMP085_LOW_H_
#define BMP085_LOW_H_

#define BMP085_handler

#include "I2CRoutines.h"


#define BMP085_I2CADDR 0x77


#define BMP085_ULTRALOWPOWER 0
#define BMP085_STANDARD      1
#define BMP085_HIGHRES       2
#define BMP085_ULTRAHIGHRES  3
#define BMP085_CAL_AC1           0xAA  // R   Calibration data (16 bits)
#define BMP085_CAL_AC2           0xAC  // R   Calibration data (16 bits)
#define BMP085_CAL_AC3           0xAE  // R   Calibration data (16 bits)
#define BMP085_CAL_AC4           0xB0  // R   Calibration data (16 bits)
#define BMP085_CAL_AC5           0xB2  // R   Calibration data (16 bits)
#define BMP085_CAL_AC6           0xB4  // R   Calibration data (16 bits)
#define BMP085_CAL_B1            0xB6  // R   Calibration data (16 bits)
#define BMP085_CAL_B2            0xB8  // R   Calibration data (16 bits)
#define BMP085_CAL_MB            0xBA  // R   Calibration data (16 bits)
#define BMP085_CAL_MC            0xBC  // R   Calibration data (16 bits)
#define BMP085_CAL_MD            0xBE  // R   Calibration data (16 bits)

#define BMP085_CONTROL           0xF4
#define BMP085_TEMPDATA          0xF6
#define BMP085_PRESSUREDATA      0xF6
#define BMP085_READTEMPCMD          0x2E
#define BMP085_READPRESSURECMD            0x34

int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
uint16_t ac4, ac5, ac6;
uint8_t oversampling;

	uint8_t BMP085_begin(uint8_t mode);  // by default go highres

	int32_t BMP085_computeB5(int32_t UT);
	uint8_t BMP085_read8(uint8_t addr);
	uint16_t BMP085_read16(uint8_t addr);
	void BMP085_write8(uint8_t addr, uint8_t data);

	

#endif /* BMP085_H_ */
