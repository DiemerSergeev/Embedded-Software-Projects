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
	PORTB&=0b11110000; //стираем информацию на входах PB0-PB3, остальное не трогаем
	PORTB|=c;
	e0; //выключаем линию Е
	_delay_ms(2);
}
//----------------------------------------
void sendbyte(unsigned char c, unsigned char mode)
{
	if (mode==0) rs0
	else rs1;
	
	e1; //включаем линию Е
	_delay_ms(2);
	PORTA&=0b00000000;
	PORTA|=c;
	
	e0; //выключаем линию Е
	_delay_ms(2);
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
	DDRA|=0b11111111;   // PA0-PA7 на выход (DB0-DB7)
	DDRB|=0b00000111;   // PB0,1,2 на выход (EN,RW,RS)
	_delay_ms(10);
	RW0;                // RW к 0
	_delay_ms(30);    // Ждем 15 мс (стр 45)
	sendbyte(0b00111000, 0); //8 битный-режим (DL=1) и 2 линии (N=1), 5x8 точек
	_delay_ms(10);
	sendbyte(0b00111000, 0); // еще раз 8 битный-режим (DL=1) и 2 линии (N=1), 5x8 точек
	_delay_ms(10);
	sendbyte(0b00001100, 0); //включаем изображение на дисплее (D=1), курсоры никакие не включаем (C=0, B=0),нет необходимости
	_delay_ms(10);
	sendbyte(0b00000001, 0); //очистка дисплея
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
char array [64]={0x41,0xA0,0x42,0xA1,0xE0,0x45,0xA3,0xA4,0xA5,0xA6,0x4B,0xA7,
	0x4D,0x48,0x4F,0xA8,0x50,0x43,0x54,0xA9,0xAA,0x58,0xE1,0xAB,
	0xAC,0xE2,0xAD,0xAE,0xAD,0xAF,0xB0,0xB1,0x61,0xB2,0xB3,0xB4,
	0xE3,0x65,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0x6F,0xBE,
	0x70,0x63,0xBF,0x79,0xE4,0x78,0xE5,0xC0,0xC1,0xE6,0xC2,0xC3,
0xC4,0xC5,0xC6,0xC7};	

	wchar_t n;
	for(n=0;str1[n]!='\0';n++)
	{
		if (str1[n]>=0xC0)
		str1[n]=array[str1[n]-0xC0];
		
		sendchar(str1[n]);
	}	
}

//----------------------------------------
void LCD_off(void)
{
	RW0;                // RW к 0
	_delay_ms(30);    // Ждем 15 мс (стр 45)
	sendbyte(0b00000001, 0); // clear LCD
	_delay_ms(10);
	//sendbyte(0b00001000, 0); // off LCD
	//_delay_ms(10);
	DDRA=0x00;
	_delay_ms(10);
	DDRB&=0b00001000;
}
//----------------------------------------