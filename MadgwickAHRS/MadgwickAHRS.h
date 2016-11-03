//=====================================================================================================
// MadgwickAHRS.h
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================
#ifndef MadgwickAHRS_h
#define MadgwickAHRS_h
#define rad_to_int 3437.746770784939
//----------------------------------------------------------------------------------------------------
// Variable declaration
#include <stdint.h>
#include <stdlib.h>
#include "helpers.h"


extern volatile vector4 q;


//---------------------------------------------------------------------------------------------------
// Function declarations


void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az);
void get_euclid(int16_t * pitch, int16_t * roll, int16_t * yaw);
void rotate_by_quatern(float * x, float * y, float * z);
#endif
//=====================================================================================================
// End of file
//=====================================================================================================
