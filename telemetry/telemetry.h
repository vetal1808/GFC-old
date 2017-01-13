#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include "radio_channal.h"
#include "helpers.h"
void tx_update(vector4 summary_quatern, uint16_t computing_time);
void rx_update(vector4 * RC_quaternion, vector3 * gyro_shift,  float * avarage_thrust, uint8_t * motor_mask);
#endif
