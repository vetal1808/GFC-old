#include "nrf24.h"


#define _BV(n) (1 << n)
#define min(x,y) (x < y ? x : y)

#define RF24_PA_MAX 3
#define RF24_PA_HIGH 2
#define RF24_PA_LOW 1
#define RF24_PA_MIN 0

#define RF24_250KBPS 2
#define RF24_1MBPS 0
#define RF24_2MBPS 1

#define RF24_CRC_DISABLED 0
#define RF24_CRC_8 1
#define RF24_CRC_16 2


uint8_t  payload_size[6] = {32, 32, 32, 32, 32, 32};

uint8_t nRF24L01_flush_rx(void)
{
	uint8_t status;

	nRF24L01_NSS(LOW);
	status = nRF24L01_SPI_Send_Byte( FLUSH_RX );
	nRF24L01_NSS(HIGH);

	return status;
}
uint8_t nRF24L01_flush_tx(void)
{
	uint8_t status;

	nRF24L01_NSS(LOW);
	status = nRF24L01_SPI_Send_Byte( FLUSH_RX );
	nRF24L01_NSS(HIGH);

	return status;
}
uint8_t nRF24L01_get_status(void)
{
  uint8_t status;

  nRF24L01_NSS(LOW);
  status = nRF24L01_SPI_Send_Byte( NOP );
  nRF24L01_NSS(HIGH);

  return status;
}
void nRF24L01_setChannel(uint8_t channel)
{
  const uint8_t max_channel = 127;
  nRF24L01_Write_Regs(RF_CH, &channel, 1);
}

void nRF24L01_begin(void)
{
	nRF24L01_HW_Init();

  	nRF24L01_Delay_us(5000);


	// Set 1500uS (minimum for 32B payload in ESB@250KBPS) timeouts, to make testing a little easier
	// WARNING: If this is ever lowered, either 250KBS mode with AA is broken or maximum packet
	// sizes must never be used. See documentation for a more complete explanation.
	uint8_t tmp = (0b0100 << ARD) | (0b1111 << ARC);
	nRF24L01_Write_Regs(SETUP_RETR, &tmp, 1);
	
	// Restore our default PA level
	nRF24L01_setPALevel( RF24_PA_MAX ) ;

	// Then set the data rate to the slowest (and most reliable) speed supported by all
	// hardware.
	nRF24L01_setDataRate( RF24_1MBPS ) ;

	// Initialize CRC and request 2-byte (16bit) CRC
	nRF24L01_setCRCLength( RF24_CRC_16 ) ;

	// Disable dynamic payloads, to match dynamic_payloads_enabled setting
	tmp = 0;
	nRF24L01_Write_Regs(DYNPD, &tmp, 1);

	// Reset current status
	// Notice reset and flush is the last thing we do
	tmp = _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT);
	nRF24L01_Write_Regs(STATUS, &tmp, 1);
	// Set up default configuration.  Callers can always change it later.
	// This channel should be universally safe and not bleed over into adjacent
	// spectrum.
	nRF24L01_setChannel(76);

	// Flush buffers
	nRF24L01_flush_rx();
	nRF24L01_flush_tx();
}
void nRF24L01_RX_mode(void)
{
	uint8_t tmp;
	nRF24L01_Read_Regs(CONFIG , &tmp, 1);
	tmp |= _BV(PWR_UP) | _BV(PRIM_RX); 
	nRF24L01_Write_Regs(CONFIG, &tmp, 1);
	tmp = _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT);
	nRF24L01_Write_Regs(STATUS, &tmp, 1);

	nRF24L01_flush_rx();
	nRF24L01_flush_tx();

	// Go!
	nRF24L01_CE(HIGH);

	// wait for the radio to come up (130us actually only needed)
	nRF24L01_Delay_us(130);
}
void nRF24L01_Standby_Mode(void)
{
	nRF24L01_CE(LOW);
	nRF24L01_flush_rx();
	nRF24L01_flush_tx();
}
void nRF24L01_powerDown(void)
{	
	uint8_t tmp;
	nRF24L01_Read_Regs(CONFIG, &tmp, 1);
	tmp &= ~_BV(PWR_UP);
	nRF24L01_Write_Regs(CONFIG, &tmp, 1);
}
void nRF24L01_powerUp(void)
{
	uint8_t tmp;
	nRF24L01_Read_Regs(CONFIG, &tmp, 1);
	tmp |= _BV(PWR_UP);
	nRF24L01_Write_Regs(CONFIG, &tmp, 1);
}

