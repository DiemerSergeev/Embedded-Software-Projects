#ifndef LCD_H_
#define LCD_H_

#include "preference.h"

void LCD_init(void);
void setpos(unsigned char x, unsigned y);
void clearlcd(void);
void str_lcd (char str1[]);
void LCD_off(void);
void LCD_on(void);
void sendbyte(unsigned char c);
void WRITE(unsigned char c);

#define ENABLE1      PORTB|=0b00000001;    // установка линии E в 1
#define ENABLE0      PORTB&=0b11111110;    // установка линии E в 0

#define A01		     PORTB|=0b00001000;    // установка линии A0 в 1 (данные)
#define A00          PORTB&=0b11110111;    // установка линии A0 в 0 (команда)

#define READWRITE0   PORTD&=0b11111101;    // установка линии RW в 0 
#define READWRITE1   PORTD|=0b00000010;    // установка линии RW в 1 

#endif /* LCD_H_ */