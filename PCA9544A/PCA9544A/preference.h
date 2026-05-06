
#ifndef PREFERENCE_H_
#define PREFERENCE_H_

#define F_CPU 8000000UL

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/common.h>

#include "lcd0802.h"

char buffer_LCD [32];			 // для вывода на дисплей

/************************************************************************/
/* RELE                                                                 */
/************************************************************************/

#define rele_SDA_SCL_ON 	  PORTC|=0b00010000;     // переключить на формулу (PC4)
#define rele_SDA_SCL_OFF	  PORTC&=0b11101111;     // переключить на МК

#define rele_SD0_SC0_ON 	  PORTB|=0b00010000;     // переключить на формулу (PB4)
#define rele_SD0_SC0_OFF	  PORTB&=0b11101111;     // переключить на МК

#define rele_SD1_SC1_ON 	  PORTC|=0b00000001;     // переключить на формулу (PC0)
#define rele_SD1_SC1_OFF	  PORTC&=0b11111110;     // переключить на МК

#define rele_SD2_SC2_ON 	  PORTA|=0b10000000;     // переключить на формулу (PA7)
#define rele_SD2_SC2_OFF	  PORTA&=0b01111111;     // переключить на МК

#define rele_SD3_SC3_ON 	  PORTC|=0b01000000;     // переключить на формулу (PC6)
#define rele_SD3_SC3_OFF	  PORTC&=0b10111111;     // переключить на МК

#define rele_INT0_INT1_ON	  PORTB|=0b01000000;     // переключить на формулу (PB6)
#define rele_INT0_INT1_OFF    PORTB&=0b10111111;     // переключить на МК

#define rele_INT2_INT3_ON	  PORTC|=0b00000100;     // переключить на формулу (PC2)
#define rele_INT2_INT3_OFF    PORTC&=0b11111011;     // переключить на МК

/************************************************************************/
/* INT                                                                */
/************************************************************************/

#define INT0_HIGH 			  PORTA|=0b00010000;     // выставить INT0 в 1 (PA4)
#define INT0_LOW			  PORTA&=0b11101111;     // выставить INT0 в 0

#define INT1_HIGH 			  PORTA|=0b00001000;     // выставить INT1 в 1 (PA3)
#define INT1_LOW			  PORTA&=0b11110111;     // выставить INT1 в 0

#define INT2_HIGH 			  PORTA|=0b00000100;     // выставить INT2 в 1 (PA2)
#define INT2_LOW			  PORTA&=0b11111011;     // выставить INT2 в 0

#define INT3_HIGH 			  PORTA|=0b00000010;     // выставить INT3 в 1 (PA1)
#define INT3_LOW			  PORTA&=0b11111101;     // выставить INT3 в 0

/************************************************************************/
/* LEDS                                                                 */
/************************************************************************/

#define LED_READ_ON           PORTC|=0b00001000;     // включить светодиод READ (PC3)
#define LED_READ_OFF		  PORTC&=0b11110111;     // выключить светодиод READ

#define LED_WRITE_ON          PORTC|=0b00100000;     // включить светодиод WRITE (PC5)
#define LED_WRITE_OFF         PORTC&=0b11011111;     // выключить светодиод WRITE

/************************************************************************/
/* READY                                                                */
/************************************************************************/

#define Ready_ON			  PORTA|=0b00000001;     // пин PA0 знак готовности принимать данные от формулы (лог.1)
#define Ready_OFF		      PORTA&=0b11111110;     // пин PA0 знак загруженности принимать данные от формулы (лог.0)

#endif /* PREFERENCE_H_ */