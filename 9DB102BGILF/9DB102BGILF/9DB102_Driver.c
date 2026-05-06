#include "preference.h"
#include "9DB102_Driver.h"
#include <util/delay.h>

/*******************************************************/
void I2C_INIT(void)
{
	DDRD|=0b00000011;				// SDA(PD1), SCL(PD0) на выход
									// SCL = CLK/(16+2*TWBR*TWSR) = 8000000/(16+2*32*1)
	TWSR &= (0<<TWPS1)|(0<<TWPS0);	// Биты предделителя настроены на 1
	TWBR = 0x20;					// Настраиваем частоту шины на 100 кГц
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
void READ_BYTES(uint8_t N, uint8_t X)
{
	uint8_t status = 0;
	uint8_t count = 0;
////////////////////////////////////////////////	
	WRITE_OFF;
	READ_ON;
////////////////////////////////////////////////
	do
	{
		I2C_COMAND(I2C_START);
		
		TWDR = WRITE_CMD;
		status = I2C_COMAND(I2C_TRANSMIT);
		count++;
		
		if(count == 50) Problem(WRITE_CMD,0x00);
		
	} while (status != 0x18);
////////////////////////////////////////////////
	TWDR = N;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////////////////////////////
	I2C_COMAND(I2C_RESTART);
////////////////////////////////////////////////
	TWDR = READ_CMD;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////////////////////////////	
	I2C_COMAND(I2C_RECEIVE_ACK);
	X = TWDR;
////////////////////////////////////////////////
	for (int i = N; i <= N+X-1; i++)
	{
		I2C_COMAND(I2C_RECEIVE_ACK);
		mem_buff[i] = TWDR;
	}
////////////////////////////////////////////////
	I2C_COMAND(I2C_RECEIVE_NACK);
	I2C_COMAND(I2C_STOP);
////////////////////////////////////////////////	
	WRITE_OFF;
	READ_OFF;
}
/*******************************************************/
void WRITE_BYTES(uint8_t data, uint8_t N, uint8_t X)
{
	uint8_t status = 0;
	uint8_t count = 0;
////////////////////////////////////////////////
	WRITE_ON;
	READ_OFF;
////////////////////////////////////////////////
	do
	{
		I2C_COMAND(I2C_START);
		
		TWDR = WRITE_CMD;
		status = I2C_COMAND(I2C_TRANSMIT);
		count++;		
		
		if(count == 50) Problem(WRITE_CMD,0x00);

	} while (status != 0x18);
////////////////////////////////////////////////
	TWDR = N;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////////////////////////////
	for (int i = N; i <= N+X-1; i++)
	{
		TWDR = data;
		I2C_COMAND(I2C_TRANSMIT);	
	}
////////////////////////////////////////////////
	I2C_COMAND(I2C_STOP);
////////////////////////////////////////////////
	WRITE_OFF;
	READ_OFF;
}
/*******************************************************/
void ICS9DB102_CHECK(void)
{
	WRITE_ON;
	READ_ON;
	_delay_ms(100);
////////////////////////////////////////////////
    READ_BYTES(CONTROL_REG_BYTE,6);
////////////////////////////////////////////////
	if (mem_buff[REVISION_VENDOR_ID_BYTE] == 0x11);
	else  Problem(0x11, mem_buff[REVISION_VENDOR_ID_BYTE]);
////////////////////////////////////////////////
	if (mem_buff[DEVICE_ID_BYTE] == 0x06);
	else  Problem(0x06, mem_buff[DEVICE_ID_BYTE]);
////////////////////////////////////////////////
	if ((mem_buff[COUNT_BYTE]&0x06) == 0x06);
	else  Problem(0x00, mem_buff[CONTROL_REG_BYTE]&0x83);
////////////////////////////////////////////////
	WRITE_OFF;
	READ_OFF;
}
/*******************************************************/
void Problem(uint8_t problem, uint8_t data)
{
	WRITE_OFF;
	Ready_OFF;
////////////////////////	
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("PROBLEM");
	_delay_ms(500);
	LCD_Clear();
	LCD_Setpos(0, 0);
	sprintf(buffer_LCD,"ER 0x%02X", problem);
	LCD_Str(buffer_LCD);
	LCD_Setpos(0, 1);
	sprintf(buffer_LCD,"EQ 0x%02X", data);
	LCD_Str(buffer_LCD);
	_delay_ms(500);
////////////////////////		
	while(1)
	{
		READ_ON;
		_delay_ms(300);
		READ_OFF;
		_delay_ms(300);
	}
}