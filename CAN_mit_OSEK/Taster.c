/**************************************************************************************************\
 * Abfragen eines Tasters an Port D, Pin 2 mit Entprellung über Timer 0. Erfordert zyklischen
 * Aufruf der Funktion CheckTaster() um interne Zustandswechsel auszuführen (Polling des Tasters).
 * Höchstens jeder zweite Aufruf von CheckTaster() erkennt einen (erneut) gedrückten Taster.
 *
 * Autor:   Prof. Dr. Axel Thuemmler
 * Datum:   27. Maerz 2023
 * Version: 1.1
 *
 * Diese Software ist urheberrechtlich geschuetzt. Eine Vervielfaeltigung oder Verbreitung,
 * auch ausschnittsweise, ist nicht ohne die Zustimmung des Autors erlaubt.
\**************************************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Taster.h"

/* CPU Taktfrequenz. */
#ifndef F_CPU
#define F_CPU 3686400
#endif

/* Laufzeit für Entprell-Timer. */
#define DEBOUNCE_TIME 0.05

/* 3.6864 MHz = 1 / 3686400 [s] = Dauer pro CPU Taktzyklus.            */
/* Prescaler = 1024 => Timerfrequenz = 3686400/1024 [1/s] = 3600 [1/s] */
#define T0VALUE (uint8_t)(256 - F_CPU / 1024 * DEBOUNCE_TIME + 0.5)

/* Zustände */
#define STATE_TASTER_NICHT_GEDRUECKT            0
#define STATE_TASTER_GEDRUECKT                  1
#define STATE_TASTER_NICHT_GEDRUECKT_VORSTUFE   2

/* Timer IRQ Signalisierung für die Entprellung. */
static volatile unsigned char EntprellTimer = 0;

/* Port D, Pin 2 für Tastereingang initialisieren. */
void Taster_Init()
{
  	DDRD = 0x00;                    /* PORTD = Eingang. */
  	PORTD = 0x04;                   /* PORTD Pin 2 = Pull-Up Widerstand aktivieren. */
}

/* Gibt 1 zurück bei steigender Flanke des Tastersignals, sonst 0. */
unsigned char Taster_CheckPressed()
{
    unsigned char result = 0;
    static unsigned char state = STATE_TASTER_NICHT_GEDRUECKT;
    
    switch (state)
    {
        case STATE_TASTER_NICHT_GEDRUECKT:
            /* Wenn Taster gedrückt (Bit 2 == 0) ... */
            if ((PIND & 0x04) == 0)
            {
                state = STATE_TASTER_GEDRUECKT;
                result = 1;
            }
            break;
        case STATE_TASTER_GEDRUECKT:
            /* Wenn Taster losgelassen (Bit 2 == 1) ... */
            if ((PIND & 0x04) != 0)
            {
                state = STATE_TASTER_NICHT_GEDRUECKT_VORSTUFE;

                /* Timer initialisieren. */
                TCCR0A = 0x00;          /* Timer normal mode. */
                TCCR0B = 0x00;          /* Timer stoppen. */
                TCNT0 = T0VALUE;        /* Timerregister neu laden. */
                TIMSK0 = 1 << TOIE0;    /* Overflow Interrupt einschalten. */
                EntprellTimer = 0;
                TCCR0B = 0x05;          /* Timer starten mit Vorteiler CPU Taktfrequenz / 1024. */
            }
            break;
        case STATE_TASTER_NICHT_GEDRUECKT_VORSTUFE:
            /* Wenn Entprell-Timer abgelaufen ... */
            if (EntprellTimer == 1)
            {
                /* und wenn Taster gedrückt ... */
                if ((PIND & 0x04) == 0)
                {
                    state = STATE_TASTER_GEDRUECKT;
                    result = 1;
                }
                else
                {
                    /* Taster ist nicht gedrückt und kann erneut gedrückt werden. */
                    state = STATE_TASTER_NICHT_GEDRUECKT;
                }
            }
            /* Wenn Entprell-Timer nicht abgelaufen und Taster gedrückt ... */
            else if ((PIND & 0x04) == 0)
            {
                /* Tastendruck nicht melden, denn Taster hat geprellt. */
                state = STATE_TASTER_GEDRUECKT;
            }
            break;
        default:
            break;
    }
    
    return result;
}

/* Interrupt Service Routine (ISR) für Entprell-Timer. */
ISR(TIMER0_OVF_vect)
{
    TCCR0B = 0x00;              /* Timer stoppen. */
    EntprellTimer = 1;
}
