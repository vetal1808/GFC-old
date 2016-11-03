/*
 * stab_alg.c
 *
 *  Created on: 02.11.2016
 *      Author: vetal
 */
#include "stab_alg.h"


#define loop_time 0.005

float P_gain = 1000.0, D_gain = 000.0, I_gain = 0.0;
float P_limit = 500.0, D_limit = 100.0, I_limit = 100.0;



float PID(float error, float d_error, uint8_t integral_switcher, float * integral_sum){

	if(integral_switcher)
		*integral_sum += error*loop_time;

	*integral_sum = limit(*integral_sum, I_limit/I_gain);
	float integr = *integral_sum*I_gain;
	float prop = limit(error*P_gain, P_limit);
	float diff = limit(d_error*D_gain, D_limit);

	return prop + diff + integr;
}

rotor4 calc_rotor4_thrust(vector3 torque_of_axis, float average_thrust){
	rotor4 tmp;//!TODO check sings
	tmp.LFW = (uint16_t)(average_thrust - (torque_of_axis.x + torque_of_axis.y - torque_of_axis.z));
	tmp.RFC = (uint16_t)(average_thrust - (torque_of_axis.x - torque_of_axis.y + torque_of_axis.z));
	tmp.LBC = (uint16_t)(average_thrust - (-torque_of_axis.x + torque_of_axis.y + torque_of_axis.z));
	tmp.RBW = (uint16_t)(average_thrust - (-torque_of_axis.x - torque_of_axis.y - torque_of_axis.z));
	return tmp;
}
vector3 quaternion_decomposition(vector4 q){
	float angle = acosf(q.q0);//!TODO maybe possible replace arccos to simpler and faster function
	vector3 tmp;
	tmp.x = angle*q.q1;
	tmp.y = angle*q.q2;
	tmp.z = angle*q.q3;
	return tmp;
}

void stab_algorithm(vector4 quaternion, vector3 gyro, rotor4 * rotor4_thrust, float average_thrust){
	static float integral_sum [3] = {0.0f, 0.0f, 0.0f};
	vector3 axis_errors = quaternion_decomposition(quaternion);

	vector3 torque_of_axis;
	uint8_t integral_switcher = 0;
	if(average_thrust>integration_trottle)
		integral_switcher = 1;
	if(average_thrust<low_trottle)
		return;
	torque_of_axis.x = PID(axis_errors.x, gyro.x, integral_switcher, &integral_sum[0]);
	torque_of_axis.y = PID(axis_errors.y, gyro.y, integral_switcher, &integral_sum[1]);
	torque_of_axis.z = PID(axis_errors.z, gyro.z, integral_switcher, &integral_sum[2]);

	*rotor4_thrust = calc_rotor4_thrust(torque_of_axis, average_thrust);
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
