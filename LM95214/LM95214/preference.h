
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
#define rele_SDA_SCL_ON 		PORTA|=0b00000010;     // переключить на формулу (PA1)
#define rele_SDA_SCL_OFF		PORTA&=0b11111101;     // переключить на МК

#define rele_TCRIT1_TCRIT2_ON   PORTA|=0b00000100;     // переключить на формулу (PA2)
#define rele_TCRIT1_TCRIT2_OFF  PORTA&=0b11111011;     // переключить на MK, подтяжка не запаяна

#define rele_TCRIT3_ON			PORTA|=0b00001000;     // переключить на формулу (PA3)
#define rele_TCRIT3_OFF			PORTA&=0b11110111;     // переключить на MK, подтяжка не запаяна

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