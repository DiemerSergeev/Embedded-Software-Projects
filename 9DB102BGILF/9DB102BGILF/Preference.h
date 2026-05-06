#ifndef PREFERENCE_H_
#define PREFERENCE_H_

#define F_CPU 8000000UL

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/common.h>

/*******************************************************/
#include "lcd0802.h"
char buffer_LCD [32];			 // для вывода на дисплей

/*******************************************************/
#define READ_ON		 PORTC|=0b00001000;      // включить светодиод (PC3)
#define READ_OFF	 PORTC&=0b11110111;      // выключить светодиод

#define WRITE_ON	 PORTC|=0b00100000;      // включить светодиод (PC5)
#define WRITE_OFF	 PORTC&=0b11011111;      // выключить светодиод

/*******************************************************/
#define rele_1_ON	 PORTA |= 0b00000100; //(PA2)
#define rele_1_OFF	 PORTA &= 0b11111011;

#define rele_2_ON	 PORTA |= 0b00010000; //(PA4)
#define rele_2_OFF	 PORTA &= 0b11101111;

#define rele_3_ON	 PORTA |= 0b00001000; //(PA3)
#define rele_3_OFF	 PORTA &= 0b11110111;

#define rele_4_ON	 PORTA |= 0b00100000; //(PA5)
#define rele_4_OFF	 PORTA &= 0b11011111;

#define rele_5_ON	 PORTA |= 0b00000010; //(PA1)
#define rele_5_OFF	 PORTA &= 0b11111101;

#define rele_6_ON	 PORTA |= 0b01000000; //(PA6)
#define rele_6_OFF	 PORTA &= 0b10111111;

/*******************************************************/
#define Ready_ON	 PORTA |= 0b00000001;   // включить сигнал готовности МК (PA0)
#define Ready_OFF	 PORTA &= 0b11111110;   // выключить сигнал готовности МК (PA0)

#endif /* PREFERENCE_H_ */