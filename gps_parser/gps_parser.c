/*
 * gps_parser.c
 *
 *  Created on: 15.02.2016
 *      Author: vetal
 */
#include "gps_parser.h"
#include "easy_uart.h"

static uint32_t latitude, longitude, altitude, accuracy, speed, course;
static uint32_t latitude0, longitude0, altitude0;
//static uint32_t gps_x, gps_y, gps_h; //relative coordinates
static float tmp;

int32_t my_atoi (uint8_t *buf, uint8_t l);


void gps_update()
{
	while(USART_line_available(USART2))
	{
		uint8_t tmp_buf[64];
		uint8_t received = USART_readLine(USART2,tmp_buf,64);
		uint8_t i, message = 0;
		for(i=0;i<received;i++){
			message = get_type(tmp_buf+i);
			if(message!=0)
				break;
		}
		switch (message)
		{
		case 1:
		{
			GPGGA_parse(tmp_buf+i);
			break;
		}
		case 2:
		{
			GPVTG_parse(tmp_buf+i);
			break;
		}
		}
	}
}


uint8_t gps_checksum(uint8_t * buff, uint8_t st, uint8_t fn)
{
	uint8_t i, tmp = 0;
	for( i = st ; i <= fn; i++)
	{
		tmp^= buff[i];
	}
	return tmp;
}

uint8_t isGPGGA(uint8_t * mess)
{
	if (mess[0] != 'G')
		return 0;
	if (mess[1] != 'P')
		return 0;
	if (mess[2] != 'G')
		return 0;
	if (mess[3] != 'G')
		return 0;
	if (mess[4] != 'A')
		return 0;
	return 0xFF;
}
uint8_t isGPVTG(uint8_t * mess)
{
	if (mess[0] != 'G')
		return 0;
	if (mess[1] != 'P')
		return 0;
	if (mess[2] != 'V')
		return 0;
	if (mess[3] != 'T')
		return 0;
	if (mess[4] != 'G')
		return 0;
	return 0xFF;
}

uint32_t getDeg(uint8_t s, uint8_t f, uint8_t * buff)
{


	uint32_t res = 0, tmp1, tmp2;
	if (f - s == 9){
		tmp1 = my_atoi(buff + s, 4);
		s += 5;
	}
	else{
		if (f - s == 10){
			tmp1 = my_atoi(buff + s, 5);
			s += 6;
		}
		else
			return 0;
	}

	tmp2 = my_atoi(buff + s, 4);
	res = tmp1 * 10000 + tmp2;
	return res + ((res % 1000000) * 2 / 3);



}
int8_t EW(uint8_t s, uint8_t f, uint8_t * buff)
{
	if (f - s != 1)
		return 0;
	if (buff[s] == 'E')
		return 1;
	if (buff[s] == 'W')
		return -1;
	return 0;
}
int8_t NS(uint8_t s, uint8_t f, uint8_t * buff)
{
	if (f - s != 1)
		return 0;
	if (buff[s] == 'N')
		return 1;
	if (buff[s] == 'S')
		return -1;
	return 0;
}
uint32_t getFloat(uint8_t s, uint8_t f, uint8_t * buff)
{
	uint32_t res = 0;
	if (f - s < 2)
		return -1;
	uint8_t k = s, dot;
	while (k<f)
	{
		if (buff[k] != '.')
			res = res * 10 + (buff[k] - '0');
		else
		{
			if (buff[k] == '.')
			{
				dot = f - k - 1;
			}
		}
		k++;
	}
	while (dot < 2)
	{
		res *= 10;
		dot++;
	}
	while (dot > 2)
	{
		res /= 10;
		dot--;
	}

	return res;
}


uint8_t get_type(uint8_t * buff)
{
	if (isGPGGA(buff))
		return 1;
	if (isGPVTG(buff))
		return 2;
	return 0;
}
//example $GPVTG,293.47,T,,M,1.71,N,3.17,K,A*34
void GPVTG_parse(uint8_t * buff)
{
	uint8_t comma[9], i = 0, j = 0;
	for (i = 0; i<9; i++)
	{
		do
		{
			j++;
		} while (buff[j] != ',');

		comma[i] = j;
	}
	course = (getFloat(comma[0] + 1, comma[1], buff))/100;
	if(course>180)
		course-=360;
	speed = (getFloat(comma[6] + 1, comma[7], buff)*100)/36; //1 unit is 0.1 m/s
}
void GPGGA_parse(uint8_t * buff)
{
	uint8_t comma[14], i = 0, j = 0;
	for (i = 0; i<14; i++)
	{
		do
		{
			j++;
		} while (buff[j] != ',');

		comma[i] = j;
	}
	latitude = getDeg(comma[1] + 1, comma[2], buff);
	latitude *= NS(comma[2] + 1, comma[3], buff);
	longitude = getDeg(comma[3] + 1, comma[4], buff);
	longitude *= EW(comma[4] + 1, comma[5], buff);
	accuracy = getFloat(comma[7] + 1, comma[8], buff);
	altitude = getFloat(comma[8] + 1, comma[9], buff);




}
void fix_zero_position()
{
	latitude0 = latitude;
	longitude0 = longitude;
	altitude0 = altitude;
	tmp = cos((float)latitude/(1000000*2*M_PI))*1.1;
}
void relativ_cords(uint32_t * x, uint32_t * y, uint32_t * h)
{
	// 1 unit is 0.11 meters
	//
	*y = (latitude - latitude0)*11/10;
	*x = (longitude - longitude0)*tmp;
	*h = altitude - altitude0;

}

