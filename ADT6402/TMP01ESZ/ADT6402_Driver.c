#include "preference.h"
#include <util/delay.h>
#include "ADT6402_Driver.h"

/************************************************************************/
/* I2C команды                                                          */
/************************************************************************/
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

/************************************************************************/
/* Настройка I2C                                                        */
/************************************************************************/
void I2C_INIT(void)
{
	DDRD|=0b00000011;				// SDA(PD1), SCL(PD0) на выход
	// SCL = CLK/(16+2*TWBR*TWSR) = 8000000/(16+2*32*1)
	TWSR&= (0<<TWPS1)|(0<<TWPS0);	// Биты предделителя настроены на 1
	TWBR=0x20;						// Настраиваем частоту шины на 100 кГц
}

/************************************************************************/
/* Чтение                                                               */
/************************************************************************/
uint8_t READ_BYTE(uint8_t COMMAND)
{
	uint8_t data = 0;
	uint8_t status = 0;
	uint8_t count = 0;
////////////////////////////////////////////////
	LED_WRITE_ON;
	LED_READ_OFF;
////////////////////////////////////////////////
	do
	{
		_delay_ms(10);
		count++;
		
		I2C_COMAND(I2C_START);
		
		TWDR = WRITE_CMD;
		status = I2C_COMAND(I2C_TRANSMIT);
		
		if(count == 1000) Problem(WRITE_CMD,0x00);
		
	} while (status != 0x18);
////////////////////////////////////////////////
	TWDR = COMMAND;
	status = I2C_COMAND(I2C_TRANSMIT);
	if(status != 0x28) { Problem(COMMAND, 0x00); }
////////////////////////////////////////////////
	status = I2C_COMAND(I2C_RESTART);
	if(status != 0x10) { Problem(I2C_RESTART, 0x00); }
////////////////////////////////////////////////
	LED_WRITE_OFF;
	LED_READ_ON;
////////////////////////////////////////////////
	TWDR = READ_CMD;
	status = I2C_COMAND(I2C_TRANSMIT);
	if(status != 0x40) { Problem(READ_CMD, 0x00); }
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

/************************************************************************/
/* Чтение                                                               */
/************************************************************************/
uint16_t READ_2BYTE(uint8_t COMMAND)
{
	uint16_t data = 0;
	uint8_t status = 0;
	uint8_t count = 0;
////////////////////////////////////////////////
	LED_WRITE_ON;
	LED_READ_OFF;
////////////////////////////////////////////////
	do
	{
		_delay_ms(10);
		count++;
		
		I2C_COMAND(I2C_START);
		
		TWDR = WRITE_CMD;
		status = I2C_COMAND(I2C_TRANSMIT);
		
		if(count == 1000) Problem(WRITE_CMD,0x00);
		
	} while (status != 0x18);
////////////////////////////////////////////////
	TWDR = COMMAND;
	status = I2C_COMAND(I2C_TRANSMIT);
	if(status != 0x28) { Problem(COMMAND, 0x00); }
////////////////////////////////////////////////
	status = I2C_COMAND(I2C_RESTART);
	if(status != 0x10) { Problem(I2C_RESTART, 0x00); }
////////////////////////////////////////////////
	LED_WRITE_OFF;
	LED_READ_ON;
////////////////////////////////////////////////
	TWDR = READ_CMD;
	status = I2C_COMAND(I2C_TRANSMIT);
	if(status != 0x40) { Problem(READ_CMD, 0x00); }
////////////////////////////////////////////////		
	I2C_COMAND(I2C_RECEIVE_ACK);
	data |= TWDR;
	data = data << 8;
////////////////////////////////////////////////	
	I2C_COMAND(I2C_RECEIVE_NACK);
	data |= TWDR;
////////////////////////////////////////////////
	I2C_COMAND(I2C_STOP);
////////////////////////////////////////////////
	LED_WRITE_OFF;
	LED_READ_OFF;
////////////////////////////////////////////////
	return data;
}

/************************************************************************/
/* Запись                                                               */
/************************************************************************/
void WRITE_BYTE(uint8_t COMMAND, uint8_t data)
{
	uint8_t count = 0;
	uint8_t status = 0;
////////////////////////////////////////////////
	LED_WRITE_ON;
	LED_READ_OFF;
////////////////////////////////////////////////
	do
	{
		_delay_ms(10);
		count++;

		I2C_COMAND(I2C_START);

		TWDR = WRITE_CMD;
		status = I2C_COMAND(I2C_TRANSMIT);

		if(count == 1000) Problem(WRITE_CMD, 0x00);

	} while (status != 0x18);
////////////////////////////////////////////////
	TWDR = COMMAND;
	status = I2C_COMAND(I2C_TRANSMIT);
	if(status != 0x28) { Problem(COMMAND, 0x00); }
////////////////////////////////////////////////
	TWDR = data;
	status = I2C_COMAND(I2C_TRANSMIT);
////////////////////////////////////////////////
	I2C_COMAND(I2C_STOP);
////////////////////////////////////////////////
	LED_WRITE_OFF;
	LED_READ_OFF;
}

/************************************************************************/
/* Настройка I2C                                                        */
/************************************************************************/
uint8_t TMP100_Init(void)
{
	uint8_t temp_data = 0;
	
	I2C_INIT();
	
	if((READ_BYTE(CONF_REG) == 0x80) || (READ_BYTE(CONF_REG) == 0x00)) temp_data = 1; // Регистр конфигурации при сбросе обнуляется, OS/ALERT = 1
	else temp_data = 0;
	
	return (temp_data);
}

/************************************************************************/
/* Чтение с датчика                                                     */
/************************************************************************/
float Temp_read(void)
{
/*	_delay_ms(10);
	uint16_t data;
	float result=0;
	
	data = READ_2BYTE(TEMP_REG);
	_delay_ms(10);
	
	data = data >> 7;
	data = data&0x01FF;
	
	if ((data&0x0100) == 0x0100)    // Проверка на Бит S, 1-отрицательные температуры, 0-положительные температуры	
	{
		data = data & 0x00FF;		// Сброс знакового бита
		result = data - 0x0100;	// Инверсия температуры
		result = result*0.5;		// Разрешение датчика 0.5°C
	}
	else 
	{
		result = data;
		result = result*0.5;		// Разрешение датчика 0.5°C
	}

	return(result);
	*/
 _delay_ms(10);

 int16_t raw;
 float result;

 raw = (int16_t)READ_2BYTE(TEMP_REG);
 _delay_ms(10);

 // Сдвиг в зависимости от формата датчика
 raw = raw >> 7;   // если действительно нужно (зависит от датчика!)

 // Перевод в градусы
 result = raw * 0.5;

 return result;
}

/************************************************************************/
/* Вывод ошибки                                                         */
/************************************************************************/
void Problem(uint8_t problem, uint8_t data)
{
	LED_WRITE_OFF;
	LED_READ_OFF;
	Ready_OFF;
	
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
	
	while(1)
	{
		LED_ERROR_ON;
		_delay_ms(250);
		LED_ERROR_OFF;
		_delay_ms(250);
	}
}