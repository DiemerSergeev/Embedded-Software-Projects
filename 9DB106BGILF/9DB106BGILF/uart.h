#ifndef USART_H_
#define USART_H_

#include "Preference.h"

void UART_init(uint16_t speed);
void UART_Transmit_Char( unsigned char data);
void UART_Transmit_Str(char *s, uint8_t end_str);
void UART_Transmit_Hex (uint16_t data);
void UART_Transmit_Float (float data);
void UART_Transmit_Dec (uint16_t data);


uint8_t rx_done (void);
void rx_clear (void);


#define LEN 12
char buffer[LEN];

#define no_end_string          0
#define end_string             1

#endif /* USART_H_ */