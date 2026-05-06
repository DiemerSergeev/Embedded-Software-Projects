
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
#include "uart.h"


#define rele_OVER_UNDER_ON    PORTA|=0b01000000;     // переключить SDA,SCL на формулу (PA6)
#define rele_OVER_UNDER_OFF   PORTA&=0b10111111;     // переключить SDA,SCL на МК

#define rele_VPTAT_ON      PORTA|=0b00010000;     // переключить A0A1 на формулу (PA4)
#define rele_VPTAT_OFF     PORTA&=0b11101111;     // переключить A0A1 на GND

#define Ready_ON		  PORTA|=0b00000001;     // пин PA0 знак готовности принимать данные от формулы (лог.1)
#define Ready_OFF		  PORTA&=0b11111110;     // пин PA0 знак загруженности принимать данные от формулы (лог.0)

#define LED1_ON           PORTA|=0b00001000;     // включить светодиод 1 (PA3)
#define LED1_OFF          PORTA&=0b11110111;     // выключить светодиод 1

#define LED2_ON           PORTA|=0b00000010;     // включить светодиод 2 (PA1)
#define LED2_OFF          PORTA&=0b11111101;     // выключить светодиод 2


#endif /* PREFERENCE_H_ */