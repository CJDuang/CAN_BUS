/*
 * mcp2515.c
 *
 * Created: 02.03.2023 14:41:01
 *  Author: 
 */

/* Copyright (c) 2007 Fabian Greif
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------


#include <avr/io.h>
#include <util/delay.h>

#include <stdint.h>
//#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>	// f�r bit_is_clear(...) und bit_is_set(...) notwendig

#include "global.h"
#include "mcp2515.h"
#include "mcp2515_defs.h"
#include "defaults.h"



// -------------------------------------------------------------------------
/* Senden oder Empfangen der Daten �ber SPI-Bus */
uint8_t spi_putc( uint8_t data ) 
{
	// ein Byte in den Sendebuffer legen
	SPDR = data;
	// warten bis gesendet wird
	while( !( SPSR & (1<<SPIF) ) );
	return SPDR;
}

// -------------------------------------------------------------------------
/*Funktion zum Schreiben von Registerwertenen */
void mcp2515_write_register( uint8_t adress, uint8_t data )
{
	RESET(MCP2515_CS);			// CS - Leitung auf LOW-Pegel legen
	spi_putc(SPI_WRITE);        // SPI-Kommando SPI_WRITE senden
	spi_putc(adress);           // Registeradresse senden
	spi_putc(data);				// Daten senden
	SET(MCP2515_CS);			// CS - Leitung wieder auf HIGH-Pegel ziehen
}

// -------------------------------------------------------------------------
/* Funktion zum Lesen von Registerwerten */
uint8_t mcp2515_read_register(uint8_t adress)
{
	uint8_t data;
	RESET(MCP2515_CS);			// CS - Leitung auf LOW-Pegel legen
	spi_putc(SPI_READ);			// SPI-Kommando SPI_READ senden
	spi_putc(adress);			// Registeradresse senden
	data = spi_putc(0xff);		// Daten empfangen, hierbei wird ein Dummy-Byte als Parameter �bergeben
	SET(MCP2515_CS);			// CS - Leitung wieder auf HIGH-Pegel ziehen
	return data;				// Empfangene Daten zur�ckgeben
}


// -------------------------------------------------------------------------
/* 
Eine weiteres n�tzliche Feature des MCP2515 ist die Bit-Modify-Funktion. Dabei werden nur bestimmt Bits gesetzt bzw. gel�scht. 
Ein Bespiel daf�r wird man unter anderem bei der Initialisierung sehen, wo damit die Operationsmodi umgeschaltet werden
Allerdings muss man aufpassen da sich diese Funktion nur auf bestimmte Register anwenden l�sst (siehe z.B. Datenblatt auf Seite 59).
Es wird dabei zuerst eine Bitmaske gesendet. Nur die Bits die dabei gesetzt sind werden sp�ter auch ver�ndert. Danach werden die 
eigentlichen Daten gesendet. Ist ein Bit in der Maske gesetzt und bei den Daten gel�scht, so wird auch das entsprechende Bit im Register
des MCP2515 gel�scht. Analog wenn das entsprechende Bit im Datenbyte gesetzt ist.
*/
void mcp2515_bit_modify(uint8_t adress, uint8_t mask, uint8_t data)
{
	RESET(MCP2515_CS);
	spi_putc(SPI_BIT_MODIFY);
	spi_putc(adress);
	spi_putc(mask);
	spi_putc(data);
	SET(MCP2515_CS);
}


// ----------------------------------------------------------------------------

uint8_t mcp2515_read_status(uint8_t type)
{
	uint8_t data;
	RESET(MCP2515_CS);
	spi_putc(type);
	data = spi_putc(0xff);
	SET(MCP2515_CS);
	return data;
}

// -------------------------------------------------------------------------


