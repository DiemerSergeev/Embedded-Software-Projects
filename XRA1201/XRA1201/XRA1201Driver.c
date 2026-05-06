#include "preference.h"
#include <util/delay.h>

#include "XRA1201Driver.h"


void I2C_INIT(void)
{
	DDRD|=0b00000011;	// SDA - PD1, SCL - PD0 на выход
	
	TWSR&= (0<<TWPS1)|(0<<TWPS0); // биты предделителя настроены на 16
	TWBR=0x02;                     //Настраиваем частоту шины на 400 кГц

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
	if(action != I2C_STOP)while (!(TWCR & _BV(TWINT)));
	return (TWSR & 0xF8);
}



uint8_t READ_REGISTER(uint8_t REG_CMD)
{
	uint8_t data=0;
	_delay_ms(10);

	I2C_COMAND(I2C_START);       // формируем сигнал START


	TWDR=WRITE_CMD;              // Загружаем адрес МС (WRITE)
	if (I2C_COMAND(I2C_TRANSMIT)!=0x20)     // Передаем
	
	{
		TWDR=REG_CMD;                // Загружаем адрес регистра
		I2C_COMAND(I2C_TRANSMIT);    // Передаем
		
		I2C_COMAND(I2C_START);       // формируем сигнал START
		
		TWDR=READ_CMD;               // Загружаем адрес МС (READ)
		I2C_COMAND(I2C_TRANSMIT);    // Передаем
		
		I2C_COMAND(I2C_RECEIVE_NACK);    // Принимаем байт
		data=TWDR;
	}

	_delay_ms(10);
	return (data);
}





void WRITE_REGISTER(uint8_t REG_CMD, uint8_t data)
{
	_delay_ms(10);

	I2C_COMAND(I2C_START);       // формируем сигнал START


	TWDR=WRITE_CMD;              // Загружаем адрес МС (WRITE)
	I2C_COMAND(I2C_TRANSMIT);    // Передаем
	
	TWDR=REG_CMD;                // Загружаем адрес регистра который будем писать
	I2C_COMAND(I2C_TRANSMIT);    // Передаем
	
	
	TWDR=data;                  // Загружаем 8 бит данных в регистр
	I2C_COMAND(I2C_TRANSMIT);   // Передаем

	I2C_COMAND(I2C_STOP);        //Формируем сигнал STOP
	_delay_ms(10);
}



uint8_t XRA1201_Chip(void)
{
	uint8_t data=0;
	if ((READ_REGISTER(IFR1_XRA)==0xFF) && (READ_REGISTER(FEIR1_XRA)==0x00)) data=1;
	return (data);                     // (должно вернуться 0x01)
}


void XRA1201_Reset(void)
{
	_delay_ms(1);
	WRITE_REGISTER(OCR1_XRA, 0x00);
	_delay_ms(1);
	WRITE_REGISTER(OCR2_XRA, 0x00);
	_delay_ms(1);
	WRITE_REGISTER(GCR1_XRA, 0xFF);
	_delay_ms(1);
	WRITE_REGISTER(GCR2_XRA, 0xFF);
	_delay_ms(1);
	WRITE_REGISTER(PIR1_XRA, 0x00);
	_delay_ms(1);
	WRITE_REGISTER(PIR2_XRA, 0x00);
	_delay_ms(1);
	WRITE_REGISTER(PUR1_XRA, 0x00);
	_delay_ms(1);
	WRITE_REGISTER(PUR2_XRA, 0x00);
	_delay_ms(1);
}


void XRA1201_SET_ONE(void)
{
	_delay_ms(1);
	XRA1201_Reset();
    _delay_ms(1);
    WRITE_REGISTER(GCR1_XRA, 0x00);	
	_delay_ms(1);
	WRITE_REGISTER(GCR2_XRA, 0x00);
	_delay_ms(1);
	WRITE_REGISTER(OCR1_XRA, 0xFF);
	_delay_ms(1);
	WRITE_REGISTER(OCR2_XRA, 0xFF);
	_delay_ms(1);
}


void XRA1201_SET_ZERO(void)
{
	_delay_ms(1);
	XRA1201_Reset();
	_delay_ms(1);
	WRITE_REGISTER(GCR1_XRA, 0x00);
	_delay_ms(1);
	WRITE_REGISTER(GCR2_XRA, 0x00);
	_delay_ms(1);
	WRITE_REGISTER(OCR1_XRA, 0x00);
	_delay_ms(1);
	WRITE_REGISTER(OCR2_XRA, 0x00);
}

void XRA1201_SET_INPUT(void)
{
	_delay_ms(1);
	XRA1201_Reset();
	_delay_ms(1);
	WRITE_REGISTER(GCR1_XRA, 0xFF);
	_delay_ms(1);
	WRITE_REGISTER(GCR2_XRA, 0xFF);
	_delay_ms(1);
	WRITE_REGISTER(OCR1_XRA, 0x00);
	_delay_ms(1);
	WRITE_REGISTER(OCR2_XRA, 0x00);
}
