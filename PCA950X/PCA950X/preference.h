
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

#define rele_SDA_SCL_ON 	  PORTC|=0b00000100;     // переключить на формулу (PC2)
#define rele_SDA_SCL_OFF	  PORTC&=0b11111011;     // переключить на МК

#define rele_VDD_RESET_ON	  PORTC|=0b00010000;     // переключить на формулу (PC4)
#define rele_VDD_RESET_OFF    PORTC&=0b11101111;     // переключить на MK, подтяжка не запаяна

#define rele_A0_A1_ON		  PORTC|=0b01000000;     // переключить на формулу (PC6)
#define rele_A0_A1_OFF        PORTC&=0b10111111;     // переключить на MK, подтяжка не запаяна

#define rele_A2_OE_ON 	      PORTA|=0b10000000;     // переключить на SMA (PA7)
#define rele_A2_OE_OFF	      PORTA&=0b01111111;     // переключить на разрыв

/************************************************************************/
/* ADDRESS                                                              */
/************************************************************************/

#define A0_HIGH 			  PORTC|=0b00000010;     // выставить в 1 (PC2)
#define A0_LOW				  PORTC&=0b11111101;     // выставить в 0

#define A1_HIGH 			  PORTC|=0b00001000;     // выставить в 1 (PC3)
#define A1_LOW			      PORTC&=0b11110111;     // выставить в 0

#define A2_HIGH 			  PORTC|=0b00100000;     // выставить в 1 (PC5)
#define A2_LOW			      PORTC&=0b11011111;     // выставить в 0

#define OE_HIGH 			  PORTC|=0b10000000;     // выставить VID3 в 1 (PC7)
#define OE_LOW			      PORTC&=0b01111111;     // выставить VID3 в 0

/************************************************************************/
/* LEDS                                                                 */
/************************************************************************/

#define LED_READ_ON           PORTA|=0b00010000;     // включить светодиод READ (PA4)
#define LED_READ_OFF		  PORTA&=0b11101111;     // выключить светодиод READ

#define LED_WRITE_ON          PORTA|=0b00000100;     // включить светодиод WRITE (PA2)
#define LED_WRITE_OFF         PORTA&=0b11111011;     // выключить светодиод WRITE

/************************************************************************/
/* READY                                                                */
/************************************************************************/

#define Ready_ON			  PORTA|=0b00000001;     // пин PA0 знак готовности принимать данные от формулы (лог.1)
#define Ready_OFF		      PORTA&=0b11111110;     // пин PA0 знак загруженности принимать данные от формулы (лог.0)

#endif /* PREFERENCE_H_ */