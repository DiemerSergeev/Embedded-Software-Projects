#include <avr/interrupt.h>
#include "preference.h"
#include <util/delay.h>


#include "DriverLM57.h"


void SPI_ini(void)
{
	//со стороны МК
	//nCS - PB4  выход (по умл.1)
	//MO - PB5   выход (по умл.0)
	//MI - PB6   вход неподтянутый
	//SCK - PB7  выход (по умл.0)

	DDRB|=  0b10110000;	   // DDRB   SPI
	DDRB&= 0b10111111;	   // PORTB
	PORTB&= 0b00011111;	   // PORTB
	PORTB|= 0b00010000;	   // PORTB
	
	SPCR = (1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(1<<SPR1)|(1<<SPR0); // SPI, MSB first, Master, Mode:1,0, Fosc/128=62khz
	SPSR = (0<<SPI2X); // скорость без множителя
	
	TRIP_T_OFF;  // выключить TRIP_TEST
	DDRC&=0b11111100; // nTover и Tover на выход
	PORTC&=0b11111100;

}


float LM57_read(void)
{
	_delay_ms(10);
	unsigned int b1,b2;
	float result=0;
	
	// Забор данных с MCP 3201 (10 раз)
	for(uint8_t i=0; i<10; i++)
	{
		b1=0;
		b2=0;
		
		nCS0;
		SPDR = 0x00;                // Посылаем команду чтения статус регистра 
		while(!(SPSR & (1<<SPIF)));
		b1=SPDR;
		
		SPDR = 0x00;                // Посылаем команду чтения статус регистра
		while(!(SPSR & (1<<SPIF)));
		b2=SPDR;
		
		b1=(b1<<8)|b2;               // выделяем полезные 12 бит
		b1<<=3;
		b1>>=4;
		result+=((float)b1*2.046)/4096;  // пересчитываем в вольты, опорник 2.046V
		nCS1;
		_delay_ms(10);
	}
	// Пересчитыаем значения

	result=result/10; // усреднение
	
	result=result/0.001; // мв

	
	result=(947.6)-result;   // ниебическая формула пересчёта из документации
	result=result*(-0.00516);
	result=26.687556-result;
	result=sqrt(result);
	result=5.166-result;
	result=result/(-0.00258);
	result=result+30;
	
	return(result);
}