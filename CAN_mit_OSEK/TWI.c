/*
 * TWI.c
 *
 * Created: 03.05.2023 15:14:51
 *  Author: Raza
 */ 
#include "TWI.h"

void TWI_init(void)
{
	/* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
	TWSR = 0;                         /* no prescaler */
	TWBR = ((F_CPU/SCL_CLOCK)-16)/2;  /* must be > 10 for stable operation */
}	

/*========================== TWI START ================================*/
void TWI_start()		
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}
		
/*========================== TWI STOP ================================*/
void TWI_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while ((TWCR & (1 << TWSTO)) != 0);
}
					
/*========================== TWI WRITE ================================*/
void TWI_write( char data )
{
	/* Slave Adresse (SLA + R/W) oder Datenbyte übertragen. */
	TWDR = data;
	/* Übertragung der Daten starten und warten bis fertig. */
	TWCR = (1 << TWINT) | (1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}

/*========================== TWI READ ACK ================================*/
char TWI_readAck()
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while(!(TWCR & (1<<TWINT)));
	return TWDR;
}
							
/*========================== TWI READ NAK ================================*/
char TWI_readNak()
{
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	return TWDR;
}