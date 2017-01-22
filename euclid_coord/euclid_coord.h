#ifndef __EUCLID_COORD_H__
#define  __EUCLID_COORD_H__


#include "timer.h"
#include "helpers.h"

vector3 global_accel;


void update_altitude(vector3 accel, vector4 q);
float get_alt_velocity();
float get_altitued();

#endif
