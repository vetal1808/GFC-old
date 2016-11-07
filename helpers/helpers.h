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

typedef struct
{
   float q0, q1, q2, q3;
} vector4;
typedef struct
{
   uint16_t LFW, RFC,
   	   	   LBC, RBW;
} rotor4;

typedef struct
{
   float x, y, z;
} vector3;


int32_t my_atoi(uint8_t *buf, uint8_t l);
//void itoa (char *buf, int base, int d);
int16_t degree_sub360(int16_t val1, int16_t val2);
float limit(float value, float limit);
void limit_value16(int16_t * val, int16_t lim);
void limit_value32(int32_t * val, int32_t lim);
void limit_value64(int64_t * val, int64_t lim);
uint8_t no_overlim(int16_t val, int16_t lim);
vector4 quaterns_multiplication(vector4 A, vector4 B);
vector4 euclid_to_quaterion(vector3 euclid);
#endif /* HELPERS_H_ */
