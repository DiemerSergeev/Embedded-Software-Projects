#ifndef LCD_H_
#define LCD_H_

#include "preference.h"

//----------------------------------------
void LCD_init(void);
void setpos(unsigned char x, unsigned y);
void clearlcd(void);
void str_lcd (char str1[]);
void LCD_off(void);
void sendbyte(unsigned char c, unsigned char mode);

//----------------------------------------
#define e1    PORTB|=0b00000100;     // установка линии E в 1
#define e0    PORTB&=0b11111011;    // установка линии E в 0
#define rs1   PORTB|=0b00000001;     // установка линии RS в 1 (данные)
#define rs0   PORTB&=0b11111110;    // установка линии RS в 0 (команда)
#define RW0   PORTB&=0b11111101;    // установка линии RW в 0 
#define RW1   PORTB|=0b00000010;    // установка линии RW в 1 
//----------------------------------------

#endif /* LCD_H_ */