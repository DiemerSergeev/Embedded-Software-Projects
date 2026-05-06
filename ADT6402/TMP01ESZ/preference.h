
#ifndef PREFERENCE_H_
#define PREFERENCE_H_

#define F_CPU 8000000UL

char buffer_LCD [32];			 // для вывода на дисплей

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/common.h>

#include "lcd.h"
#include "uart.h"

#define rele_TOVER_ON     PORTA |= 0b00000100;     // подтяжка TOVER к VDD для ADT6401 (PA2)
#define rele_TOVER_OFF    PORTA &= 0b11111011;     

#define Ready_ON		  PORTA |= 0b00000001;     // пин PA0 знак готовности принимать данные от формулы (лог.1)
#define Ready_OFF		  PORTA &= 0b11111110;     // пин PA0 знак загруженности принимать данные от формулы (лог.0)

#define LED_READ_ON       PORTC |= 0b00001000;     // включить светодиод (PC3)
#define LED_READ_OFF      PORTC &= 0b11110111;     // выключить светодиод

#define LED_WRITE_ON      PORTC |= 0b00100000;     // включить светодиод (PC5)
#define LED_WRITE_OFF     PORTC &= 0b11011111;     // выключить светодиод

#define LED_ERROR_ON      PORTC |= 0b10000000;     // включить светодиод (PC7)
#define LED_ERROR_OFF     PORTC &= 0b01111111;     // выключить светодиод


#endif /* PREFERENCE_H_ */