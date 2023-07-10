/**************************************************************************************************\
 * CAN Knoten für Temperaturmessung
 * Realisierung mit OSEK Betriebssystem.
 *
 * Autor: Prof. Dr. Axel Thuemmler 
 * Datum: 15.3.2023
\**************************************************************************************************/

#include <avr/io.h>
#include "Os.h"
#include "Os_Cfg.h"
#include "Taster.h"
#include "Usart.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
#include <compat/twi.h>  // Hier stehen Definitionen der Register
#include <ctype.h>
#include <stddef.h>

#include "mcp2515.h"
#include "global.h"
#include "defaults.h"
#include "mcp2515_defs.h"

#include "LM75.h"
#include "TWI.h"

/*------------------------------------------------------------------------------------------------*/
/* DEFINES                                                                                        */
/*------------------------------------------------------------------------------------------------*/

#define CANSPEED_125 	7		/* CAN speed at 125 kbps  */
#define CANSPEED_250  	3		/* CAN speed at 250 kbps  */
#define CANSPEED_500	1		/* CAN speed at 500 kbps  */
#define CANSPEED_1000	0		/* CAN speed at 1000 kbps */

#define MESSAGE_TASTER_ID 0x80
#define MESSAGE_TEMPERATUR_ID 0x90
#define MESSAGE_STATUS_LED_ID 0x100

/*------------------------------------------------------------------------------------------------*/
/* TASK FUNCTIONS                                                                                 */
/*------------------------------------------------------------------------------------------------*/

/* Idle-Task */
TASK(IdleTask)
{
    /* Idle-Task sollte sich nicht beenden sonst wird das OS beendet, gleichbedeutend mit ShutdownOS().  */
    for (;;);
    TerminateTask();
}

TASK(StartUpTask)
{
    USART_Init(115200);
	USART_PutString("StartupTask aufgerufen.\n");

	TWI_init();                                   /* TWI initialisieren */
	LM75_init();								  /* LM75 initialisieren */
	mcp2515_init(CANSPEED_125);					  /* MCP2515 initialisieren */

    SetAbsAlarm(Alarm1, 1, 1);                    /* Alarm fuer Task 1 initialisieren. */
	
    TerminateTask();
}

TASK(Task1)
{
	/*====================== Todo ========================*/
	/* 
	   - Auf Empfang einer Nachricht überprüfen
	   - Falls eine Nachricht empfangen wurde, überprüfen, ob dies eine Taster-Nachricht ist
	   - Falls die Taste-Nachricht empfangen wurde, überprüfen um welchen Taster es sich hier genau handelt
	   - und die entsprechende Botschaft für den LED-Status versenden. 
	*/
	
	static uint8_t zustand_messung = 0;
	tCAN message_received;
		
	//USART_PutString("1.Task aufgerufen.\n");
	if(mcp2515_check_message())													/* auf Empfang eine Nachricht prüfen */
	{
		mcp2515_get_message(&message_received);									/* Nachricht zwischenspeichern */
		if(message_received.id == MESSAGE_TASTER_ID)							/* Taster-Nachricht gesendet? */
		{	
			if(message_received.data[0] == 1 && zustand_messung == 0)			/* Messung gestartet */
			{
				tCAN message_status_led = { MESSAGE_STATUS_LED_ID, {0, 1}, {1} };

				SetRelAlarm(Alarm2, 0, 10);

				mcp2515_send_message(&message_status_led);						/* Status LED umschalten*/
				zustand_messung = 1;											/* den Zustand 'Messung gestartet' merken */
			}
			if(message_received.data[0] == 0 && zustand_messung == 1)
			{
				tCAN message_status_led = { MESSAGE_STATUS_LED_ID, {0, 1}, {0} };
				tCAN message_temperatur = { MESSAGE_TEMPERATUR_ID, {0, 2}, {0, 0} };

				CancelAlarm(Alarm2);

				mcp2515_send_message(&message_temperatur);
				mcp2515_send_message(&message_status_led);						/* Status LED umschalten*/
				zustand_messung = 0;											/* den Zustand 'Messung beendet' merken */
			}
		}
	}
	/*====================================================*/
	
	TerminateTask();
}

TASK(Task2)
{
	/*====================== Todo ========================*/
	/* 
	- Temperatur per TWI auslesen 
	- und auf dem CAN-Bus passend versenden
	*/
	tCAN message_temperatur = { MESSAGE_TEMPERATUR_ID, {0, 2}, {0, 0} };
	
	//USART_PutString("2.Task wird aufgerufen.\n");

	uint16_t temperatur = ReadTemp();
	message_temperatur.data[0] = temperatur % 256;								/* Intel-Byte-Order: Erst Low-Byte, dann */
	message_temperatur.data[1] = temperatur / 256;								/* High-Byte mit 3 Bits. */
	
	mcp2515_send_message(&message_temperatur);
	/*====================================================*/
	
    TerminateTask();
}

/*------------------------------------------------------------------------------------------------*/
/* MAIN FUNCTION                                                                                  */
/*------------------------------------------------------------------------------------------------*/

int main(void)
{
    StartOS(OSDEFAULTAPPMODE);
}
