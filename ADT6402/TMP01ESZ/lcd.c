#include "lcd.h"

/*
LCD_Init();         // Инициализация дисплея
LCD_Deinit();       // Деинициализация дисплея
LCD_Clear();        // Очистка всего дисплея
LCD_Setpos(0, 0);   // Выбор ячейки 0-7 и номер строки 0-1
LCD_Str("Not Fnd"); // Написать текст

Пример 1:
LCD_Clear();
LCD_Setpos(0, 0);
LCD_Str("String 1");
LCD_Setpos(0, 1);
LCD_Str("String 2");

Пример 2:
char buffer_LCD [32];
float z = 1.5;
LCD_Clear();
LCD_Setpos(0, 0);
LCD_Str("String 1");
LCD_Setpos(0, 1);
sprintf(buffer_LCD,"T=%.1fC",  z);
LCD_Str(buffer_LCD);
*/


//----------------------------------------
void sendhalfbyte(unsigned char c)
{
	lcd_en1; //включаем линию Е
	_delay_ms(2);
	//_delay_us(100);
	PORTG&=0b11110000; //стираем информацию на входах PG0-PG3, остальное не трогаем
	PORTG|=c&0b00001111;
	lcd_en0; //выключаем линию Е
	_delay_ms(2);
	//_delay_us(100);
}
//----------------------------------------
void sendbyte(unsigned char c, unsigned char mode)
{
	if (mode==0) lcd_rs0
	else lcd_rs1;
	unsigned char hc=0;
	hc=c>>4;
	
	sendhalfbyte(hc);
	sendhalfbyte(c);
}
//----------------------------------------
void sendchar(unsigned char c)
{
	sendbyte(c,1);
}
//----------------------------------------
void LCD_Setpos(unsigned char x, unsigned y)
{
	char adress;
	adress=(0x40*y+x)|0b10000000;
	sendbyte(adress, 0);
}
//----------------------------------------

//PG0-DB4
//PG1-DB5
//PG2-DB6
//PG3-DB7
//PF7-E
//PF6-RS
//GND-RW
void LCD_Init(void)
{
	DDRF|=0b11000000;   // PF6,PF7 на выход
	PORTF&=0b00111111;

	DDRG|=0b00001111;
	PORTG&=0b11110000;  // PG0,PG1,PG2,PG3 на выход
	
	_delay_ms(10);
	//_delay_ms(18);     // Ждем 15 мс (стр 45)
	sendhalfbyte(0b00000011);
	_delay_ms(10);
	//_delay_ms(2);
	sendhalfbyte(0b00000011);
	_delay_ms(10);
	//_delay_ms(2);
	sendhalfbyte(0b00000011);
	_delay_ms(10);
	//_delay_ms(2);
	sendhalfbyte(0b00000010);
	_delay_ms(10);
	//_delay_ms(2);
	sendbyte(0b00101000, 0); //4 битный-режим (DL=0) и 2 линии (N=1)
	_delay_ms(10);
	//_delay_ms(2);
	sendbyte(0b00001100, 0); //включаем изображение на дисплее (D=1), курсоры никакие не включаем (C=0, B=0),нет необходимости
	_delay_ms(10);
	//_delay_ms(2);
	sendbyte(0b00000110, 0); //курсор (хоть он у нас и невидимый) будет двигаться влево
	_delay_ms(10);
	//_delay_ms(2);
}

void LCD_Deinit(void)
{
	DDRF&=0b10111111;   // PF6 на вход
	PORTF&=0b10111111;
	lcd_en1;

	DDRG&=0b11110000;
	PORTG&=0b11110000;  // PG0,PG1,PG2,PG3 на вход
}

//----------------------------------------
void LCD_Clear(void)
{
	sendbyte(0b00000001, 0);
	_delay_ms(1);
	//_delay_us(100);
	sendbyte(0b10000000, 0);
	_delay_ms(1);
	//_delay_us(100);
}
//----------------------------------------

void LCD_Str(const char str1[])
{
	if (str1 == NULL) return; // Проверка на NULL
	for (uint8_t i = 0; str1[i] != '\0'; i++)
	{
		sendchar(str1[i]);
	}
}
//----------------------------------------