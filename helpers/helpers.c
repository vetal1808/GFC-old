/*
 * helpers.c
 *
 *  Created on: 11.03.2016
 *      Author: vetal
 */
#include "helpers.h"

int32_t my_atoi (uint8_t *buf, uint8_t l)
{
	int32_t tmp=0;
	uint8_t i =0, k;
	for(i = 0; buf[i] == ' ' && i<l; i++); //skip spaces
	if(buf[i] == '-')
	{
		k = i;
		i++;
	}
	for(; i<l;i++)
	{
		if(buf[i]>= '0' && buf[i]<= '9')
			tmp = tmp*10+buf[i]-'0';
		else
			return 0;
	}
	if(buf[k] == '-')
		tmp*=(-1);
	return tmp;

}
/*
void itoa
(
	char *buf,
	int base,
	int d
)
{
	char *p = buf;
	char *p1, *p2;
	unsigned long ud = d;
	int divisor = 10;


	if (base == 'd' && d < 0) {
			*p++ = '-';
			buf++;
			ud = -d;
	} else if (base == 'x') {
			divisor = 16;
	}


	do {
			int remainder = ud % divisor;

			*p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
			ud /= divisor;
	} while (ud);





	p1 = buf;
	p2 = p - 1;
	while (p1 < p2) {
			char tmp = *p1;
			*p1 = *p2;
			*p2 = tmp;
			p1++;
			p2--;
	}
}
*/

vector4 quaterns_multiplication(vector4 A, vector4 B){
	vector4 tmp;
	tmp.q0 = A.q0*B.q0 - A.q1*B.q1 - A.q2*B.q2 - A.q3*B.q3;
	tmp.q1 = A.q2*B.q3 - A.q3*B.q2 + A.q0*B.q1 + A.q1*B.q0;
	tmp.q2 = A.q3*B.q1 - A.q1*B.q3 + A.q0*B.q2 + A.q2*B.q0;
	tmp.q3 = A.q1*B.q2 - A.q2*B.q1 + A.q0*B.q3 + A.q3*B.q0;
	return tmp;
}

vector4 euclid_to_quaterion(vector3 euclid){
	euclid.x*=0.5;
	euclid.y*=0.5;
	euclid.z*=0.5;
	float sin_fi = sin(euclid.x);
	float sin_teta = sin(euclid.y);
	float sin_psi = sin(euclid.z);
	float cos_fi = cos(euclid.x);
	float cos_teta = cos(euclid.y);
	float cos_psi = cos(euclid.z);
	float cos_fi_cos_teta = cos_fi * cos_teta;
	float sin_fi_cos_teta = sin_fi * cos_teta;
	float cos_fi_sin_teta = cos_fi * sin_teta;
	float sin_fi_sin_teta = sin_fi * sin_teta;
	vector4 tmp;
	tmp.q0 = - cos_fi_cos_teta * cos_psi - sin_fi_sin_teta * sin_psi;
	tmp.q1 = sin_fi_cos_teta * cos_psi - cos_fi_sin_teta * sin_psi;
	tmp.q2 = cos_fi_sin_teta * cos_psi + sin_fi_cos_teta * sin_psi;
	tmp.q3 = cos_fi_cos_teta * sin_psi - sin_fi_sin_teta * cos_psi;
	return tmp;
}
int16_t degree_sub360(int16_t val1, int16_t val2)
{

	int16_t tmp = val1 - val2;
	if(tmp >= 0 )
	{
		if(tmp>180*degree_to_int)
			tmp-=(360*degree_to_int);
		return tmp;
	}
	else
	{
		if(tmp<-180*degree_to_int)
			tmp+=(360*degree_to_int);
		return tmp;
	}
}

float limit(float value, float limit){
	if(value > limit)
		return limit;
	if(value < -limit)
		return -limit;
	return value;
}

void limit_value16(int16_t * val, int16_t lim)
{
	if(*val > lim)
		*val = lim;
	else
		if(*val < -lim)
			*val = -lim;
}

void limit_value32(int32_t * val, int32_t lim)
{
	if(*val > lim)
		*val = lim;
	else
		if(*val < -lim)
			*val = -lim;
}
void limit_value64(int64_t * val, int64_t lim)
{
	if(*val > lim)
		*val = lim;
	else
		if(*val < -lim)
			*val = -lim;
}
uint8_t no_overlim(int16_t val, int16_t lim)
{
	if(abs(val)<lim)
		return 1;
	else
		return 0;
}
