#ifndef PREFERENCE_H_
#define PREFERENCE_H_

#define F_CPU 16000000UL

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



#define LED3_ON				    PORTA|=0b00000001;      // включить светодиод 3 (PA0)
#define LED3_OFF			    PORTA&=0b11111110;      // выключить светодиод 3

#define LED1_ON				    PORTB|=0b00000001;      // включить светодиод 1 (PB0)
#define LED1_OFF				PORTB&=0b11111110;      // выключить светодиод 1

#define LED2_ON				    PORTB|=0b00000010;      // включить светодиод 2 (PB1)
#define LED2_OFF				PORTB&=0b11111101;      // выключить светодиод 2

#define nCS_OFF					PORTB|=0b00010000;      // CS вверх (PB4)
#define nCS_ON					PORTB&=0b11101111;      // CS вниз

#define nWP_OFF					PORTB|=0b00001000;      // CS вверх (PB3)
#define nWP_ON					PORTB&=0b11110111;      // CS вниз

#define nHOLD_OFF				PORTB|=0b00000100;      // CS вверх (PB2)
#define nHOLD_ON				PORTB&=0b11111011;      // CS вниз

//RELE 1
#define rele_CSSCK_ON			PORTA|=0b00001000;     // переключить CS,SCK на формулу (PA3)
#define rele_CSSCK_OFF			PORTA&=0b11110111;     // переключить CS,SCK на МК
//RELE 2	
#define rele_MI_ON				PORTA|=0b00010000;     // переключить MI на формулу (PA4)
#define rele_MI_OFF				PORTA&=0b11101111;     // переключить MI на МК
//RELE 3
#define rele_MO_ON				PORTA|=0b00100000;     // переключить MO на формулу (PA5)
#define rele_MO_OFF				PORTA&=0b11011111;     // переключить MO на МК
//RELE 4
#define rele_nWPnHOLD_ON        PORTA|=0b00000100;	   // переключить nWP,nHOLD на формулу (PA2)
#define rele_nWPnHOLD_OFF       PORTA&=0b11111011;     // переключить nWP,nHOLD на МК

//#define RST_ON					PORTB&=0b11110111;     // включить сброс памяти (PB3)
//#define RST_OFF					PORTB|=0b00001000;     // выключить сброс памяти

#define Ready_OFF				PORTA&=0b10111111;     // выключить сигнал готовности МК (PA6)
#define Ready_ON				PORTA|=0b01000000;     // включить сигнал готовности МК (PA6)

#endif /* PREFERENCE_H_ */