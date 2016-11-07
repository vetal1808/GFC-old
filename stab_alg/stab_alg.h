#ifndef STAB_ALG_H_
#define STAB_ALG_H_

#include "helpers.h"
#include <math.h>

#define integration_trottle 250.f
#define low_trottle 150.f

//debug intermediate data
vector3 Ox, Oy, Oz;

void stab_algorithm(vector4 quaternion, vector3 gyro, rotor4 * rotor4_thrust, float average_thrust);
vector3 quaternion_decomposition(vector4 q);
void set_P_gain(float val);
void set_I_gain(float val);
void set_D_gain(float val);
void set_P_limit(float val);
void set_I_limit(float val);
void set_D_limit(float val);

#endif
