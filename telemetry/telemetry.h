#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include "radio_channal.h"
#include "helpers.h"
#include "quadcopter_config.h"


//TX mnemonic
#define ANGLES 0
#define PITCH 0
#define ROLL 1
#define YAW 2

#define PICTH_PID 3
#define PROP_PITCH 3
#define INTEGR_PITCH 4
#define DIFF_PITCH 5

#define ROLL_PID 6
#define PROP_ROLL 6
#define INTEGR_ROLL 7
#define DIFF_ROLL 8

#define YAW_PID 9
#define PROP_YAW 9
#define INTEGR_YAW 10
#define DIFF_YAW 11

#define LOOP_TIME 12
#define ALTITUDE 13

#define RESERVED_CHANNAL0 14
#define RESERVED_CHANNAL1 15
#define RESERVED_CHANNAL2 16
#define RESERVED_CHANNAL3 17
#define RESERVED_CHANNAL4 18
#define RESERVED_CHANNAL5 19

//RX mnemonic
#define RC_ANGLES 0
#define RC_PITCH 0
#define RC_ROLL 1
#define RC_YAW 2

#define PID_GAIN 3
#define PROP_GAIN 3
#define INTEGR_GAIN 4
#define DIFF_GAIN 5

#define PID_LIMIT 6
#define PROP_LIMIT 6
#define INTEGR_LIMIT 7
#define DIFF_LIMIT 8

#define THRUST 9

#define MOTOR_MASK 10
#define TX_MASK 11
#define LOST_CONNECTION_COUNTER 12

void tx_update();
void rx_update();

int8_t get_RC_state(vector4 * RC_quaternion, vector3 * RC_spin, int16_t * thrust);
#endif
