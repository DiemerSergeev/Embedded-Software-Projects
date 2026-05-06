#include "preference.h"
#include <util/delay.h>

#include "LM96000_Driver.h"

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
uint8_t READ_BYTE(uint8_t COMMAND)
{
   uint8_t data = 0x00;
   uint8_t status = 0;
   uint8_t count = 0;
   LED_WRITE_OFF;
   LED_READ_ON;
	do 
	{
		_delay_ms(100);
		count++;
		
		I2C_COMAND(I2C_START);
   
		TWDR = WRITE_CMD;
		status = I2C_COMAND(I2C_TRANSMIT);
		
		if(count == 50) 
		{ 
			Problem(WRITE_CMD,0x00); 
		}
	} while (status != 0x18);
	
   TWDR = COMMAND;
   status = I2C_COMAND(I2C_TRANSMIT);
   if(status != 0x28) { Problem(COMMAND, 0x00); }

   status = I2C_COMAND(I2C_RESTART);
   if(status != 0x10) { Problem(I2C_RESTART, 0x00); }

   TWDR = READ_CMD;
   status = I2C_COMAND(I2C_TRANSMIT);
   if(status != 0x40) { Problem(READ_CMD, 0x00); }

   I2C_COMAND(I2C_RECEIVE_NACK);
   data = TWDR;

   I2C_COMAND(I2C_STOP);
   
   	LED_WRITE_OFF;
   	LED_READ_OFF;

   return data;
}
/*******************************************************/
void WRITE_BYTE(uint8_t COMMAND, uint8_t data)
{
	uint8_t count = 0;
	uint8_t status = 0;
	
	LED_WRITE_ON;
	LED_READ_OFF;
////////////////////////
	do
	{
		_delay_ms(100);
		count++;

		I2C_COMAND(I2C_START);

		TWDR = WRITE_CMD;
		status = I2C_COMAND(I2C_TRANSMIT);

		if(count == 50) Problem(WRITE_CMD, 0x00);

	} while (status != 0x18);
////////////////////////
	TWDR = COMMAND;
	status = I2C_COMAND(I2C_TRANSMIT);
	if(status != 0x28) { Problem(COMMAND, 0x00); }
////////////////////////
	TWDR = data;
	status = I2C_COMAND(I2C_TRANSMIT);
////////////////////////
	I2C_COMAND(I2C_STOP);
////////////////////////	
	LED_WRITE_OFF;
	LED_READ_OFF;
}
/*******************************************************/
void LM96000_CHECK(void)
{
	uint8_t data = 0x00;

	LED_WRITE_ON;
	LED_READ_ON;
	_delay_ms(100);
////////////////////////
	data = READ_BYTE(REG_ID);
	if (data == 0x01);
	else Problem(REG_ID, data);
	_delay_ms(100);
////////////////////////
	data = READ_BYTE(REG_VERSION);
	if ((data&0xF8) == 0x68);
	else Problem(REG_VERSION, data);
	_delay_ms(100);
////////////////////////
	data = READ_BYTE(REG_READY_LOCK_START_OVERRIDE);
	if ((data&0b00000100) == 0b00000100) WRITE_BYTE(REG_READY_LOCK_START_OVERRIDE,0x01);
	else Problem(REG_READY_LOCK_START_OVERRIDE, data);
	_delay_ms(100);
////////////////////////

	WRITE_BYTE(0x40, 0x01);
	_delay_ms(100);
	
	WRITE_BYTE(0x5C, 0xE0);
	_delay_ms(100);
	WRITE_BYTE(0x5D, 0xE0);
	_delay_ms(100);
	WRITE_BYTE(0x5E, 0xE0);
	_delay_ms(100);
	
	WRITE_BYTE(0x5F, 0x80);
	_delay_ms(100);
	WRITE_BYTE(0x60, 0x80);
	_delay_ms(100);
	WRITE_BYTE(0x61, 0x80);
	_delay_ms(100);

	WRITE_BYTE(0x64, 0x00);
	_delay_ms(100);
	WRITE_BYTE(0x65, 0x00);
	_delay_ms(100);
	WRITE_BYTE(0x66, 0x00);
	_delay_ms(100);
	
	WRITE_BYTE(REG_PWM1, 0x00);
	_delay_ms(100);
	WRITE_BYTE(REG_PWM2, 0x00);
	_delay_ms(100);
	WRITE_BYTE(REG_PWM3, 0x00);
	_delay_ms(100);

}
/*******************************************************/
int Set_THIGH(int temp)
{
	uint8_t data = 0x00;
	int result = 0;
////////////////////////
	temp = TEMP_RECONVERSION(temp);
////////////////////////
	WRITE_BYTE(REG_PROCESS_ZONE1_HIGH_TEMP,temp);  // REMOTE
	_delay_ms(10);
	WRITE_BYTE(REG_INTERNAL_ZONE2_HIGH_TEMP,temp); // LOCAL
	_delay_ms(10);
	WRITE_BYTE(REG_REMOTE_ZONE3_HIGH_TEMP,temp);   // REMOTE
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(REG_PROCESS_ZONE1_HIGH_TEMP);
	if (data == temp);
	else Problem(REG_PROCESS_ZONE1_HIGH_TEMP,data);
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(REG_INTERNAL_ZONE2_HIGH_TEMP);
	if (data == temp);
	else Problem(REG_INTERNAL_ZONE2_HIGH_TEMP,data);
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(REG_REMOTE_ZONE3_HIGH_TEMP);
	if (data == temp);
	else Problem(REG_REMOTE_ZONE3_HIGH_TEMP, data);
	_delay_ms(10);
////////////////////////
	result = TEMP_CONVERSION(temp);
	return result;
}
/*******************************************************/
int Set_TLOW(int temp)
{
	uint8_t data = 0x00;
	int result = 0;
////////////////////////
	temp = TEMP_RECONVERSION(temp);
////////////////////////
	WRITE_BYTE(REG_PROCESS_ZONE1_LOW_TEMP,temp);  // REMOTE
	_delay_ms(10);
	WRITE_BYTE(REG_INTERNAL_ZONE2_LOW_TEMP,temp); // LOCAL
	_delay_ms(10);
	WRITE_BYTE(REG_REMOTE_ZONE3_LOW_TEMP,temp);   // REMOTE
	_delay_ms(10);
////////////////////////	
	data = READ_BYTE(REG_PROCESS_ZONE1_LOW_TEMP);
	if (data == temp);
	else Problem(REG_PROCESS_ZONE1_LOW_TEMP, data);
	_delay_ms(10);
////////////////////////	
	data = READ_BYTE(REG_INTERNAL_ZONE2_LOW_TEMP);
	if (data == temp);
	else Problem(REG_INTERNAL_ZONE2_LOW_TEMP, data);
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(REG_REMOTE_ZONE3_LOW_TEMP);
	if (data == temp);
	else Problem(REG_REMOTE_ZONE3_LOW_TEMP, data);
	_delay_ms(10);
////////////////////////
	result = TEMP_CONVERSION(temp);
	return result;
}
/*******************************************************/
int TEMP_CONVERSION(int temp)
{
	if((temp&0b10000000) == 0b10000000)	// отрицательные температуры
	{
		temp = temp-256;
	}
	return temp;						// положительные температуры конвертируются сразу
}
/*******************************************************/
int TEMP_RECONVERSION(int temp)
{
	if(temp<0)	// отрицательные температуры
	{
		temp = -temp;
		temp = ~temp&0xFF;
		temp++;
	}
	return temp;						// положительные температуры конвертируются сразу
}
/*******************************************************/
void Problem(uint8_t problem, uint8_t data)
{
	LED_WRITE_OFF;
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
		LED_READ_ON;
		_delay_ms(300);
		LED_READ_OFF;
		_delay_ms(300);
	}
}