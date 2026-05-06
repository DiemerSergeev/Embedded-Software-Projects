#ifndef LCD_H_
#define LCD_H_

#include "preference.h"

//----------------------------------------
void LCD_init(void);
void setpos(unsigned char x, unsigned y);
void clearlcd(void);
void str_lcd (char str1[]);
//----------------------------------------
#define e1    PORTA |= 0b00000100; // установка линии E в 1 (PA2)
#define e0    PORTA &= 0b11111011; // установка линии E в 0 (PA2)
#define rs1   PORTA |= 0b00000001; // установка линии RS в 1 (данные) (PA0)
#define rs0   PORTA &= 0b11111110; // установка линии RS в 0 (команда) (PA0)
#define RW    PORTA &= 0b11111101; // установка линии RW в 0 (команда) (PA1)
//----------------------------------------


#endif /* LCD_H_ */