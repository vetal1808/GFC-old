/*
 * stab_alg.c
 *
 *  Created on: 02.11.2016
 *      Author: vetal
 */
#include "stab_alg.h"


#define loop_time 0.005

float P_gain = 16.0, D_gain = 9.0, I_gain = 1.0;
float P_limit = 100.0, D_limit = 100.0, I_limit = 0.0;


float PID(float error, float d_error, uint8_t integral_switcher, float * integral_sum, vector3 * PID_summand){

	if(integral_switcher)
		*integral_sum += error*loop_time;

	*integral_sum = limit(*integral_sum, I_limit/I_gain);
	PID_summand->x = limit(error*P_gain, P_limit);
	PID_summand->y = *integral_sum*I_gain;
	PID_summand->z = limit(d_error*D_gain, D_limit);

	return PID_summand->x + PID_summand->y + PID_summand->z;
}

rotor4 calc_rotor4_thrust(vector3 torque_of_axis, float average_thrust){
	rotor4 tmp;
	tmp.LFW = (uint16_t)(average_thrust - (torque_of_axis.x + torque_of_axis.y + torque_of_axis.z));
	tmp.RFC = (uint16_t)(average_thrust - (torque_of_axis.x - torque_of_axis.y - torque_of_axis.z));
	tmp.LBC = (uint16_t)(average_thrust - (-torque_of_axis.x + torque_of_axis.y - torque_of_axis.z));
	tmp.RBW = (uint16_t)(average_thrust - (-torque_of_axis.x - torque_of_axis.y + torque_of_axis.z));
	return tmp;
}
vector3 quaternion_decomposition(vector4 q){
	float angle = - acosf(q.q0);//!TODO maybe possible replace arccos to simpler and faster function
	vector3 tmp;
	tmp.x = angle*q.q1;
	tmp.y = angle*q.q2;
	tmp.z = angle*q.q3;
	return tmp;
}

void stab_algorithm(vector4 quaternion, vector3 gyro, rotor4 * rotor4_thrust, float average_thrust){
	static float integral_sum [3] = {0.0f, 0.0f, 0.0f};
	vector3 axis_errors = quaternion_decomposition(quaternion);

	//vector3 torque;
	uint8_t integral_switcher = 0;
	if(average_thrust>integration_trottle)
		integral_switcher = 1;
	if(average_thrust<low_trottle){
		rotor4_thrust->LBC = 0;
		rotor4_thrust->LFW = 0;
		rotor4_thrust->RBW = 0;
		rotor4_thrust->RFC = 0;
		return;
	}
	torque.x = PID(axis_errors.x, gyro.x, integral_switcher, &integral_sum[0], &Ox);
	torque.y = PID(axis_errors.y, gyro.y, integral_switcher, &integral_sum[1], &Oy);
	torque.z = PID(axis_errors.z, gyro.z, integral_switcher, &integral_sum[2], &Oz);

	*rotor4_thrust = calc_rotor4_thrust(torque, average_thrust);
}

void set_P_gain(float val){
    P_gain = val;
}
void set_I_gain(float val){
	I_gain = val;
}
void set_D_gain(float val){
    D_gain = val;
}
void set_P_limit(float val){
	P_limit = val;
}
void set_I_limit(float val){
	I_limit = val;
}
void set_D_limit(float val){
	D_limit = val;
}
