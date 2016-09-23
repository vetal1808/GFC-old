

#include "stm32f10x.h"
#include "stdint.h"
#include "easy_uart.h"
#include "mpu6050.h"
#include "MadgwickAHRS.h"
#include "timer.h"
#include "BMP085_timer.h"
#include "gps_parser.h"


int main()
{
	ESC_init();
	uint16_t vals[] = {10,10,10,10};
	while(1)
	{
		ESC_set_power(vals);
	}


}
