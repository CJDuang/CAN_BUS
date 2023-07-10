/**************************************************************************************************\
 * USART Interface.
 *
 * Autor:   Prof. Dr. Axel Thuemmler
 * Datum:   19. Maerz 2023
 *
 * Diese Software ist urheberrechtlich geschuetzt. Eine Vervielfaeltigung oder Verbreitung,
 * auch ausschnittsweise, ist nicht ohne die Zustimmung des Autors erlaubt.
\**************************************************************************************************/
#ifndef _USART_H_
#define _USART_H_

/*------------------------------------------------------------------------------------------------*/
/* DEFINES                                                                                        */
/*------------------------------------------------------------------------------------------------*/

/* Default baud rate for USART. */
#define USART_DEFAULT_BAUD_RATE 115200

/* Puffergröße des Puffer, der von Printf benötigt wird. */
#define USART_PRINTF_BUFFER_SIZE 100

/* USART error codes. */
#define E_OK 0
#define E_USART_NO_CHAR 1
#define E_USART_PRINTF_BUFFER_OVERFLOW 2

/*------------------------------------------------------------------------------------------------*/
/* FUNCTION PROTOTYPES                                                                            */
/*------------------------------------------------------------------------------------------------*/

/* USART Schnittstelle initialisieren. */
void USART_Init(unsigned long baud);

/* Meldet zurück, ob der USART-Baustein schon initialisiert wurde. */
unsigned char USART_IsInitialized();

/* Ausgabe eines Zeichens über die USART Schnittstelle. */
void USART_PutChar(unsigned char z);

/* Ausgabe einer Zeichenkette über die USART Schnittstelle. */
void USART_PutString(const char text[]);

/* Ausgabe einer vorzeichenlose Ganzzahlen (bis zu 16-Bit) in dezimalen ASCII-Zeichen über die USART Schnittstelle. */
void USART_PutUint16AsDecimalAscii(unsigned int v);

/* Formatierte Textausgabe a la printf (Achtung: Benötigt sehr viel Programm- und Datenspeicher!) . */
unsigned char USART_Printf(const char text[], ...);

/* Einlesen eines Zeichens von der USART Schnittstelle ohne Warten. */
unsigned char USART_GetChar(unsigned char* z);

#endif /* _USART_H_ */
