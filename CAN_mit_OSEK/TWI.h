/*
 * TWI.h
 *
 * Created: 03.05.2023 15:15:04
 *  Author: Raza
 */ 


#ifndef TWI_H_
#define TWI_H_

/* CPU Taktfrequenz. */
#ifndef F_CPU
#define F_CPU 3686400
#endif

#include <avr/io.h>

#define SCL_CLOCK 100000L		/* TWI clock in Hz */
#define DEV_LM75  0x90			/* device address:  1001 000 + R/W */
#define I2C_READ    1			/* defines the data direction (reading from I2C device) in i2c_start(),i2c_rep_start() */
#define I2C_WRITE   0			/* defines the data direction (writing to I2C device) in i2c_start(),i2c_rep_start() */

/*Initialisierung*/
void TWI_init(void);
//---------------------------------------------------------------------------------------------
/* START abschicken und warten bis fertig. */
void TWI_start(); 
//---------------------------------------------------------------------------------------------
/* STOP abschicken und warten bis fertig. */
void TWI_stop(void);
//---------------------------------------------------------------------------------------------
/* Daten versenden */
void TWI_write( char data );
//---------------------------------------------------------------------------------------------
/* Daten empfangen und mit Acknowledgement quittieren. (Mit Ackn.-Bit) */
char TWI_readAck(void);
//---------------------------------------------------------------------------------------------
/* Daten empfangen und mit Acknowledgement quittieren.(Ohne Ackn.-Bit) */
char TWI_readNak(void);
//---------------------------------------------------------------------------------------------

#endif /* TWI_H_ */