#ifndef UART_H_
#define UART_H_

//--------------------------------------------
// Либо это всё определено в "preference.h"
#include "preference.h"
// либо по отдельности
//#define F_CPU 8000000UL // Частота проца

//#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <stdio.h>
//#include <stdlib.h>
//--------------------------------------------

void UART_Init(uint8_t uart_number, unsigned long speed);
void UART_Deinit(void);
void UART_Transmit_Char( unsigned char data);
void UART_Transmit_Str(char *s, uint8_t end_str);
void UART_Transmit_Hex(uint16_t data, int width);
void UART_Transmit_Float(float data, int precision);
void UART_Transmit_Dec(uint16_t data, int width);


uint8_t rx_done (void);
void rx_clear (void);


#define LEN 12                  // Размер кадра
char buffer[LEN];

#define no_end_string          0
#define end_string             1

#endif /* UART_H_ */