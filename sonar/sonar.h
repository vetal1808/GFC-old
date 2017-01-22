#ifndef __SONAR_H__
#define __SONAR_H__
#include "stdint.h"

void init_sonar();
void sonar_update();
uint16_t get_sonar_distanse();
#endif
