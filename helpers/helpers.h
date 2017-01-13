/*
 * helpers.h
 *
 *  Created on: 11.03.2016
 *      Author: vetal
 */

#ifndef HELPERS_H_
#define HELPERS_H_

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define degree_to_int 60
#define frq 250
#define update_period_in_us 1000000/frq
#define update_period_in_ms (float)update_period_in_us/1000.0f
#define update_period_in_sec (float)update_period_in_us/1000000.0f
#define rad_to_minuteArc 3437.746770784939
#define minuteArc_to_rad 2.908882086e-4

typedef struct{
   float q0, q1, q2, q3;
} vector4;
typedef struct{
   uint16_t LFW, RFC,
   	   	   LBC, RBW;
} rotor4;
typedef struct{
   float x, y, z;
} vector3;
typedef struct{
   int16_t x, y, z;
} vector3_int16;


int32_t my_atoi(uint8_t *buf, uint8_t l);
int16_t degree_sub360(int16_t val1, int16_t val2);
float limit(float value, float limit);
void limit_value16(int16_t * val, int16_t lim);
void limit_value32(int32_t * val, int32_t lim);
void limit_value64(int64_t * val, int64_t lim);
uint8_t no_overlim(int16_t val, int16_t lim);
vector4 quaterns_multiplication(vector4 A, vector4 B);
vector4 euclid_to_quaterion(vector3 euclid);
void euclid_from_quatern(vector4 q ,vector3_int16 * return_vector);
void rotate_vector3_by_quatern(vector4 q, vector3 * return_vector);
#endif /* HELPERS_H_ */
