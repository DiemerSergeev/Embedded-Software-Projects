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

uint8_t mem_buff1[128];

uint8_t mem_buff2[128];

uint8_t buff_check;

#define LED1_ON				    PORTA|=0b00001000;      // включить светодиод 1 (PA3)
#define LED1_OFF			    PORTA&=0b11110111;      // выключить светодиод 1

#define LED2_ON				    PORTA|=0b00000010;      // включить светодиод 2 (PA1)
#define LED2_OFF				PORTA&=0b11111101;      // выключить светодиод 2

#define rele_1_ON	 PORTD |= 0b00010000;
#define rele_2_ON	 PORTC |= 0b10000000;
#define rele_3_ON	 PORTA |= 0b01000000;
#define rele_4_ON	 PORTA |= 0b00010000;
#define rele_5_ON	 PORTB |= 0b00000010;
#define rele_6_ON	 PORTC |= 0b00001000;
#define rele_7_ON	 PORTD |= 0b10000000;
#define rele_8_ON	 PORTA |= 0b00000100;
#define rele_9_ON	 PORTC |= 0b00100000;

#define rele_1_OFF	 PORTD &= 0b11101111;
#define rele_2_OFF	 PORTC &= 0b01111111;
#define rele_3_OFF	 PORTA &= 0b10111111;
#define rele_4_OFF	 PORTA &= 0b11101111;
#define rele_5_OFF	 PORTB &= 0b11111101;
#define rele_6_OFF	 PORTC &= 0b11110111;
#define rele_7_OFF	 PORTD &= 0b01111111;
#define rele_8_OFF	 PORTA &= 0b11111011;
#define rele_9_OFF	 PORTC &= 0b11011111; 

#define Ready_ON				PORTA|=0b00000001;     // включить сигнал готовности МК (PA6)
#define Ready_OFF				PORTA&=0b11111110;     // выключить сигнал готовности МК (PA6)


#endif /* PREFERENCE_H_ */