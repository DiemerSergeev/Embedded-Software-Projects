#include "preference.h"
#include <util/delay.h>

#include "LM75_Driver.h"

/*******************************************************/
void I2C_INIT(void)
{
	DDRD|=0b00000011;	// SDA(PD1), SCL(PD0) на выход
	
	TWSR&= (0<<TWPS1)|(0<<TWPS0);  // Биты предделителя настроены на 16
	TWBR=0x02;                     // Настраиваем частоту шины на 400 кГц
}
/*******************************************************/
uint8_t I2C_COMAND(uint8_t action){
	switch(action){
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
uint8_t READ_ONE_BYTE_REG(uint8_t REG_CMD)
{
	uint8_t data=0;
	_delay_ms(10);

	I2C_COMAND(I2C_START);       // формируем сигнал START

	LED_WRITE_ON;
	LED_READ_OFF;
	
	TWDR = WRITE_CMD;              // Загружаем адрес МС (WRITE)
	if (I2C_COMAND(I2C_TRANSMIT)!=0x20)     // Передаем
	{
		TWDR = REG_CMD&0x03;           // Загружаем адрес регистра
		I2C_COMAND(I2C_TRANSMIT);    // Передаем
		
		I2C_COMAND(I2C_START);       // формируем сигнал START
		
		LED_WRITE_OFF;
		LED_READ_ON;
			
		TWDR = READ_CMD;               // Загружаем адрес МС (READ)
		I2C_COMAND(I2C_TRANSMIT);    // Передаем
	
		I2C_COMAND(I2C_RECEIVE_NACK);    // Принимаем байт 
		data = TWDR;
		
		I2C_COMAND(I2C_STOP);        //Формируем сигнал STOP
		_delay_ms(10);
	}
	LED_WRITE_OFF;
	LED_READ_OFF;
	_delay_ms(10);
	return (data);
}
/*******************************************************/
uint16_t READ_TWO_BYTE_REG(uint8_t REG_CMD)
{
	uint16_t data = 0;
	_delay_ms(10);

	I2C_COMAND(I2C_START);       // формируем сигнал START

	LED_WRITE_ON;
	LED_READ_OFF;
	
	TWDR = WRITE_CMD;              // Загружаем адрес МС (WRITE)
	if (I2C_COMAND(I2C_TRANSMIT)!=0x20)     // Передаем
	{
		TWDR = REG_CMD&0x03;           // Загружаем адрес регистра
		I2C_COMAND(I2C_TRANSMIT);    // Передаем
		
		I2C_COMAND(I2C_START);       // формируем сигнал START
		
		LED_WRITE_OFF;
		LED_READ_ON;
			
		TWDR = READ_CMD;               // Загружаем адрес МС (READ)
		I2C_COMAND(I2C_TRANSMIT);    // Передаем
		
		I2C_COMAND(I2C_RECEIVE_ACK);    // Принимаем байт старший (ответ в 9 битах)
		data = TWDR;
		data = data<<8;
		
		I2C_COMAND(I2C_RECEIVE_NACK);    // Принимаем байт младший
		data |= TWDR;
				
		I2C_COMAND(I2C_STOP);        //Формируем сигнал STOP
		_delay_ms(10);
	}
	LED_WRITE_OFF;
	LED_READ_OFF;
	_delay_ms(10);
	return (data);
}
/*******************************************************/
void WRITE_ONE_BYTE_REG(uint8_t REG_CMD, uint8_t data)
{
	_delay_ms(10);

	I2C_COMAND(I2C_START);       // формируем сигнал START

	LED_WRITE_ON;
	LED_READ_OFF;
	
	TWDR=WRITE_CMD;              // Загружаем адрес МС (WRITE)
	I2C_COMAND(I2C_TRANSMIT);    // Передаем
	
	TWDR=REG_CMD&0x03;           // Загружаем адрес регистра который будем писать
	I2C_COMAND(I2C_TRANSMIT);    // Передаем
	
	
	TWDR=data;                   // Загружаем 8 бит данных в регистр
	I2C_COMAND(I2C_TRANSMIT);    // Передаем

	I2C_COMAND(I2C_STOP);        //Формируем сигнал STOP
	_delay_ms(10);
	
	LED_WRITE_OFF;
	LED_READ_OFF;
}
/*******************************************************/
void WRITE_TWO_BYTE_REG(uint8_t REG_CMD, uint16_t data)
{
	_delay_ms(10);

	I2C_COMAND(I2C_START);       // формируем сигнал START
	
	LED_WRITE_ON;
	LED_READ_OFF;

	TWDR = WRITE_CMD;              // Загружаем адрес МС (WRITE)
	I2C_COMAND(I2C_TRANSMIT);    // Передаем
	
	TWDR = REG_CMD&0x03;           // Загружаем адрес регистра который будем писать
	I2C_COMAND(I2C_TRANSMIT);    // Передаем
	
	
	TWDR=(uint8_t)((data&0xFF00)>>8); // Загружаем старшие 8 бит
	I2C_COMAND(I2C_TRANSMIT);    // Передаем
	
	TWDR=(uint8_t)(data&0x00FF); // Загружаем младшие 8 бит
	I2C_COMAND(I2C_TRANSMIT);    // Передаем

	I2C_COMAND(I2C_STOP);        //Формируем сигнал STOP
	_delay_ms(10);
	
	LED_WRITE_OFF;
	LED_READ_OFF;
}
/*******************************************************/
uint8_t LM75_Init(void)
{
	uint8_t Conf_data_byte = 0b00011000;
	uint8_t temp_data=0;
	
	I2C_INIT();
	WRITE_ONE_BYTE_REG(Conf_Reg, Conf_data_byte);
	_delay_ms(400);
	
	temp_data = READ_ONE_BYTE_REG(Conf_Reg);
	_delay_ms(100);
	if(temp_data==Conf_data_byte) temp_data=1; 
	else temp_data=0;
	
	return (temp_data);
}
uint8_t LM75_ReadConf(void)
{
		WRITE_ONE_BYTE_REG(Conf_Reg, 0b00000000);
		_delay_ms(1000);

		uint8_t data=0;
		_delay_ms(10);

		I2C_COMAND(I2C_START);       // формируем сигнал START

		LED_WRITE_ON;
		LED_READ_OFF;
		_delay_ms(100);
		
		TWDR = WRITE_CMD;              // Загружаем адрес МС (WRITE)
		if (I2C_COMAND(I2C_TRANSMIT)!=0x20)     // Передаем
		{
			TWDR = Conf_Reg&0x03;           // Загружаем адрес регистра
			I2C_COMAND(I2C_TRANSMIT);    // Передаем
			
			I2C_COMAND(I2C_START);       // формируем сигнал START
			
			LED_WRITE_OFF;
			LED_READ_ON;
			
			TWDR = READ_CMD;               // Загружаем адрес МС (READ)
			I2C_COMAND(I2C_TRANSMIT);    // Передаем
			
			rele_SDAOS_ON;
			_delay_ms(3000);
			rele_SDAOS_OFF;
						
			I2C_COMAND(I2C_RECEIVE_NACK);    // Принимаем байт
			data = TWDR;
			
			I2C_COMAND(I2C_STOP);        //Формируем сигнал STOP
			_delay_ms(10);
		}
		LED_WRITE_OFF;
		LED_READ_OFF;
		_delay_ms(10);
		return data;
}
/*******************************************************/
float LM75_Read_Temp(void)
{
	uint16_t temp_word = 0;
	float real_temp = 0;
	
	temp_word = READ_TWO_BYTE_REG(Temp_Reg);
	if ((temp_word&0x8000)==0x8000)           // Проверка на Бит S, 1-отрицательные температуры, 0-положительные температуры	
	{
		/*
		temp_word = (temp_word&0x7FF0)>>5; // Отрицательные температуры	
		//temp_word=0x0800-temp_word;
		//temp_word = 0x7FF-temp_word;
		//temp_word = temp_word+1;
		temp_word=temp_word&0x3FF;
		real_temp=-temp_word;
		real_temp=temp_word*(-0.125);
		*/
		temp_word=(temp_word&0x7FF0)>>5; // Отрицательные температуры
		temp_word=0x0400-temp_word;
		real_temp=temp_word*0.125;
		real_temp=0-real_temp;
	}
	else
	{
	  temp_word=(temp_word&0x7FF0)>>5;  // Положительные температуры	
	  real_temp=temp_word*0.125;
	}
	return (real_temp);
}
/*******************************************************/
void LM75_Set_TOS(float temp_os) // Максимальная температура больше которой OS переключится в 0
{
	uint16_t data=0;
	if (temp_os<0)
	{
		temp_os=temp_os*(-1);
		//temp_os=temp_os/0.0625;
		temp_os=temp_os/0.5;
		//data=0x0800-((uint16_t)(temp_os));
		data=0x0100-((uint16_t)(temp_os));
		data=data<<7;
		data|=0x8000;
	}
	else
	{
		temp_os=temp_os/0.5;
		data=((uint16_t)(temp_os));
		data=data<<7;
	}
    
    WRITE_TWO_BYTE_REG(TOS_Reg, data);
	_delay_ms(100);
}
/*******************************************************/
void LM75_Set_THYST(float temp_HYST) // Минимальная температура при которой OS возвращается в высокий уровень
{
	uint16_t data=0;
	if (temp_HYST<0)
	{
		temp_HYST=temp_HYST*(-1);
		temp_HYST=temp_HYST/0.5;
		data=0x0800-((uint16_t)(temp_HYST));
		data=data<<7;
		data|=0x8000;
	}
	else
	{
		temp_HYST=temp_HYST/0.5;
		data=((uint16_t)(temp_HYST));
		data=data<<7;
	}
	
	WRITE_TWO_BYTE_REG(THyst_Reg, data);
	_delay_ms(100);	
	
}
uint16_t TempConverter(uint16_t temp)
{
	if ((temp&0x8000)==0x8000)           // Проверка на Бит S, 1-отрицательные температуры, 0-положительные температуры
	{
		temp = (temp&0x7FF0)>>7; // Отрицательные температуры
		//temp_word=0x0800-temp_word;
		temp = 0x7FF-temp;
		temp = temp+1;
		//real_temp=0-real_temp;
		temp=temp*(-0.5);
	}
	else
	{
		temp=(temp&0x7FF0)>>7;  // Положительные температуры
		temp=temp*0.5;
	}
	return temp;
	
}

