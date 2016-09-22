/*
 * gps_parser.h
 *
 *  Created on: 15.02.2016
 *      Author: vetal
 */

#ifndef GPS_PARSER_H_
#define GPS_PARSER_H_


#include <stdint.h>
#include <math.h>




uint8_t gps_checksum(uint8_t * buff, uint8_t st, uint8_t fn);



void gps_update();
uint8_t isGPGGA(uint8_t * mess);
uint8_t isGPVTG(uint8_t * mess);
uint32_t getDeg(uint8_t s, uint8_t f, uint8_t * buff);
int8_t EW(uint8_t s, uint8_t f, uint8_t * buff);
int8_t NS(uint8_t s, uint8_t f, uint8_t * buff);
uint32_t getFloat(uint8_t s, uint8_t f, uint8_t * buff);
uint8_t new_mess_ready(uint8_t * s, uint8_t * f, uint8_t * buff);
uint8_t get_type(uint8_t * buff);
void GPGGA_parse(uint8_t * buff);
void GPVTG_parse(uint8_t * buff);
void fix_zero_position();



#endif /* GPS_PARSER_H_ */
