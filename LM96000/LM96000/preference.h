
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
#define rele_SDA_SCL_ON 	  PORTC|=0b00100000;     // переключить на формулу (PC5)
#define rele_SDA_SCL_OFF	  PORTC&=0b11011111;     // переключить на МК

#define rele_TACH1_TACH2_ON   PORTC|=0b00001000;     // переключить на формулу (PC3)
#define rele_TACH1_TACH2_OFF  PORTC&=0b11110111;     // переключить на MK, подтяжка не запаяна

#define rele_TACH3_TACH4_ON   PORTC|=0b00010000;     // переключить на формулу (PC4)
#define rele_TACH3_TACH4_OFF  PORTC&=0b11101111;     // переключить на MK, подтяжка не запаяна

#define rele_PWM1_ON 	      PORTC|=0b00000001;     // переключить на SMA (PC0)
#define rele_PWM1_OFF	      PORTC&=0b11111110;     // переключить на разрыв

#define rele_PWM2_ON 	      PORTC|=0b00000010;     // переключить на SMA (PC1)
#define rele_PWM2_OFF	      PORTC&=0b11111101;     // переключить на разрыв

#define rele_PWM3_ON 	      PORTC|=0b00000100;     // переключить на SMA (PC2)
#define rele_PWM3_OFF	      PORTC&=0b11111011;     // переключить на разрыв

/************************************************************************/
/* VID                                                                  */
/************************************************************************/

#define VID0_HIGH 			  PORTA|=0b00100000;     // выставить VID0 в 1 (PA5)
#define VID0_LOW			  PORTA&=0b11011111;     // выставить VID0 в 0

#define VID1_HIGH 			  PORTA|=0b00010000;     // выставить VID1 в 1 (PA4)
#define VID1_LOW			  PORTA&=0b11101111;     // выставить VID1 в 0

#define VID2_HIGH 			  PORTA|=0b00001000;     // выставить VID2 в 1 (PA3)
#define VID2_LOW			  PORTA&=0b11110111;     // выставить VID2 в 0

#define VID3_HIGH 			  PORTA|=0b00000100;     // выставить VID3 в 1 (PA2)
#define VID3_LOW			  PORTA&=0b11111011;     // выставить VID3 в 0

#define VID4_HIGH 			  PORTA|=0b00000010;     // выставить VID4 в 1 (PA1)
#define VID4_LOW			  PORTA&=0b11111101;     // выставить VID4 в 0

/************************************************************************/
/* LEDS                                                                 */
/************************************************************************/

#define LED_READ_ON           PORTA|=0b10000000;     // включить светодиод READ (PA7)
#define LED_READ_OFF		  PORTA&=0b01111111;     // выключить светодиод READ

#define LED_WRITE_ON          PORTA|=0b01000000;     // включить светодиод WRITE (PA6)
#define LED_WRITE_OFF         PORTA&=0b10111111;     // выключить светодиод WRITE

/************************************************************************/
/* READY                                                                */
/************************************************************************/

#define Ready_ON			  PORTA|=0b00000001;     // пин PA0 знак готовности принимать данные от формулы (лог.1)
#define Ready_OFF		      PORTA&=0b11111110;     // пин PA0 знак загруженности принимать данные от формулы (лог.0)

#endif /* PREFERENCE_H_ */