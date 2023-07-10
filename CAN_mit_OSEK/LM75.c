/*
 * LM75.c
 *
 * Created: 03.05.2023 15:40:49
 *  Author: Raza
 */

#include "LM75.h" 
#include "TWI.h"

/* Initialisierung des Temperaturmoduls zur Temperaturmessung */
void LM75_init(void)
{
		TWI_start();
		TWI_write(DEV_LM75 + I2C_WRITE);
		TWI_write(0x01);
		TWI_write(0x00);
		TWI_stop();
}

/* Auslesen der Temperatur als 11 Bit Festkommazahl mit LSB = 2^(?3) */
uint16_t  ReadTemp(void)
{
	    uint8_t m, l;
		uint16_t temp;
		TWI_start();
		TWI_write(DEV_LM75 + I2C_WRITE);
		TWI_write(0x00);
		TWI_start();
		TWI_write(DEV_LM75 + I2C_READ);
		m = TWI_readAck();
		l = TWI_readNak();
		TWI_stop();
		temp = m * 256 + l;
		temp = temp >> 5;
		return temp;
}