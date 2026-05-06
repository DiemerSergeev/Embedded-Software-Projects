
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

#define rele_ADD01_ON 	      PORTC|=0b00000010;     // переключить ALARM на формулу (PC1)
#define rele_ADD01_OFF	      PORTC&=0b11111101;     // выведен на светодиоды

#define rele_ALERT_ON 	      PORTC|=0b00000100;     // переключить SDA и OS на формулу (PC2)
#define rele_ALERT_OFF	      PORTC&=0b11111011;     // SDA на МК, и SDA и OS подтянуты к питанию VCC

#define rele_SDAOS_ON 	      PORTC|=0b00001000;     // переключить SDA и OS на формулу (PC3)
#define rele_SDAOS_OFF	      PORTC&=0b11110111;     // SDA на МК, и SDA и OS подтянуты к питанию VCC

#define Ready_ON			  PORTA|=0b00000001;     // пин PA0 знак готовности принимать данные от формулы (лог.1)
#define Ready_OFF		      PORTA&=0b11111110;     // пин PA0 знак загруженности принимать данные от формулы (лог.0)

#define A1_HIGH 			  PORTD|=0b00010000;     // выставить A1 в 1 (PD4)
#define A1_LOW				  PORTD&=0b11101111;     // выставить A1 в 0

#define A0_HIGH 			  PORTD|=0b00100000;     // выставить A0 в 1 (PD5)
#define A0_LOW				  PORTD&=0b11011111;     // выставить A0 в 0

#define LED_READ_ON           PORTA|=0b00000010;     // включить светодиод READ (PA1)
#define LED_READ_OFF		  PORTA&=0b11111101;     // выключить светодиод READ

#define LED_WRITE_ON          PORTA|=0b00000100;     // включить светодиод WRITE (PA2)
#define LED_WRITE_OFF         PORTA&=0b11111011;     // выключить светодиод WRITE

#endif /* PREFERENCE_H_ */