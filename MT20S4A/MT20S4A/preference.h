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

#define rele_1_ON    PORTA|=0b00100000;     // переключить DB0,DB1 на формулу (PA5)
#define rele_1_OFF   PORTA&=0b11011111;     // переключить DB0,DB1 на МК

#define rele_2_ON    PORTA|=0b01000000;     // переключить DB2,DB3 на формулу (PA6)
#define rele_2_OFF   PORTA&=0b10111111;     // переключить DB2,DB3 на МК

#define rele_3_ON    PORTA|=0b00010000;     // переключить DB4,DB5 на формулу (PA4)
#define rele_3_OFF   PORTA&=0b11101111;     // переключить DB4,DB5 на МК

#define rele_4_ON    PORTA|=0b00000100;     // переключить DB6,DB7 на формулу (PA2)
#define rele_4_OFF   PORTA&=0b11111011;     // переключить DB6,DB7 на МК

#define rele_5_ON    PORTA|=0b10000000;     // переключить DB6,DB7 на формулу (PA7)
#define rele_5_OFF   PORTA&=0b01111111;     // переключить DB6,DB7 на МК

#define Ready_ON	 PORTA|=0b00000001;     // пин DI1 знак готовности принимать данные от формулы (лог.1)
#define Ready_OFF    PORTA&=0b11111110;     // пин DI1 знак загруженности принимать данные от формулы (лог.0)

#define LED1_ON      PORTA|=0b00001000;     // включить светодиод 1 (PA3)
#define LED1_OFF     PORTA&=0b11110111;     // выключить светодиод 1

#define LED2_ON      PORTA|=0b00000010;     // включить светодиод 1 (PA1)
#define LED2_OFF     PORTA&=0b11111101;     // выключить светодиод 1

#endif /* PREFERENCE_H_ */