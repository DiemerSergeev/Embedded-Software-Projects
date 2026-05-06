#include "preference.h"
#include <util/delay.h>
#include "TMP01ESZ_Driver.h"

void SPI_ini(void)
{
	//со стороны МК
	//nCS - PB0  выход (по умл.1)
	//MO - PB2   выход (по умл.0)
	//MI - PB3   вход неподтянутый
	//SCK - PB1  выход (по умл.0)

	DDRB|=  0b00000111;	   // DDRB   SPI
	DDRB&=  0b11110111;	   // PORTB
	PORTB|= 0b00000111;	   // PORTB
	
	SPCR = (1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(1<<SPR1)|(1<<SPR0); // SPI, MSB first, Master, Mode:1,0, Fosc/128=62khz
	SPSR = (0<<SPI2X); // скорость без множителя
	
	DDRC|=0b00000101; // OVER и UNDER на выход


}

uint8_t TMP01ESZ_Init(void)
{
	uint8_t temp_data = 0;
	
	SPI_ini();
	
	if(TMP01ESZ_read()>-70) temp_data = 1;
	else temp_data = 0;
	
	return (temp_data);
}

/************************************************************************/
/* Чтение температуры с датчика                                         */
/************************************************************************/
float TMP01ESZ_read(void)
{
	_delay_ms(10);
	unsigned int b1,b2;
	float result=0;
	
	// Забор данных с MCP3201 (10 раз)
	for(uint8_t i=0; i<10; i++)
	{
		b1=0;
		b2=0;
		
		nCS0;
		SPDR = 0x00;                // Посылаем команду чтения статус регистра
		while(!(SPSR & (1<<SPIF)));
		b1 = SPDR;
		
		SPDR = 0x00;                // Посылаем команду чтения статус регистра
		while(!(SPSR & (1<<SPIF)));
		b2 = SPDR;
		
		b1 = (b1<<8)|b2;               // выделяем полезные 12 бит
		b1<<=3;
		b1>>=4;
		result+=((float)b1*2.046)/4096;  // пересчитываем в вольты, опорник 2.046V
		UART_Transmit_Hex (b1);

		nCS1;
		_delay_ms(10);
		
	}
	// Пересчитыаем значения

	result=result/10; // усреднение
	
	result=(result/0.005)-273; // Температура

	return(result);
}