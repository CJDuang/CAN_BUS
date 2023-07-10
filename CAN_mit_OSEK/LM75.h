/*
 * LM75.h
 *
 * Created: 03.05.2023 15:38:51
 *  Author: Raza
 */ 

#ifndef LM75_H_
#define LM75_H_

#include <inttypes.h>

//---------------------------------------------------------------------------------------------
/* Initialisierung des Temperaturmoduls zur Temperaturmessung */
void LM75_init(void);
//---------------------------------------------------------------------------------------------
/* Auslesen der Temperatur als 11 Bit Festkommazahl mit LSB = 2^(?3) */
uint16_t  ReadTemp(void);
//---------------------------------------------------------------------------------------------

#endif /* LM75_H_ */