uint8_t nRF24L01_TX_mode(){
	uint8_t tmp;
	nRF24L01_Read_Regs(CONFIG, &tmp, 1);
	tmp = (tmp | _BV(PWR_UP) ) & ~_BV(PRIM_RX);
	nRF24L01_Write_Regs(CONFIG, &tmp, 1);	
}

uint8_t nRF24L01_available(uint8_t * pipe_num)
{
  uint8_t status = nRF24L01_get_status();

  // Too noisy, enable if you really want lots o data!!
  //IF_SERIAL_DEBUG(print_status(status));

  uint8_t result = ( status & _BV(RX_DR) );

  if (result)
  {
	// If the caller wants the pipe number, include that
	if ( pipe_num )
		*pipe_num = ( status >> RX_P_NO ) & 0b111;

	// Clear the status bit

	// ??? Should this REALLY be cleared now?  Or wait until we
	// actually READ the payload?
	status = _BV(RX_DR);
	nRF24L01_Write_Regs(STATUS, &status,1);
	// Handle ack payload receipt
	if ( status & _BV(TX_DS) )
	{
		status = _BV(TX_DS);
		nRF24L01_Write_Regs(STATUS,&status,1);
    }
  }

  return result;
}

void nRF24L01_setPALevel(uint8_t level)
{
	uint8_t setup;
	nRF24L01_Read_Regs(RF_SETUP, &setup, 1);
	setup &= ~(_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;

	// switch uses RAM (evil!)
	if ( level == RF24_PA_MAX )
	{
		setup |= (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;
	}
	else if ( level == RF24_PA_HIGH )
	{
		setup |= _BV(RF_PWR_HIGH) ;
	}
	else if ( level == RF24_PA_LOW )
	{
		setup |= _BV(RF_PWR_LOW);
	}
	else if ( level == RF24_PA_MIN )
	{
		// nothing
	}
	else
	{
		return;
	}
	nRF24L01_Write_Regs(RF_SETUP, &setup, 1);
}

/****************************************************************************/

uint8_t nRF24L01_getPALevel(void)
{


	uint8_t setup;
	nRF24L01_Read_Regs(RF_SETUP, &setup, 1);
	return (setup >> RF_PWR_LOW) & 0b11;

}

uint8_t nRF24L01_setDataRate(uint8_t speed)
{
	uint8_t result = 0;
	uint8_t setup;
	nRF24L01_Read_Regs(RF_SETUP, &setup,1);

	// HIGH and LOW '00' is 1Mbs - our default
	setup &= ~(_BV(RF_DR_LOW) | _BV(RF_DR_HIGH)) ;

	if( speed == RF24_250KBPS )
	{
		// Must set the RF_DR_LOW to 1; RF_DR_HIGH (used to be RF_DR) is already 0
		// Making it '10'.
		setup |= _BV( RF_DR_LOW ) ;
	}
	else
	{
	// Set 2Mbs, RF_DR (RF_DR_HIGH) is set 1
	// Making it '01'
		if ( speed == RF24_2MBPS )
		{
			setup |= _BV(RF_DR_HIGH);
		}
	}
	nRF24L01_Write_Regs(RF_SETUP, &setup, 1);

	// Verify our result
	uint8_t tmp;
	nRF24L01_Read_Regs(RF_SETUP, &tmp,1);
	if ( tmp == setup )
		result = 1;

	return result;
}

/****************************************************************************/

uint8_t nRF24L01_getDataRate( void )
{
  uint8_t result ;
  uint8_t dr;
  nRF24L01_Read_Regs(RF_SETUP, &dr,1);
  dr &= (_BV(RF_DR_LOW) | _BV(RF_DR_HIGH));
  
  // switch uses RAM (evil!)
  // Order matters in our case below
  if ( dr == _BV(RF_DR_LOW) )
  {
    // '10' = 250KBPS
    result = RF24_250KBPS ;
  }
  else if ( dr == _BV(RF_DR_HIGH) )
  {
    // '01' = 2MBPS
    result = RF24_2MBPS ;
  }
  else
  {
    // '00' = 1MBPS
    result = RF24_1MBPS ;
  }
  return result ;
}

void nRF24L01_setCRCLength(uint8_t length)
{
	uint8_t config;
	nRF24L01_Read_Regs(CONFIG, &config, 1);
	config &= ~( _BV(CRCO) | _BV(EN_CRC)) ;

		// switch uses RAM (evil!)
	if ( length == RF24_CRC_DISABLED )
	{
		// Do nothing, we turned it off above. 
	}
	else if ( length == RF24_CRC_8 )
	{
		config |= _BV(EN_CRC);
	}
	else
	{
		config |= _BV(EN_CRC);
		config |= _BV( CRCO );
	}
	nRF24L01_Write_Regs(CONFIG, &config, 1);
}

/****************************************************************************/

uint8_t nRF24L01_getCRCLength(void)
{
	uint8_t result = RF24_CRC_DISABLED;
  	uint8_t config;
	nRF24L01_Read_Regs(CONFIG, &config, 1);
	config &= ( _BV(CRCO) | _BV(EN_CRC)) ;
  
	if ( config & _BV(EN_CRC ) )
	{
	if ( config & _BV(CRCO) )
		result = RF24_CRC_16;
	else
		result = RF24_CRC_8;
	}
  	return result;

}

/****************************************************************************/
static const uint8_t child_pipe[] =
{
  RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5
};
static const uint8_t child_payload_size[] =
{
  RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5
};
static const uint8_t child_pipe_enable[] =
{
  ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5
};

void nRF24L01_set_reading_pipe( uint8_t child, uint64_t address)
{
// If this is pipe 0, cache the address.  This is needed because
  // openWritingPipe() will overwrite the pipe 0 address, so
  // startListening() will have to restore it.

  uint8_t * tmp = (uint8_t *) &address;

  if (child <= 6)
  {
    // For pipes 2-5, only write the LSB
    if ( child < 2 )
    	nRF24L01_Write_Regs(child_pipe[child], tmp, 5);
    else
    {
    	tmp += 7; // set pinter to LSB
    	nRF24L01_Write_Regs(child_pipe[child], tmp, 1);
    }
    nRF24L01_setPayloadSize(payload_size[child], child);
    // Note it would be more efficient to set all of the bits for all open
    // pipes at once.  However, I thought it would make the calling code
    // more simple to do it this way.
	nRF24L01_enable_RX_pipe(child);

  }
}
uint64_t nRF24L01_get_reading_pipe_adr( uint8_t child)
{
// If this is pipe 0, cache the address.  This is needed because
  // openWritingPipe() will overwrite the pipe 0 address, so
  // startListening() will have to restore it.


	uint64_t result = 0;
	uint8_t * pointer = (uint8_t *) &result;
	if (child <= 6)
	{
		uint8_t a = min(child, 2);
		nRF24L01_Read_Regs(child_pipe[a], pointer, 5);
		// For pipes 2-5, read the LSB
		if ( child >2)
		{
			nRF24L01_Read_Regs(child_pipe[child], pointer, 1);
		}

		return result;
	}
}
void nRF24L01_setPayloadSize(uint8_t size, uint8_t child)
{
  const uint8_t max_payload_size = 32;
  payload_size[child] = min(size,max_payload_size);
  nRF24L01_Write_Regs(child_payload_size[child], &payload_size[child], 1);
}
uint8_t nRF24L01_getPayloadSize(uint8_t child)
{
  return payload_size[child];
}
void nRF24L01_enable_RX_pipe(uint8_t child)
{
	uint8_t tmp;
    nRF24L01_Read_Regs(EN_RXADDR, &tmp, 1);
    tmp |= _BV(child_pipe_enable[child]);
    nRF24L01_Write_Regs(EN_RXADDR, &tmp, 1);
}
void nRF24L01_disable_RX_pipe(uint8_t child)
{
	uint8_t tmp;
    nRF24L01_Read_Regs(EN_RXADDR, &tmp, 1);
    tmp &= ~_BV(child_pipe_enable[child]);
    nRF24L01_Write_Regs(EN_RXADDR, &tmp, 1);
}
void nRF24L01_set_writing_pipe(uint64_t adr, uint8_t payload_size){
	uint8_t * tmp = (uint8_t *) &adr;
	nRF24L01_Write_Regs(TX_ADDR, tmp, 5);
	//!TODO what to do with fix TX payload?
}
void nRF24L01_set_Address_Widths(uint8_t width){
	// from 3 to 5 bytes are available
	width -= 2;
	if(width < 4 && width > 0){
		nRF24L01_Write_Regs(SETUP_AW, &width, 1);
	}
}
uint8_t nRF24L01_get_Address_Widths(void){
	uint8_t tmp;
	nRF24L01_Read_Regs(SETUP_AW, &tmp, 1);
	return tmp + 2; 
}

/****************************************************************************/

uint8_t nRF24L01_testRPD(void)
{
	uint8_t tmp;
	nRF24L01_Read_Regs(RPD, &tmp, 1);
	return tmp;
}
uint8_t nRF24L01_end_of_transimit(void){
	uint8_t status = nRF24L01_get_status();
	if(!(status & ( _BV(TX_DS) | _BV(MAX_RT) )))
		return 1;
	else
		return 0;
}
