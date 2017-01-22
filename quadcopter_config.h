#ifndef __QUADCOPTER_CONFIG_H__
#define __QUADCOPTER_CONFIG_H__

#define frq 250
#define update_period_in_us 1000000/frq
#define update_period_in_ms (float)update_period_in_us/1000.0f
#define update_period_in_sec (float)update_period_in_us/1000000.0f

#endif
