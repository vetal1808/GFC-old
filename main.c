#include "stm32f10x.h"
#include "stdint.h"
#include "easy_uart.h"
#include "mpu6050.h"
#include "MadgwickAHRS.h"
#include "timer.h"
#include "BMP085_timer.h"
#include "gps_parser.h"
#include "math.h"
#include "helpers.h"
#include "stab_alg.h"
#include "telemetry.h"
#include "ESC_control.h"
#include "nRF24L01_lib_stm32_gcc/nrf24.h"
void setup();


uint32_t sync_time;

int main()
{
	setup();
	sync_time = micros();
	uint8_t buff[32];
	uint8_t stat;
	int i = 0;
	 for (i = 0; i < 32; i++)
	 {
		 stat = nRF24L01_Read_Regs(i, &buff[i], 1);
	 }
	while(1)
	{
		while(micros()-sync_time<update_period_in_us);
		sync_time += update_period_in_us;
		//body of sync loop

	}


}

void setup(){
	init_timer();
	nRF24L01_HW_Init();

}
