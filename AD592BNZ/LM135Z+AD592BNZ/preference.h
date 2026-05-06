
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

#include "lcd.h"

#define rele_VOUT_ADC     PORTA &= 0b11011111; //PA5
#define rele_VOUT_F       PORTA |= 0b00100000; //PA5

#define LED1_ON           PORTD |= 0b00000001; // включить светодиод 1 (PD0)
#define LED1_OFF          PORTD &= 0b11111110; // выключить светодиод 1 (PD0)

#define LED2_ON           PORTD |= 0b00000010;     // включить светодиод 2 (PD1)
#define LED2_OFF          PORTD &= 0b11111101;     // выключить светодиод 2

#define Ready_ON		  PORTA |= 0b01000000;     // пин PA6 знак готовности принимать данные от формулы (лог.1)
#define Ready_OFF		  PORTA &= 0b10111111;     // пин PA6 знак загруженности принимать данные от формулы (лог.0)

#endif /* PREFERENCE_H_ */