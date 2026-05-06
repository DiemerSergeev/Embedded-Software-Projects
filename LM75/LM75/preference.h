
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
#include "uart.h"

#define ADDR0_HIGH			  PORTC|=0b10000000;
#define ADDR0_LOW			  PORTC&=0b01111111;

#define ADDR1_HIGH			  PORTA|=0b01000000;
#define ADDR1_LOW			  PORTA&=0b10111111;

#define ADDR2_HIGH   		  PORTA|=0b00010000;
#define ADDR2_LOW			  PORTA&=0b11101111;

#define rele_SDAOS_ON 	      PORTA|=0b00000100;     // переключить SDA и OS на формулу (PA2)
#define rele_SDAOS_OFF	      PORTA&=0b11111011;     // SDA на МК, и SDA и OS подтянуты к питанию VCC

#define Ready_ON			  PORTA|=0b00000001;     // пин PA0 знак готовности принимать данные от формулы (лог.1)
#define Ready_OFF		      PORTA&=0b11111110;     // пин PA0 знак загруженности принимать данные от формулы (лог.0)

#define LED_READ_ON           PORTC|=0b00001000;     // включить светодиод READ (PC3)
#define LED_READ_OFF		  PORTC&=0b11110111;     // выключить светодиод READ

#define LED_WRITE_ON          PORTC|=0b00100000;     // включить светодиод WRITE (PC5)
#define LED_WRITE_OFF         PORTC&=0b11011111;     // выключить светодиод WRITE

#endif /* PREFERENCE_H_ */