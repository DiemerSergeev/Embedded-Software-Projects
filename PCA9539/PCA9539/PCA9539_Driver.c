#include "preference.h"
#include <util/delay.h>

#include "PCA9539_Driver.h"

/*******************************************************/
void I2C_INIT(void)
{
	DDRD|=0b00000011;				// SDA(PD1), SCL(PD0) на выход
									// SCL = CLK/(16+2*TWBR*TWSR) = 8000000/(16+2*32*1)
	TWSR&= (0<<TWPS1)|(0<<TWPS0);	// Биты предделителя настроены на 1
	TWBR=0x20;						// Настраиваем частоту шины на 100 кГц
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
uint8_t READ_ONE_BYTE(uint8_t COMMAND)
{
	uint8_t data = 0x00;
	uint8_t count = 0;
	uint8_t status = 0;
////////////////////////
	LED_WRITE_OFF;
	LED_READ_ON;
////////////////////////
	do
	{
		count++;
		
		I2C_COMAND(I2C_START);
			
		TWDR = WRITE_CMD;
		status = I2C_COMAND(I2C_TRANSMIT);
		
		if(count == 50) Problem(WRITE_CMD,0x00);
		
	} while (status != 0x18);
////////////////////////
	TWDR = COMMAND;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////
	I2C_COMAND(I2C_RESTART);
////////////////////////
	TWDR = READ_CMD;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////
	I2C_COMAND(I2C_RECEIVE_NACK);
	data = TWDR;
////////////////////////
	I2C_COMAND(I2C_STOP);
////////////////////////	
	LED_WRITE_OFF;
	LED_READ_OFF;

	return data;
}
/*******************************************************/
uint64_t READ_ALL_BYTES(uint8_t COMMAND)
{
	uint64_t data = 0x00;
	uint8_t count = 0;
	uint8_t status = 0;
////////////////////////
	LED_WRITE_OFF;
	LED_READ_ON;
////////////////////////
	do
	{
		count++;
		
		I2C_COMAND(I2C_START);
		
		TWDR = WRITE_CMD;
		status = I2C_COMAND(I2C_TRANSMIT);
		
		if(count == 50) Problem(WRITE_CMD,0x00);

	} while (status != 0x18);
////////////////////////
	TWDR = COMMAND;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////
	I2C_COMAND(I2C_RESTART);
////////////////////////
	TWDR = READ_CMD;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////
	I2C_COMAND(I2C_RECEIVE_ACK);
	data |= TWDR;
	data = data << 8;
////////////////////////	
	I2C_COMAND(I2C_RECEIVE_ACK);
	data |= TWDR;
	data = data << 8;
////////////////////////	
	I2C_COMAND(I2C_RECEIVE_ACK);
	data |= TWDR;
	data = data << 8;
////////////////////////	
	I2C_COMAND(I2C_RECEIVE_ACK);
	data |= TWDR;
	data = data << 8;
////////////////////////	
	I2C_COMAND(I2C_RECEIVE_ACK);
	data |= TWDR;
////////////////////////	
	I2C_COMAND(I2C_RECEIVE_NACK);
////////////////////////	
	I2C_COMAND(I2C_STOP);
////////////////////////
	LED_WRITE_OFF;
	LED_READ_OFF;

	return data;
}
/*******************************************************/
void WRITE_ONE_BYTE(uint8_t COMMAND, uint8_t data)
{
	uint8_t status = 0;
	uint8_t count = 0; 
////////////////////////
	LED_WRITE_ON;
	LED_READ_OFF;
////////////////////////
	do
	{
		count++;

		I2C_COMAND(I2C_START);

		TWDR = WRITE_CMD;
		status = I2C_COMAND(I2C_TRANSMIT);

		if(count == 50) Problem(WRITE_CMD, 0x00);

	} while (status != 0x18);
////////////////////////
	TWDR = COMMAND;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////
	TWDR = data;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////
	I2C_COMAND(I2C_STOP);
////////////////////////	
	LED_WRITE_OFF;
	LED_READ_OFF;
}
/*******************************************************/
void WRITE_ALL_BYTES(uint8_t COMMAND, uint8_t data)
{
	uint8_t count = 0;
	uint8_t status = 0;
////////////////////////
	LED_WRITE_ON;
	LED_READ_OFF;
////////////////////////
	do
	{
		count++;

		I2C_COMAND(I2C_START);

		TWDR = WRITE_CMD;
		status = I2C_COMAND(I2C_TRANSMIT);

		if(count == 50) Problem(WRITE_CMD, 0x00);

	} while (status != 0x18);
////////////////////////
	TWDR = COMMAND;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////
	TWDR = data;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////
	TWDR = data;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////
	TWDR = data;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////
	TWDR = data;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////
	TWDR = data;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////
	I2C_COMAND(I2C_STOP);
////////////////////////
	LED_WRITE_OFF;
	LED_READ_OFF;
}
/*******************************************************/
void PCA9539_CHECK(void)
{
	uint8_t data = 0x00;

	LED_WRITE_ON;
	LED_READ_ON;
	_delay_ms(100);
/************************************************************************/
/* IOC                                                                  */
/************************************************************************/
    WRITE_ONE_BYTE(IOC0, 0xFF);
	_delay_ms(10);
    WRITE_ONE_BYTE(IOC1, 0xFF);
    _delay_ms(10);
////////////////////////	
	data = READ_ONE_BYTE(IOC0);
	if (data == 0xFF);
	else  Problem(IOC0, data);
	_delay_ms(10);
////////////////////////
	data = READ_ONE_BYTE(IOC1);
	if (data == 0xFF);
	else  Problem(IOC1, data);	
	_delay_ms(10);
////////////////////////
	WRITE_ONE_BYTE(IOC0, 0x00);
	_delay_ms(10);
	WRITE_ONE_BYTE(IOC1, 0x00);
	_delay_ms(10);
////////////////////////
	data = READ_ONE_BYTE(IOC0);
	if (data == 0x00);
	else  Problem(IOC0, data);
	_delay_ms(10);
////////////////////////
	data = READ_ONE_BYTE(IOC1);
	if (data == 0x00);
	else  Problem(IOC1, data);
	_delay_ms(10);
/************************************************************************/
/* OP                                                                   */
/************************************************************************/
	WRITE_ONE_BYTE(OP0, 0xFF);
	_delay_ms(10);
	WRITE_ONE_BYTE(OP1, 0xFF);
	_delay_ms(10);	
////////////////////////
	data = READ_ONE_BYTE(OP0);
	if (data == 0xFF);
	else Problem(OP0, data);
	_delay_ms(10);
////////////////////////
	data = READ_ONE_BYTE(OP1);
	if (data == 0xFF);
	else Problem(OP0, data);
	_delay_ms(10);
////////////////////////
	WRITE_ONE_BYTE(OP0, 0x00);
	_delay_ms(10);
	WRITE_ONE_BYTE(OP1, 0x00);
	_delay_ms(10);
////////////////////////
	data = READ_ONE_BYTE(OP0);
	if (data == 0x00);
	else Problem(OP0, data);
	_delay_ms(10);
////////////////////////
	data = READ_ONE_BYTE(OP1);
	if (data == 0x00);
	else Problem(OP0, data);
	_delay_ms(10);
/************************************************************************/
/*  PI                                                                  */
/************************************************************************/
	WRITE_ALL_BYTES(PI0, 0xFF);
	_delay_ms(10);
	WRITE_ALL_BYTES(PI1, 0xFF);
	_delay_ms(10);
////////////////////////
	data = READ_ALL_BYTES(PI0);
	if (data == 0xFF);
	else Problem(PI0, data);
	_delay_ms(10);
////////////////////////
	data = READ_ALL_BYTES(PI1);
	if (data == 0xFF);
	else Problem(PI0, data);
	_delay_ms(10);
////////////////////////
	WRITE_ALL_BYTES(PI0, 0x00);
	_delay_ms(10);
	WRITE_ALL_BYTES(PI1, 0x00);
	_delay_ms(10);
////////////////////////
	data = READ_ALL_BYTES(PI0);
	if (data == 0x00);
	else Problem(PI0, data);
	_delay_ms(10);
////////////////////////
	data = READ_ALL_BYTES(PI1);
	if (data == 0x00);
	else Problem(PI0, data);
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