/*
 * BMP085.c
 *
 *  Created on: 09.02.2016
 *      Author: vetal
 */
#include "BMP085_timer.h"
#include <math.h>
#include "timer.h"


uint8_t oversampling;

int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
uint16_t ac4, ac5, ac6;


//volatile float BMP_zero_press = 100000;
float BMP_zero_press = 100000;
volatile int32_t BMP_Altitude;
volatile int32_t BMP_Alt_, BMP_velo_;



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

void BMP085_readRawTemperature_reqest()
{
	BMP085_write8(BMP085_CONTROL, BMP085_READTEMPCMD);
}
uint16_t BMP085_readRawTemperature_ask()
{
	return BMP085_read16(BMP085_TEMPDATA);
}
void BMP085_readRawPressure_reqest()
{
	BMP085_write8(BMP085_CONTROL, BMP085_READPRESSURECMD + (oversampling << 6));
}
uint32_t BMP085_readRawPressure_ask()
{
	uint16_t tmp[2];

	tmp[0] = BMP085_read16(BMP085_PRESSUREDATA);
	tmp[1] = BMP085_read8(BMP085_PRESSUREDATA+2);

	return ((tmp[0]<<8 | tmp[1]) >> (8-oversampling));
}
int32_t BMP085_readPressure(uint16_t UT, uint32_t UP)
{
	int32_t B3, B5, B6, X1, X2, X3, p;
	uint32_t B4, B7;

	  B5 = BMP085_computeB5(UT);



	  // do pressure calcs
	  B6 = B5 - 4000;
	  X1 = ((int32_t)b2 * ( (B6 * B6)>>12 )) >> 11;
	  X2 = ((int32_t)ac2 * B6) >> 11;
	  X3 = X1 + X2;
	  B3 = ((((int32_t)ac1*4 + X3) << oversampling) + 2) / 4;



	  X1 = ((int32_t)ac3 * B6) >> 13;
	  X2 = ((int32_t)b1 * ((B6 * B6) >> 12)) >> 16;
	  X3 = ((X1 + X2) + 2) >> 2;
	  B4 = ((uint32_t)ac4 * (uint32_t)(X3 + 32768)) >> 15;
	  B7 = ((uint32_t)UP - B3) * (uint32_t)( 50000UL >> oversampling );



	  if (B7 < 0x80000000) {
	    p = (B7 * 2) / B4;
	  } else {
	    p = (B7 / B4) * 2;
	  }
	  X1 = (p >> 8) * (p >> 8);
	  X1 = (X1 * 3038) >> 16;
	  X2 = (-7357 * p) >> 16;



	  p = p + ((X1 + X2 + (int32_t)3791)>>4);

	  return p;
}

float BMP085_readAltitude(float pressure)
{
  return 4433000 * (1.0 - pow(pressure /BMP_zero_press,0.1903));
}


void BMP085_update()
{
	static uint8_t BMP_state = 0, BMP_temp_skip = 0;
	static uint32_t start_time;
	static uint32_t BMP_tmp1, BMP_tmp2;

	switch (BMP_state){
		case 0 :{
			BMP085_readRawTemperature_reqest();
			start_time = micros();
			BMP_state++;
			break;
		}
		case 1 :{
			if(micros() - start_time > 4499){
				BMP_tmp1 = BMP085_readRawTemperature_ask();
				BMP085_readRawPressure_reqest();
				BMP_state++;
				start_time = micros();
			}
			break;
		}
		case 2 :{
			if(micros() - start_time > 25499){
				BMP_tmp2 = BMP085_readRawPressure_ask();
				BMP_Altitude = BMP085_readAltitude(BMP085_readPressure(BMP_tmp1,BMP_tmp2));
				BMP085_Handler();
				if(BMP_temp_skip < 4){ // skip temperature reading 5 times
					BMP085_readRawPressure_reqest();
					start_time = micros();
					BMP_temp_skip++;
				}
				else {
					BMP_state = 0;
					BMP_temp_skip = 0;
				}
			}
			break;

		}
		default :{
			BMP_state = 0;
			break;
		}
	}
}
int32_t BMP085_get_altitude(){
	return BMP_Altitude;
}
uint32_t BMP085_meagure_press(){
	uint32_t BMP_tmp1, BMP_tmp2;
	BMP085_readRawTemperature_reqest();
	delay_us(4500);
	BMP_tmp1 = BMP085_readRawTemperature_ask();
	BMP085_readRawPressure_reqest();
	delay_us(24500);
	BMP_tmp2 = BMP085_readRawPressure_ask();
	return BMP085_readPressure(BMP_tmp1,BMP_tmp2);
}
void BMP085_set_zero_pressure(uint32_t z){
	BMP_zero_press = z;
}
#ifdef BMP085_handler

void BMP085_Handler(){
	#define len 32
    const int32_t FIRCoef[len] = {
    		2271,
    		         2540,
    		         2893,
    		         3247,
    		         3719,
    		         4193,
    		         4841,
    		         5495,
    		         6422,
    		         7361,
    		         8793,
    		        10244,
    		        12837,
    		        15463,
    		        23719,
    		        32049,
    		        23719,
    		        15463,
    		        12837,
    		        10244,
    		         8793,
    		         7361,
    		         6422,
    		         5495,
    		         4841,
    		         4193,
    		         3719,
    		         3247,
    		         2893,
    		         2540,
    		         2271,
    		         2001
    };
	static int32_t alt_seq[len];
	static int32_t alt_velo_seq[len];
	uint8_t i;
	for(i = len-1; i != 0; i--)
		alt_seq[i] = alt_seq[i-1];
	alt_seq[0] = BMP_Altitude;
	for(i = len-1; i != 0; i--)
		alt_velo_seq[i] = alt_velo_seq[i-1];
	alt_velo_seq[0] = (alt_seq[0] - alt_seq[1])*36;
	int64_t	tmp = 0;
	for(i = 0; i < len; i++)
		tmp+=alt_seq[i]*FIRCoef[i];
	BMP_Alt_ = tmp/262144;
	tmp = 0;
	for(i = 0; i < len; i++)
		tmp+=alt_velo_seq[i]*FIRCoef[i];
	BMP_velo_ = tmp/262144;
}
void BMP085_get_data(int32_t * altitude, int32_t * velocity){
	*altitude = BMP_Alt_;
	*velocity = BMP_velo_;
}
#endif

