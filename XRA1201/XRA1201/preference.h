
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

#include "uart.h"

//PB2 - nIRQ

#define rele_SDASCL_ON         PORTA|=0b00000100;     // переключить SDA,SCL на формулу (PA2)
#define rele_SDASCL_OFF        PORTA&=0b11111011;     // переключить SDA,SCL на МК

#define rele_NIRQAD0_ON        PORTA|=0b00010000;     // переключить nIRQ, ADDR0 на формулу (PA4)
#define rele_NIRQAD0_OFF       PORTA&=0b11101111;     // переключить nIRQ, ADDR0 на МК

#define rele_AD1AD2_ON         PORTA|=0b01000000;     // переключить ADDR1, ADDR2 на формулу (PA6)
#define rele_AD1AD2_OFF        PORTA&=0b10111111;     // переключить ADDR1, ADDR2 на МК

#define Ready_ON		       PORTA|=0b00000001;     // пин PA0 знак готовности принимать данные от формулы (лог.1)
#define Ready_OFF		       PORTA&=0b11111110;     // пин PA0 знак загруженности принимать данные от формулы (лог.0)

#define LED1_ON                PORTA|=0b00001000;     // включить светодиод 1 (PA3)
#define LED1_OFF               PORTA&=0b11110111;     // выключить светодиод 1

#define LED2_ON                PORTA|=0b00000010;     // включить светодиод 1 (PA1)
#define LED2_OFF               PORTA&=0b11111101;     // выключить светодиод 1




#endif /* PREFERENCE_H_ */