#include "preference.h"
#include <util/delay.h>

#include "LM95214_Driver.h"

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
   uint8_t data = 0;
   uint8_t status = 0;
   uint8_t count = 0;
//////////////////////////////////////////////// 
   LED_WRITE_OFF;
   LED_READ_ON;
////////////////////////////////////////////////
	do 
	{
		_delay_ms(100);
		count++;
		
		I2C_COMAND(I2C_START);
   
		TWDR = WRITE_CMD;
		status = I2C_COMAND(I2C_TRANSMIT);
		
		if(count == 50) Problem(WRITE_CMD,0x00);
		
	} while (status != 0x18);
////////////////////////////////////////////////
   TWDR = COMMAND;
   status = I2C_COMAND(I2C_TRANSMIT);
   if(status != 0x28) { Problem(COMMAND, 0x00); }
////////////////////////////////////////////////
   status = I2C_COMAND(I2C_RESTART);
   if(status != 0x10) { Problem(I2C_RESTART, 0x00); }
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
/*******************************************************/
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
		_delay_ms(100);
		count++;

		I2C_COMAND(I2C_START);

		TWDR = WRITE_CMD;
		status = I2C_COMAND(I2C_TRANSMIT);

		if(count == 50) Problem(WRITE_CMD, 0x00);

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
/*******************************************************/
void LM95214_CHECK(void)
{
	uint8_t data = 0x00;
////////////////////////////////////////////////
	LED_WRITE_ON;
	LED_READ_ON;
////////////////////////////////////////////////	
	data = READ_BYTE(MANUFACTURER_ID);
	if (data == 0x01);
	else Problem(MANUFACTURER_ID, data);
	_delay_ms(10);
////////////////////////////////////////////////	
	data = READ_BYTE(REVISION_ID);
	if ((data) == 0x79);
	else Problem(REVISION_ID, data);
	_delay_ms(10);
////////////////////////////////////////////////
	LED_WRITE_OFF;
	LED_READ_OFF;
}
/*******************************************************/
void LM95214_CONFIG(void)
{
	uint8_t data = 0x00;
////////////////////////
	WRITE_BYTE(TCRIT1_MASK,0x00);  // REMOTE
	_delay_ms(10);
	WRITE_BYTE(TCRIT2_MASK,0x00);  // REMOTE
	_delay_ms(10);
	WRITE_BYTE(TCRIT3_MASK,0x00);  // REMOTE
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(TCRIT1_MASK);
	if (data == 0x00);
	else Problem(TCRIT1_MASK, data);
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(TCRIT2_MASK);
	if (data == 0x00);
	else Problem(TCRIT2_MASK, data);
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(TCRIT3_MASK);
	if (data == 0x00);
	else Problem(TCRIT3_MASK, data);
	_delay_ms(10);
////////////////////////
	WRITE_BYTE(CONVERSION_RATE,0x00);  // REMOTE
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(CONVERSION_RATE);
	if (data == 0x00);
	else Problem(CONVERSION_RATE, data);
	_delay_ms(10);
////////////////////////
	WRITE_BYTE(CHANNEL_CONVERSION_ENABLE,0x00);  // REMOTE
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(CHANNEL_CONVERSION_ENABLE);
	if (data == 0x00);
	else Problem(CHANNEL_CONVERSION_ENABLE, data);
	_delay_ms(10);
////////////////////////
	WRITE_BYTE(CONFIGURATION,0x00);  // REMOTE
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(CONFIGURATION);
	if (data == 0x00);
	else Problem(CONFIGURATION, data);
	_delay_ms(10);
////////////////////////
	WRITE_BYTE(REMOTE1_OFFSET,0x00);  // REMOTE
	_delay_ms(10);
	WRITE_BYTE(REMOTE2_OFFSET,0x00);  // REMOTE
	_delay_ms(10);
	WRITE_BYTE(REMOTE3_OFFSET,0x00);  // REMOTE
	_delay_ms(10);
	WRITE_BYTE(REMOTE4_OFFSET,0x00);  // REMOTE
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(REMOTE1_OFFSET);
	if (data == 0x00);
	else Problem(REMOTE1_OFFSET, data);
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(REMOTE2_OFFSET);
	if (data == 0x00);
	else Problem(REMOTE2_OFFSET, data);
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(REMOTE3_OFFSET);
	if (data == 0x00);
	else Problem(REMOTE3_OFFSET, data);
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(REMOTE4_OFFSET);
	if (data == 0x00);
	else Problem(REMOTE4_OFFSET, data);
	_delay_ms(10);
////////////////////////
	WRITE_BYTE(LOCAL_TCRIT_LIMIT,0xFF);  // REMOTE
	_delay_ms(10);
	WRITE_BYTE(REMOTE1_TCRIT1_LIMIT,0xFF);  // REMOTE
	_delay_ms(10);
	WRITE_BYTE(REMOTE2_TCRIT1_LIMIT,0xFF);  // REMOTE
	_delay_ms(10);
	WRITE_BYTE(REMOTE3_TCRIT_LIMIT,0xFF);  // REMOTE
	_delay_ms(10);
	WRITE_BYTE(REMOTE4_TCRIT_LIMIT,0xFF);  // REMOTE
	_delay_ms(10);
	WRITE_BYTE(REMOTE1_TCRIT2_TCRIT3_LIMIT,0xFF);  // REMOTE
	_delay_ms(10);
	WRITE_BYTE(REMOTE2_TCRIT2_TCRIT3_LIMIT,0xFF);  // REMOTE
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(LOCAL_TCRIT_LIMIT);
	if (data == 0xFF);
	else Problem(LOCAL_TCRIT_LIMIT, data);
	_delay_ms(10);
	
	data = READ_BYTE(REMOTE1_TCRIT1_LIMIT);
	if (data == 0xFF);
	else Problem(REMOTE1_TCRIT1_LIMIT, data);
	_delay_ms(10);
	
	data = READ_BYTE(REMOTE2_TCRIT1_LIMIT);
	if (data == 0xFF);
	else Problem(REMOTE2_TCRIT1_LIMIT, data);
	_delay_ms(10);
	
	data = READ_BYTE(REMOTE3_TCRIT_LIMIT);
	if (data == 0xFF);
	else Problem(REMOTE3_TCRIT_LIMIT, data);
	_delay_ms(10);
	
	data = READ_BYTE(REMOTE4_TCRIT_LIMIT);
	if (data == 0xFF);
	else Problem(REMOTE4_TCRIT_LIMIT, data);
	_delay_ms(10);
	
	data = READ_BYTE(REMOTE1_TCRIT2_TCRIT3_LIMIT);
	if (data == 0xFF);
	else Problem(REMOTE1_TCRIT2_TCRIT3_LIMIT, data);
	_delay_ms(10);
	
	data = READ_BYTE(REMOTE2_TCRIT2_TCRIT3_LIMIT);
	if (data == 0xFF);
	else Problem(REMOTE2_TCRIT2_TCRIT3_LIMIT, data);
	_delay_ms(10);
////////////////////////
	WRITE_BYTE(FILTER_SETTING,0x00);  // REMOTE
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(FILTER_SETTING);
	if (data == 0x00);
	else Problem(FILTER_SETTING, data);
	_delay_ms(10);
////////////////////////
	WRITE_BYTE(REMOTE1_OFFSET,0x00);  // REMOTE
	_delay_ms(10);
}
/*******************************************************/
int Set_TCRIT(int temp)
{
	uint8_t data = 0x00;
	int result = 0;
////////////////////////
	temp = TEMP_RECONVERSION(temp);
////////////////////////
	WRITE_BYTE(LOCAL_TCRIT_LIMIT,temp);  // REMOTE
	_delay_ms(10);
	WRITE_BYTE(REMOTE1_TCRIT1_LIMIT,temp); // LOCAL
	_delay_ms(10);
	WRITE_BYTE(REMOTE2_TCRIT1_LIMIT,temp);   // REMOTE
	_delay_ms(10);
	WRITE_BYTE(REMOTE3_TCRIT_LIMIT,temp);  // REMOTE
	_delay_ms(10);
	WRITE_BYTE(REMOTE4_TCRIT_LIMIT,temp); // LOCAL
	_delay_ms(10);
	WRITE_BYTE(REMOTE1_TCRIT2_TCRIT3_LIMIT,temp);   // REMOTE
	_delay_ms(10);
	WRITE_BYTE(REMOTE2_TCRIT2_TCRIT3_LIMIT,temp);   // REMOTE
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(LOCAL_TCRIT_LIMIT);
	if (data == temp);
	else Problem(LOCAL_TCRIT_LIMIT,data);
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(REMOTE1_TCRIT1_LIMIT);
	if (data == temp);
	else Problem(REMOTE1_TCRIT1_LIMIT,data);
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(REMOTE2_TCRIT1_LIMIT);
	if (data == temp);
	else Problem(REMOTE2_TCRIT1_LIMIT, data);
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(REMOTE3_TCRIT_LIMIT);
	if (data == temp);
	else Problem(REMOTE3_TCRIT_LIMIT,data);
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(REMOTE4_TCRIT_LIMIT);
	if (data == temp);
	else Problem(REMOTE4_TCRIT_LIMIT,data);
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(REMOTE1_TCRIT2_TCRIT3_LIMIT);
	if (data == temp);
	else Problem(REMOTE1_TCRIT2_TCRIT3_LIMIT, data);
	_delay_ms(10);
////////////////////////
	data = READ_BYTE(REMOTE2_TCRIT2_TCRIT3_LIMIT);
	if (data == temp);
	else Problem(REMOTE2_TCRIT2_TCRIT3_LIMIT, data);
	_delay_ms(10);
////////////////////////
	result = TEMP_CONVERSION(temp);
	return result;
}
/*******************************************************/
int Set_HYST(int temp)
{
	uint8_t data = 0x00;
	int result = 0;
////////////////////////
	temp = TEMP_RECONVERSION(temp);
////////////////////////
	WRITE_BYTE(COMMON_TCRIT_HYSTERESIS,temp);  // REMOTE
	_delay_ms(10);
////////////////////////	
	data = READ_BYTE(COMMON_TCRIT_HYSTERESIS);
	if (data == temp);
	else Problem(COMMON_TCRIT_HYSTERESIS, data);
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