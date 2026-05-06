
#include "main.h"
#include "PCF8575.h"
#include "i2c.h"


const int WRITE_CMD = 0x40;			 //Адресс МИКРОСХЕМЫ+0, будем писать (0  0  1  1  0-A2  0-A1  1-A0  R/W) 10000000 (0x40);
const int READ_CMD = 0x41;			 //Адресс МИКРОСХЕМЫ+1, будем читать (0  0  1  1  0-A2  0-A1  1-A0  R/W) 10000001 (0x41);

const int PORT0_CMD = 0x11;         // ????
const int PORT1_CMD = 0x12;         //????




uint16_t READ_REGISTER(void)
{
	uint16_t data=0;
	_delay_ms(10);

	I2C_COMAND(I2C_START);       // формируем сигнал START

		TWDR=READ_CMD;                // Загружаем адрес запроса ID
		I2C_COMAND(I2C_TRANSMIT);    // Передаем
				
		I2C_COMAND(I2C_RECEIVE_ACK);    // Принимаем младший байт
		data=TWDR;
		
		I2C_COMAND(I2C_RECEIVE_NACK);    // Принимаем старший байт
		data=data<<8;
		data+=TWDR;
		I2C_COMAND(I2C_STOP); 	
//	}

	_delay_ms(10);
	return (data);
}


void WRITE_REGISTER(uint16_t data)
{
	_delay_ms(10);

	I2C_COMAND(I2C_START);       // формируем сигнал START

	TWDR=WRITE_CMD;              // Загружаем адрес МС (WRITE)
	I2C_COMAND(I2C_TRANSMIT);    // Передаем
	
	TWDR=(uint8_t)(data & 0xFF);                  // Загружаем 8 младших бит данных в регистр 
	I2C_COMAND(I2C_TRANSMIT);   // Передаем
	
	TWDR=(uint8_t)(data >> 8);                  // Загружаем 8 старших бит данных в регистр  !!! 
	I2C_COMAND(I2C_TRANSMIT);   // Передаем

	I2C_COMAND(I2C_STOP);        //Формируем сигнал STOP
	_delay_ms(10);
}

uint8_t FindChip(void)
{
	uint8_t data=0;
	_delay_ms(10);

	I2C_COMAND(I2C_START);       // формируем сигнал START

	TWDR=WRITE_CMD;              // Загружаем адрес МС (WRITE)
	if (I2C_COMAND(I2C_TRANSMIT)!=0x20)     // Передаем	
	{
	//	TWDR=MANUF_ID;               // Загружаем адрес запроса ID
		I2C_COMAND(I2C_TRANSMIT);    // Передаем
		
		I2C_COMAND(I2C_START);       // формируем сигнал START
	
		TWDR=READ_CMD;               // Загружаем адрес МС (READ)
		I2C_COMAND(I2C_TRANSMIT);    // Передаем
		
		I2C_COMAND(I2C_RECEIVE_NACK);    // Принимаем байт
		data=TWDR;
	}
	else
	{
		data = I2C_TRANSMIT;
	}
	
	return (data);                     // (должно вернуться 0x01)
}

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
	uint8_t k=0;
	if(action != I2C_STOP){
		 while ((!(TWCR & _BV(TWINT))) || (k<40))
		 {
			 _delay_ms(2);
			 k++;
		 }
	}
	return (TWSR & 0xF8);
}