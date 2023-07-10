/**************************************************************************************************\
 * Abfragen eines Tasters an Port D, Pin 2 mit Entprellung �ber Timer 0. Erfordert zyklischen
 * Aufruf der Funktion CheckTaster() um interne Zustandswechsel auszuf�hren (Polling des Tasters).
 * H�chstens jeder zweite Aufruf von CheckTaster() erkennt einen (erneut) gedr�ckten Taster.
 *
 * Autor:   Prof. Dr. Axel Thuemmler
 * Datum:   27. Maerz 2023
 * Version: 1.1
 *
 * Diese Software ist urheberrechtlich geschuetzt. Eine Vervielfaeltigung oder Verbreitung,
 * auch ausschnittsweise, ist nicht ohne die Zustimmung des Autors erlaubt.
\**************************************************************************************************/
#ifndef _TASTER_H_
#define _TASTER_H_

/* Port D, Pin 2 f�r Tastereingang initialisieren. */
void Taster_Init();

/* Gibt 1 zur�ck bei steigender Flanke des Tastersignals, sonst 0. */
unsigned char Taster_CheckPressed();

#endif /* _TASTER_H_ */
