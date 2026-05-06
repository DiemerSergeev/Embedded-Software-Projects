#ifndef LCD0802_H_
#define LCD0802_H_

#include "preference.h"

//----------------------------------------
void LCD_Init(void);
void LCD_Deinit(void);
void sendhalfbyte(unsigned char c);
void sendchar(unsigned char c);
void LCD_Setpos(unsigned char x, unsigned y);
void LCD_Clear(void);
void LCD_Str(const char str1[]);
//----------------------------------------

//PG0-DB4
//PG1-DB5
//PG2-DB6
//PG3-DB7
//PF7-E
//PF6-RS
//GND-RW
#define lcd_en1   PORTF|=0b10000000;     // установка линии E в 1 (PF7)
#define lcd_en0   PORTF&=0b01111111;     // установка линии E в 0
#define lcd_rs1   PORTF|=0b01000000;     // установка линии RS в 1 (данные) (PF6)
#define lcd_rs0   PORTF&=0b10111111;     // установка линии RS в 0 (команда)
//----------------------------------------



#endif /* LCD0802_H_ */