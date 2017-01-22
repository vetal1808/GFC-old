#ifndef BMP085_HIGH_H_
#define BMP085_HIGH_H_

#include "BMP085_low.h"

#define use_fir_filter

void BMP085_readRawTemperature_reqest();
uint16_t BMP085_readRawTemperature_ask();
void BMP085_readRawPressure_reqest();
uint32_t BMP085_readRawPressure_ask();

int32_t BMP085_CalculatePressure(uint16_t UT, uint32_t UP);
float BMP085_CalculateAltitude(float pressure) ;



void BMP085_update();
int32_t BMP085_get_altitude();
uint32_t BMP085_measuring_pressure();
void BMP085_set_zero_pressure(uint32_t z);

void BMP085_get_data(int32_t * altitude, int32_t * velocity);
void BMP085_update();

#endif
