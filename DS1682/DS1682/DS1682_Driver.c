#include "preference.h"
#include <util/delay.h>

#include "DS1682_Driver.h"

/*******************************************************/
void I2C_INIT(void)
{
	DDRD|=0b00000011;	// SDA(PD1), SCL(PD0) на выход
	// SCL = CLK/(16+2*TWBR*TWSR) = 8000000/(16+2*32*1)
	TWSR&= (0<<TWPS1)|(0<<TWPS0);  // Биты предделителя настроены на 1
	TWBR=0x20;                     // Настраиваем частоту шины на 100 кГц
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
uint8_t READ_ONE_REG(uint8_t REG_CMD)
{
	uint8_t data = 0x00;
	
	I2C_COMAND(I2C_START);
	
////////////////////////
	LED_WRITE_ON;
	LED_READ_OFF;
////////////////////////
	
	TWDR = WRITE_CMD;
	I2C_COMAND(I2C_TRANSMIT);

	TWDR = REG_CMD;
	I2C_COMAND(I2C_TRANSMIT);
	
	I2C_COMAND(I2C_START);
	
////////////////////////
	LED_WRITE_OFF;
	LED_READ_ON;
////////////////////////
	
	TWDR = READ_CMD;
	I2C_COMAND(I2C_TRANSMIT);

	I2C_COMAND(I2C_RECEIVE_NACK);
	
	data = TWDR;
	
////////////////////////
	LED_WRITE_OFF;
	LED_READ_OFF;
////////////////////////
	
	return data;
}
/*******************************************************/
void WRITE_ONE_REG(uint8_t REG_CMD, uint8_t data)
{
	I2C_COMAND(I2C_START);
	
////////////////////////
	LED_WRITE_ON;
	LED_READ_OFF;
////////////////////////
	
	TWDR = WRITE_CMD;
	I2C_COMAND(I2C_TRANSMIT);
	
	TWDR = REG_CMD;
	I2C_COMAND(I2C_TRANSMIT);
	
	TWDR = data;
	I2C_COMAND(I2C_TRANSMIT);

	I2C_COMAND(I2C_STOP);
	
////////////////////////	
	LED_WRITE_OFF;
	LED_READ_OFF;
////////////////////////

}
/*******************************************************/
uint8_t DS1682_CHECK(uint8_t data)
{
/*	0x0|AF|WDF|WMDF|AOS|RE|AP|ECMSB
	
	AF - ALARM FLAG
	WDF - WRITE DISABLE FLAG
	WMDF - WRITE-MEMORY DISABLE FLAG
	AOS - ALARM OUTPUT SELECT
	RE - RESET ENABLE
	AP - ALARM POLARITY
	ECMSB - EVENT COUNTER MSB */
	
////////////////////////
	LED_WRITE_ON;
	LED_READ_ON;
////////////////////////
	EVENT_HIGH;
	_delay_ms(100);
	EVENT_LOW;
	_delay_ms(100);
	
	WRITE_ONE_REG(CONF_REG, data);
	_delay_ms(100);
	
	return READ_ONE_REG(CONF_REG);
}