#include "I2CRoutines.h"
#include "BMP085_timer.h"
int main(void)
{
	I2C_LowLevel_Init(I2C1);

	BMP085_begin(BMP085_ULTRAHIGHRES);
	/*
	uint8_t b = 0xAA;
	uint8_t rx_buff[16];
	Status _stat = Error, _stat2 = Error;
	_stat = I2C_Master_BufferWrite(I2C1, &b, 1, Polling, (0x77<<1));
	_stat2 = I2C_Master_BufferRead(I2C1, rx_buff, 16, Polling, (0x77<<1));
	 */

    while(1)
    {
    }
}
