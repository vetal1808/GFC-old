/*
 * helpers.c
 *
 *  Created on: 11.03.2016
 *      Author: vetal
 */
#include "helpers.h"
#include "telemetry.h"

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
float limit(float value, float limit){
	if(value > limit)
		return limit;
	if(value < -limit)
		return -limit;
	return value;
}
void limit_value16(int16_t * val, int16_t lim){
	if(*val > lim)
	{
		*val =  lim;
	}
	else if(*val < -lim)
	{
		*val =  -lim;
	}
}
void euclid_from_quatern(vector4 q ,vector3_int16 * return_vector)
{
	return_vector->x = atan2 (2*(q.q0*q.q1+q.q2*q.q3),1-2*(q.q1*q.q1+q.q2*q.q2))*rad_to_minuteArc;
	return_vector->y = -asin (2*(q.q0*q.q2-q.q3*q.q1))* rad_to_minuteArc;
	return_vector->z = -atan2 (2*(q.q0*q.q3+q.q1*q.q2),1-2*(q.q2*q.q2+q.q3*q.q3))* rad_to_minuteArc;
}

void load_euclid_angle_telemetry (vector4 * q){
	vector3_int16 euclid_angles;
	euclid_from_quatern(*q, &euclid_angles);
    int16_t tmp_array[3];
	tmp_array[0] = euclid_angles.x*6;
	tmp_array[1] = euclid_angles.y*6;
	tmp_array[2] = euclid_angles.z*3;
	load_tx_buffer(tmp_array, ANGLES, 3);
}

void rotate_vector3_by_quatern(vector4 q, vector3 * return_vector)
{
	float q1q1, q2q2, q3q3,
	q0q1,q0q2,q0q3,
	q1q2,q1q3,
	q2q3;
	q0q1 = q.q0 * q.q1;
	q0q2 = q.q0 * q.q2;
	q0q3 = q.q0 * q.q3;
	q1q1 = q.q1 * q.q1;
	q1q2 = q.q1 * q.q2;
	q1q3 = q.q1 * q.q3;
	q2q2 = q.q2 * q.q2;
	q2q3 = q.q2 * q.q3;
	q3q3 = q.q3 * q.q3;
	float _x = return_vector->x, _y = return_vector->y, _z = return_vector->z;
	return_vector->x = 2.0f * ((0.5f - q2q2 - q3q3)*_x + (q1q2 - q0q3)*(_y)		 + (q1q3 + q0q2)*(_z));
	return_vector->y = 2.0f * ((q1q2 + q0q3)*_x 		 + (0.5f - q1q1 - q3q3)*(_y) + (q2q3 - q0q1)*(_z));
	return_vector->z = 2.0f * ((q1q3 - q0q2)*_x 		 + (q2q3 + q0q1)*(_y) 	     + (0.5f - q1q1 - q2q2)*(_z));
}

vector3 vector3_sub(vector3 a, vector3 b){
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}
