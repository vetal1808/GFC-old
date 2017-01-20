#include "euclid_coord.h"
#include "BMP085_high.h"
#include "quadcopter_config.h"


float altitude = 0, velocity = 0;
float alfa_beta_filter(float x_prev, float x_speed, float x_high_noise, const float update_time, const float k);
void update_altitude(vector3 accel, vector4 q){
	rotate_vector3_by_quatern(q, &accel);
	global_accel = accel;
	global_accel.z -=952;
	int32_t BMP_alt, BMP_vel;
	BMP085_get_data(&BMP_alt, &BMP_vel);
	velocity = alfa_beta_filter(velocity, global_accel.z, (float)BMP_vel, update_period_in_sec, 0.999);
	altitude = alfa_beta_filter(altitude, velocity, (float)BMP_alt, update_period_in_sec, 0.999);
}

float get_alt_velocity(){
	return velocity;
}

float get_altitued(){
	return altitude;
}
float alfa_beta_filter(float x_prev, float x_speed, float x_high_noise, const float update_time, const float k){
	const float k_ = 1.0 - k;
	return (x_prev + x_speed * update_time)*k + x_high_noise * k_;
}
