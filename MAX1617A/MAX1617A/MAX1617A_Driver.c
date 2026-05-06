#include "preference.h"
#include <util/delay.h>

#include "MAX1617A_Driver.h"

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
void START(void)
{
	LED_WRITE_ON;
	LED_READ_OFF;
	////////////////////////
	I2C_COMAND(I2C_START);
	////////////////////////
	TWDR = 0x54;
	I2C_COMAND(I2C_TRANSMIT);
	////////////////////////
	TWDR = SPOR;
	I2C_COMAND(I2C_TRANSMIT);
	////////////////////////
	I2C_COMAND(I2C_STOP);
	////////////////////////
	LED_WRITE_OFF;
	LED_READ_OFF;
}
/*******************************************************/
uint8_t READ_BYTE(uint8_t COMMAND)
{
/*
	uint8_t data = 0x00;
////////////////////////
	LED_WRITE_ON;
	LED_READ_OFF;
////////////////////////		
	I2C_COMAND(I2C_START);
////////////////////////
	TWDR = WRITE_CMD;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////
	TWDR = COMMAND;
	I2C_COMAND(I2C_TRANSMIT);
////////////////////////
	I2C_COMAND(I2C_START);
////////////////////////
	LED_WRITE_OFF;
	LED_READ_ON;
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
////////////////////////
	return data;
*/
   uint8_t data = 0x00;
   uint8_t status = 0;

   // Шаг 1: Отправляем START
   status = I2C_COMAND(I2C_START);
   // Проверка статуса: START передан (0x08)
   if(status != 0x08) { Problem(); }

   // Шаг 2: Отправляем адрес слейва на запись
   TWDR = WRITE_CMD;
   status = I2C_COMAND(I2C_TRANSMIT);
   // Проверка статуса: SLA+W передан, ACK получен (0x18)
   if(status != 0x18) { Problem(); }

   // Шаг 3: Отправляем адрес регистра
   TWDR = COMMAND;
   status = I2C_COMAND(I2C_TRANSMIT);
   // Проверка статуса: Данные переданы, ACK получен (0x28)
   if(status != 0x28) { Problem(); }

   // Шаг 4: Отправляем REPEATED START
   status = I2C_COMAND(I2C_RESTART);
   // Проверка статуса: REPEATED START передан (0x10)
   if(status != 0x10) { Problem(); }

   // Шаг 5: Отправляем адрес слейва на чтение
   TWDR = READ_CMD;
   status = I2C_COMAND(I2C_TRANSMIT);
   // Проверка статуса: SLA+R передан, ACK получен (0x40)
   if(status != 0x40) { Problem(); }

   // Шаг 6: Читаем байт данных с NACK
   I2C_COMAND(I2C_RECEIVE_NACK);
   data = TWDR;

   // Шаг 7: Отправляем STOP
   I2C_COMAND(I2C_STOP);

   return data;
}
uint8_t RECEIVE_BYTE(void)
{
	uint8_t data = 0x00;
////////////////////////
	LED_WRITE_OFF;
	LED_READ_ON;
////////////////////////
	I2C_COMAND(I2C_START);
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
////////////////////////
	return data;
}
/*******************************************************/
void WRITE_BYTE(uint8_t COMMAND, uint8_t data)
{
	LED_WRITE_ON;
	LED_READ_OFF;
////////////////////////
	I2C_COMAND(I2C_START);
////////////////////////
	TWDR = WRITE_CMD;
	I2C_COMAND(I2C_TRANSMIT);
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
void SEND_BYTE(uint8_t data)
{
	LED_WRITE_ON;
	LED_READ_OFF;
////////////////////////	
	I2C_COMAND(I2C_START);
////////////////////////
	TWDR = WRITE_CMD;
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
uint8_t MAX1617A_CHECK(uint8_t data)
{
	uint8_t result = 0;
////////////////////////
	LED_WRITE_ON;
	LED_READ_ON;
////////////////////////
	A0_LOW;
	A1_LOW;
////////////////////////
	WRITE_BYTE(WCA, data);
	_delay_ms(50);
////////////////////////
	if (READ_BYTE(RCL) == data) result = 1;
	else result = 0;
////////////////////////	
	return result;
}
/*******************************************************/
int Set_THIGH(int temp)
{
	int result = 0x00;
	
	WRITE_BYTE(WLHO, TEMP_RECONVERSION(temp)); // LOCAL
	WRITE_BYTE(WRHA, TEMP_RECONVERSION(temp)); // REMOTE
	
	result = READ_BYTE(RLHN);
	result = TEMP_CONVERSION(result);
	
	return result;
}
/*******************************************************/
int Set_TLOW(int temp)
{
	int result = 0x00;

	WRITE_BYTE(WLLM,TEMP_RECONVERSION(temp)); // LOCAL
	WRITE_BYTE(WRLN,TEMP_RECONVERSION(temp)); // REMOTE
	
	result = READ_BYTE(RLLI);
	result = TEMP_CONVERSION(result);
	
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
void Problem(void)
{
	LED_WRITE_OFF;
	Ready_OFF;
	while(1)
	{
		LED_READ_ON;
		_delay_ms(300);
		LED_READ_OFF;
		_delay_ms(300);
	}
}
uint8_t READ_ALERT_RESPONSE(void)
{
	  uint8_t data = 0x00;
	  uint8_t status = 0;

	  // Адрес команды Alert Response (0x0C) с битом чтения (1)
	  const uint8_t ALERT_RESPONSE_CMD = 0x19;

	  // Шаг 1: Отправляем START
	  status = I2C_COMAND(I2C_START);
	  if(status != 0x08) { Problem(); }

	  // Шаг 2: Отправляем специальную команду Alert Response
	  TWDR = ALERT_RESPONSE_CMD;
	  status = I2C_COMAND(I2C_TRANSMIT);
	  // Ожидаемый статус: SLA+R передан, ACK получен (0x40)
	  if(status != 0x40) { Problem(); }

	  // Шаг 3: Читаем байт данных с NACK
	  I2C_COMAND(I2C_RECEIVE_NACK);
	  data = TWDR;

	  // Шаг 4: Отправляем STOP
	  I2C_COMAND(I2C_STOP);
	  
	  return data;
  }