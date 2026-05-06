#include "preference.h"
#include "PCA9544A_Driver.h"
#include <util/delay.h>

/*******************************************************/
void I2C_INIT(void)
{
	DDRD|=0b00000011;				// SDA(PD1), SCL(PD0) на выход
									// SCL = CLK/(16+2*TWBR*TWSR) = 8000000/(16+2*32*1)
	TWSR &= (0<<TWPS1)|(0<<TWPS0);	// Биты предделителя настроены на 1
	TWBR = 0x20;						// Настраиваем частоту шины на 100 кГц
}
/*******************************************************/
uint8_t I2C_COMAND(uint8_t action)
{
	switch(action)
	{
		case I2C_START:
		case I2C_RESTART:
		TWCR = _BV(TWSTA) | _BV(TWEN) | _BV(TWINT);// Если нужно прерывание | _BV(TWIE);
		break;
		case I2C_STOP:
		TWCR = _BV(TWSTO) | _BV(TWEN) | _BV(TWINT);// | _BV(TWIE);
		break;
		case I2C_TRANSMIT:
		TWCR = _BV(TWEN) | _BV(TWINT);// | _BV(TWIE);
		break;
		case I2C_RECEIVE_ACK:
		TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA);//| _BV(TWIE);
		break;
		case I2C_RECEIVE_NACK:
		TWCR = _BV(TWEN) | _BV(TWINT);// | _BV(TWIE);
		break;
	}
	if(action != I2C_STOP)while (!(TWCR & _BV(TWINT)));
	return (TWSR & 0xF8);
}
/*******************************************************/
uint8_t READ_ONE_BYTE(void)
{
	uint8_t data = 0x00;
////////////////////////////////////////////////
	LED_WRITE_OFF;
	LED_READ_ON;
////////////////////////////////////////////////
	I2C_COMAND(I2C_START);
////////////////////////////////////////////////
	TWDR = READ_CMD;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////////////////////////////
	I2C_COMAND(I2C_RECEIVE_NACK);
	data = TWDR;
////////////////////////////////////////////////
	I2C_COMAND(I2C_STOP);
////////////////////////////////////////////////	
	LED_WRITE_OFF;
	LED_READ_OFF;
////////////////////////////////////////////////
	return data;
}
/*******************************************************/
void WRITE_ONE_BYTE(uint8_t data)
{
	LED_WRITE_ON;
	LED_READ_OFF;
////////////////////////////////////////////////
	I2C_COMAND(I2C_START);
////////////////////////////////////////////////
	TWDR = WRITE_CMD;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////////////////////////////
	TWDR = data;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////////////////////////////
	I2C_COMAND(I2C_STOP);
////////////////////////////////////////////////
	LED_WRITE_OFF;
	LED_READ_OFF;
}
/*******************************************************/
void PCA9544A_CHECK(void)
{
	uint8_t data = 0x00;
////////////////////////////////////////////////
	LED_WRITE_ON;
	LED_READ_ON;
	_delay_ms(100);
////////////////////////////////////////////////
    WRITE_ONE_BYTE(NOT_ENABLE);
	_delay_ms(10);
////////////////////////////////////////////////	
	data = READ_ONE_BYTE();
	if (data == NOT_ENABLE);
	else  Problem(NOT_ENABLE, data);
	_delay_ms(10);
////////////////////////////////////////////////
	WRITE_ONE_BYTE(CH0_ENABLE);
	_delay_ms(10);
////////////////////////////////////////////////
	data = READ_ONE_BYTE();
	if (data == CH0_ENABLE);
	else  Problem(CH0_ENABLE, data);
	_delay_ms(10);
////////////////////////////////////////////////
	WRITE_ONE_BYTE(CH1_ENABLE);
	_delay_ms(10);
////////////////////////////////////////////////
	data = READ_ONE_BYTE();
	if (data == CH1_ENABLE);
	else  Problem(CH1_ENABLE, data);
	_delay_ms(10);
////////////////////////////////////////////////
	WRITE_ONE_BYTE(CH2_ENABLE);
	_delay_ms(10);
////////////////////////////////////////////////
	data = READ_ONE_BYTE();
	if (data == CH2_ENABLE);
	else  Problem(CH2_ENABLE, data);
	_delay_ms(10);
////////////////////////////////////////////////
	WRITE_ONE_BYTE(CH3_ENABLE);
	_delay_ms(10);
////////////////////////////////////////////////
	data = READ_ONE_BYTE();
	if (data == CH3_ENABLE);
	else  Problem(CH3_ENABLE, data);
	_delay_ms(10);
////////////////////////////////////////////////	
	WRITE_ONE_BYTE(NOT_ENABLE);
	_delay_ms(10);
////////////////////////////////////////////////
	data = READ_ONE_BYTE();
	if (data == NOT_ENABLE);
	else  Problem(NOT_ENABLE, data);
	_delay_ms(10);
}
/*******************************************************/
void Problem(uint8_t problem, uint8_t data)
{
	LED_WRITE_OFF;
	Ready_OFF;
////////////////////////	
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("PROBLEM");
	_delay_ms(500);
	LCD_Clear();
	LCD_Setpos(0, 0);
	sprintf(buffer_LCD,"0x%02X", problem);
	LCD_Str(buffer_LCD);
	LCD_Setpos(0, 1);
	sprintf(buffer_LCD,"0x%02X", data);
	LCD_Str(buffer_LCD);
	_delay_ms(500);
////////////////////////		
	while(1)
	{
		LED_READ_ON;
		_delay_ms(300);
		LED_READ_OFF;
		_delay_ms(300);
	}
}