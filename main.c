#include "I2CRoutines.h"
int main(void)
{
	I2C_LowLevel_Init(I2C1);
	uint8_t b = 1;
	Status _stat = Error;
	uint8_t a, i = 0;
	for(a = 0; a < 127; a++)
	{
		_stat = I2C_Master_BufferWrite(I2C1, &b, 1, Polling, a<<1);
		if(_stat)
		{
			i++;
		}
	}


    while(1)
    {
    }
}
