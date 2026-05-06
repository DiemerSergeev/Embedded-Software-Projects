#include "lcd.h"

//----------------------------------------
void sendhalfbyte(unsigned char c)
{
	c<<=4;
	c = (c & 0xF0) >> 4 | (c & 0x0F) << 4;
	c = (c & 0xCC) >> 2 | (c & 0x33) << 2;
	c = (c & 0xAA) >> 1 | (c & 0x55) << 1;
	
	e1; //включаем линию Е
	_delay_ms(2);
	PORTB &= 0b11110000; //стираем информацию на входах PB0-PB3, остальное не трогаем
	PORTB |= c;
	e0; //выключаем линию Е
	_delay_ms(2);
}
//----------------------------------------
void sendbyte(unsigned char c, unsigned char mode)
{
	if (mode==0) rs0
	else rs1;
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
void setpos(unsigned char x, unsigned y)
{
	char adress;
	adress=(0x40*y+x)|0b10000000;
	sendbyte(adress, 0);
}
//----------------------------------------
void LCD_init(void)
{
	//DDRC|=0b00001111; // PC0,1,2,3 на выход
	DDRB|=0b00001111; // PB1,2,3,5,6,7 на выход
	_delay_ms(10);
	RW                // RW к 0
	_delay_ms(30);    // Ждем 15 мс (стр 45)
	sendhalfbyte(0b00000011);
	_delay_ms(10);
	sendhalfbyte(0b00000011);
	_delay_us(150);
	sendhalfbyte(0b00000011);
	_delay_ms(10);
	sendhalfbyte(0b00000010);
	_delay_ms(10);
	sendbyte(0b00101000, 0); //4 битный-режим (DL=0) и 2 линии (N=1)
	_delay_ms(10);
	sendbyte(0b00001100, 0); //включаем изображение на дисплее (D=1), курсоры никакие не включаем (C=0, B=0),нет необходимости
	_delay_ms(10);
	sendbyte(0b00000110, 0); //курсор (хоть он у нас и невидимый) будет двигаться влево
	_delay_ms(10);
}
//----------------------------------------
void clearlcd(void)
{
	sendbyte(0b00000001, 0);
	_delay_ms(1);
}
//----------------------------------------
void str_lcd (char str1[])
{
	wchar_t n;
	for(n=0;str1[n]!='\0';n++)
	sendchar(str1[n]);
}
//----------------------------------------
