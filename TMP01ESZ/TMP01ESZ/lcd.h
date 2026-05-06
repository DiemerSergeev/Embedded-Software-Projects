#ifndef LCD_H_
#define LCD_H_

#include "preference.h"

//----------------------------------------
void LCD_init(void);
void setpos(unsigned char x, unsigned y);
void clearlcd(void);
void str_lcd (char str1[]);
//----------------------------------------

//PC4-DB4
//PC5-DB5
//PC6-DB6
//PC7-DB7
#define e1    PORTD|=0b00010000;     // установка линии E в 1 (PD4)
#define e0    PORTD&=0b11101111;    // установка линии E в 0
#define rs1   PORTD|=0b10000000;     // установка линии RS в 1 (данные) (PD7)
#define rs0   PORTD&=0b01111111;    // установка линии RS в 0 (команда)
#define RW    PORTB&=0b11111101;    // установка линии RW в 0 (команда) (PB1)
//----------------------------------------



#endif /* LCD_H_ */