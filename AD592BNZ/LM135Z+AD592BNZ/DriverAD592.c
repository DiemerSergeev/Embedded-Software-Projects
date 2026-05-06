#include <avr/interrupt.h>
#include "preference.h"
#include <util/delay.h>


#include "DriverAD592.h"


void SPI_ini(void)
{
	//со стороны МК
	//nCS - PB4  выход (по умл.1)
	//MO - PB5   выход (по умл.0)
	//MI - PB6   вход неподтянутый
	//SCK - PB7  выход (по умл.0)

	DDRB  |= 0b10110000;	   // DDRB   SPI
	DDRB  &= 0b10111111;	   // PORTB
	PORTB &= 0b00011111;	   // PORTB
	PORTB |= 0b00010000;	   // PORTB
	
	SPCR = (1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(1<<SPR0)|(0<<SPR1); // SPI, MSB first, Master, Mode:1,0, Fosc/128=62khz
	SPSR = (0<<SPI2X); // скорость без множителя
	
}

float expRunningAverageAdaptive(float newVal) 
{
	static float filVal = 0;
	float k;
	
	if (abs(newVal - filVal) > 1.5) k = 0.9;
	else k = 0.03;
	
	filVal += (newVal - filVal) * k;
	return filVal;
}

const float _err_measure = 0.8;
const float _q = 0.1;
	
float simpleKalman(float newVal) 
{
	
	float _kalman_gain, _current_estimate;
	static float _err_estimate = _err_measure;
	static float _last_estimate;
	
	_kalman_gain = (float)_err_estimate / (_err_estimate + _err_measure);
	_current_estimate = _last_estimate + (float)_kalman_gain * (newVal - _last_estimate);
	_err_estimate =  (1.0 - _kalman_gain) * _err_estimate + fabs(_last_estimate - _current_estimate) * _q;
	_last_estimate = _current_estimate;
	
	return _current_estimate;
}

float AD592_read(void)
{
	unsigned int b1,b2;
	float result = 0;
	int result2 = 0;
	int cycle = 16;
	_delay_ms(50);
	// Забор данных с MCP 3201 (10 раз)
	for(uint8_t i=0; i<cycle; i++)
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

		nCS1;
		
		b1 = (b1<<8)|b2;               // выделяем полезные 12 бит
		b1<<=3;
		b1>>=4;
		//b1 &= 0x0FFF;
		
		result2 = (int)b1;
		result2 = expRunningAverageAdaptive(result2);
		result2 = simpleKalman(result2);
		
		//result += (int)b1;  // пересчитываем в вольты, опорник 4.096V
		result += result2;  // пересчитываем в вольты, опорник 4.096V
		
		_delay_ms(10);

	}
	// Пересчитыаем значения
	result = result/cycle; // усреднение
	

	
	result = result-2950;  // сравнение с напряжением калибровки
	result = result/10;    // 10mV - 1K
	result = result+25;    // искомая температура относительно калибровки
	
	return(result);
}

