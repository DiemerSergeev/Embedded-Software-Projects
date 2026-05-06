#include "uart.h"

#define F_CPU 8000000UL
#include "i2c.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <inttypes.h>



#include <stdio.h>
#include <stdlib.h>
#include <avr/common.h>



//#define DS_NOTFND		 PORTB|=0b01000000;    // ставим PB6 как ошибку


#define ERROR_LED_ON	PORTF|=0b00001000;
#define ERROR_LED_OFF	PORTF&=0b11110111;

#define OK_LED_ON	PORTF|=0b00000100;
#define OK_LED_OFF	PORTF&=0b11111011;


#define BOOT_LED_ON  PORTB|=0b10000000;
#define BOOT_LED_OFF  PORTB&=0b01111111;

#define ERROR_TO_FORMULA  PORTB&=0b10011111;

#define DATA_1_H  PORTB|= 0b00100000;   // data1
#define DATA_1_L  PORTB&= 0b11011111;   // data1
#define DATA_2_H  PORTB|= 0b01000000;   // data2
#define DATA_2_L  PORTB&= 0b10111111;   // data2


#define Ready_ON   PORTB|= 0b01000000;   // готовность по ноге data2   //pb6
//#define TEST_3_OK  PORTB|= 0b0110000;