// Den SPI-Interface des ATmegas und den CAN-Controller einstellen (Initialisation) 
// Das einzig Schwierige bei der Initialisierung ist das Einstellen des Bit-Timings bzw. der Bit Rate des CAN Buses.
uint8_t mcp2515_init(uint8_t speed)
{
	SET(MCP2515_CS);			// Ilya: Hier ganz am Anfang macht es keinen Sinn?
	SET_OUTPUT(MCP2515_CS);     // PB2 auf Ausgang f�r Chip Slect (CS) Pin setzen alternativ DDRB |= 1 << PB2
	
	RESET(P_SCK);
	RESET(P_MOSI);
	RESET(P_MISO);
	
	SET_OUTPUT(P_SCK);
	SET_OUTPUT(P_MOSI);
	SET_INPUT(P_MISO);
	
	SET_INPUT(MCP2515_INT);
	SET(MCP2515_INT);
	
	// active SPI master interface
	SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR1)|(1<<SPR0);
	SPSR = 0;
	
	// reset MCP2515 by software reset.
	// After this he is in configuration mode.
	RESET(MCP2515_CS);
	spi_putc(SPI_RESET);
	SET(MCP2515_CS);
	
	// wait a little bit until the MCP2515 has restarted
	_delay_us(10);
	
	// load CNF1..3 Register
	RESET(MCP2515_CS);
	spi_putc(SPI_WRITE);
	spi_putc(CNF3);

    /*Auswahl der CAN-Baudrate*/
	switch(speed)
	{
		// Bitrate 1000 kbps at 16 MHz
		case 0:	spi_putc((1<<PHSEG21));
				spi_putc((1<<BTLMODE)|(1<<PHSEG11));
				spi_putc(0);
				break;
		
		// Bitrate 500 kbps at 16 MHz	
		case 1:	spi_putc((1<<PHSEG21));		
				spi_putc((1<<BTLMODE)|(1<<PHSEG11));
				spi_putc(1<<BRP0);
				break;
			
		// Bitrate 250 kbps at 16 MHz
		case 3: spi_putc((1<<PHSEG21));
		        spi_putc((1<<BTLMODE)|(1<<PHSEG11)); 
				spi_putc((1<<BRP1)|(1<<BRP0));
			    break;
				
		// Bitrate 125 kbps at 16 MHz		
		case 7:	spi_putc((1<<PHSEG21));		
				spi_putc((1<<BTLMODE)|(1<<PHSEG11));
				spi_putc((1<<BRP2)|(1<<BRP1)|(1<<BRP0));
				break;
	}
	
	// activate interrupts
	spi_putc((1<<RX1IE)|(1<<RX0IE));
	SET(MCP2515_CS);
	
	// test if we could read back the value => is the chip accessible?
	if (mcp2515_read_register(CNF1) != speed) 
	{
		return false;
	}
	
	// deaktivate the RXnBF Pins (High Impedance State)
	mcp2515_write_register(BFPCTRL, 0);
	
	// set TXnRTS as inputs
	mcp2515_write_register(TXRTSCTRL, 0);
	
	// turn off filters => receive any message
	mcp2515_write_register(RXB0CTRL, (1<<RXM1)|(1<<RXM0));
	mcp2515_write_register(RXB1CTRL, (1<<RXM1)|(1<<RXM0));
	
	// reset device to normal mode
	mcp2515_write_register(CANCTRL, 0);

	return true;
}

// ----------------------------------------------------------------------------
// check if there are any new messages waiting

uint8_t mcp2515_check_message(void) {
	return (!IS_SET(MCP2515_INT));
}

// ----------------------------------------------------------------------------
// check if there is a free buffer to send messages

uint8_t mcp2515_check_free_buffer(void)
{
	uint8_t status = mcp2515_read_status(SPI_READ_STATUS);
	
	if ((status & 0x54) == 0x54) {
		// all buffers used
		return false;
	}
	
	return true;
}

// ----------------------------------------------------------------------------
void mcp2515_get_message(tCAN *message)
{
	// read status
	uint8_t status = mcp2515_read_status(SPI_RX_STATUS);
	uint8_t addr;
	uint8_t t;
	if (bit_is_set(status,6)) {
		// message in buffer 0
		addr = SPI_READ_RX;
	}
	else if (bit_is_set(status,7)) {
		// message in buffer 1
		addr = SPI_READ_RX | 0x04;
	}
	else {
		// Error: no message available
		return;
	}

	RESET(MCP2515_CS);
	spi_putc(addr);
	
	// read id
	message->id  = (uint16_t) spi_putc(0xff) << 3;
	message->id |=            spi_putc(0xff) >> 5;
	
	spi_putc(0xff);
	spi_putc(0xff);
	
	// read DLC
	uint8_t length = spi_putc(0xff) & 0x0f;
	
	message->header.length = length;
	message->header.rtr = (bit_is_set(status, 3)) ? 1 : 0;
	
	// read data
	for (t=0;t<length;t++) {
		message->data[t] = spi_putc(0xff);
	}
	SET(MCP2515_CS);
	
	// clear interrupt flag
	if (bit_is_set(status, 6)) {
		mcp2515_bit_modify(CANINTF, (1<<RX0IF), 0);
	}
	else {
		mcp2515_bit_modify(CANINTF, (1<<RX1IF), 0);
	}
	
	//return (status & 0x07) + 1;
}

// ----------------------------------------------------------------------------

uint8_t mcp2515_send_message(tCAN *message)
{
	uint8_t status = mcp2515_read_status(SPI_READ_STATUS);
	
	/* Statusbyte:
	 *
	 * Bit	Function
	 *  2	TXB0CNTRL.TXREQ
	 *  4	TXB1CNTRL.TXREQ
	 *  6	TXB2CNTRL.TXREQ
	 */
	uint8_t address;
	uint8_t t;
	
	if (bit_is_clear(status, 2)) {
		address = 0x00;
	}
	else if (bit_is_clear(status, 4)) {
		address = 0x02;
	} 
	else if (bit_is_clear(status, 6)) {
		address = 0x04;
	}
	else {
		// all buffer used => could not send message
		return 0;
	}
	
	RESET(MCP2515_CS);
	spi_putc(SPI_WRITE_TX | address);
	
	spi_putc(message->id >> 3);
    spi_putc(message->id << 5);
	
	spi_putc(0);
	spi_putc(0);
	
	uint8_t length = message->header.length & 0x0f;
	
	if (message->header.rtr) {
		// a rtr-frame has a length, but contains no data
		spi_putc((1<<RTR) | length);
	}
	else {
		// set message length
		spi_putc(length);
		
		// data
		for (t=0;t<length;t++) {
			spi_putc(message->data[t]);
		}
	}
	SET(MCP2515_CS);
	
	_delay_us(1);
	
	// send message
	RESET(MCP2515_CS);
	address = (address == 0) ? 1 : address;
	spi_putc(SPI_RTS | address);
	SET(MCP2515_CS);
	
	return 1;
}
