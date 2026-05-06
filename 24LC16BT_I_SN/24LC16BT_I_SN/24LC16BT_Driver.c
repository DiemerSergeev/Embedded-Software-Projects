#include "preference.h"
#include <util/delay.h>
#include "24LC16BT_Driver.h"

/************************************************************************/
/* Инициализация интерфейса I2C                                         */
/************************************************************************/
void I2C_INIT(void)
{
	DDRD |=0b00000011;	// SDA - PD1, SCL - PD0 на выход
	PORTD &= 0xDF;	
	DDRD  |= 0x20;   
	
	TWSR  |= (0<<TWPS1)|(0<<TWPS0);  // Биты предделителя настроены на 1
	TWBR  = 0x02;                     // Настраиваем частоту шины на 400 кГц
}
/************************************************************************/
/* Команды интефейса I2C, выставление регистров                         */
/************************************************************************/
uint8_t I2C_COMAND(uint8_t action)
{
	switch(action){
		case I2C_START:
		TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
		case I2C_RESTART:
		TWCR = _BV(TWSTA) | _BV(TWEN) | _BV(TWINT);// Если нужно прерывание | _BV(TWIE);
		break;
		case I2C_STOP:
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
		break;
		case I2C_TRANSMIT:
		//TWCR = _BV(TWEN) | _BV(TWINT);// | _BV(TWIE);
		TWCR = (1<<TWINT) |(1<<TWEN);
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
/* Чтение последовательности                                            */
/************************************************************************/
uint8_t READ_ONE_SEQUENTIAL(uint8_t CONTROL_BYTE,uint8_t data)
{
	uint8_t result = 0;

	I2C_COMAND(I2C_START);
	while (!(TWCR &(1<<TWINT)));        // формируем сигнал START
	
	TWDR = CONTROL_BYTE&0b11111110;     // Загружаем адрес МС (WRITE)
	I2C_COMAND(I2C_TRANSMIT);		    // Передаем
	while (!(TWCR &(1<<TWINT)));
		
	TWDR = 0x00;						// Отправка адреса ячейки
	I2C_COMAND(I2C_TRANSMIT);    
	while (!(TWCR &(1<<TWINT)));        // Ожидание ACK/NACK
		
	I2C_COMAND(I2C_START);              // формируем сигнал START
	while (!(TWCR &(1<<TWINT)));
		
	TWDR = CONTROL_BYTE;                // Загружаем адрес МС (READ)
	I2C_COMAND(I2C_TRANSMIT);           // Передаем
	while (!(TWCR &(1<<TWINT)));
	
	for (int i=0; i<15; i++)
	{
		I2C_COMAND(I2C_RECEIVE_ACK);
		while (!(TWCR &(1<<TWINT)));
		
		result = TWDR;
		
		if (result != data) return 0;
		
	}
	I2C_COMAND(I2C_RECEIVE_NACK);
	while (!(TWCR &(1<<TWINT)));
	
	I2C_COMAND(I2C_STOP);                  // формируем сигнал STOP
	return 1;
}
/************************************************************************/
/* Запись страницы байт в память                                        */
/************************************************************************/
void WRITE_PAGE(uint8_t CONTROL_BYTE, uint8_t DATA)
{
	TWDR = 0x00;     
	TWDR = 0x00;                 // Загружаем SLAVE-ADRESS
	I2C_COMAND(I2C_STOP);        // Формируем сигнал STOP
	I2C_COMAND(I2C_START);       // формируем сигнал START
	TWDR = CONTROL_BYTE;                 // Загружаем SLAVE-ADRESS
	I2C_COMAND(I2C_TRANSMIT);
	while (!(TWCR &(1<<TWINT)));
	
	TWDR = 0x00;              // Загружаем адрес регистра который будем писать
	I2C_COMAND(I2C_TRANSMIT);    // Передаем
	while (!(TWCR &(1<<TWINT)));

	for (int i=0; i<16; i++)
	{
		TWDR = DATA;                 // Загружаем 8 бит данных в регистр
		I2C_COMAND(I2C_TRANSMIT);    // Передаем
		while (!(TWCR &(1<<TWINT)));
		
	}
	I2C_COMAND(I2C_STOP);// ОБЯЗАТЕЛЕН, ПРОИЗВОДИТСЯ ЗАПИСЬ В ПАМЯТЬ ПОСЛЕ СИГНАЛА СТОП

}
uint8_t CHECK_SCHEME(uint8_t CONTROL_BYTE_WRITE, uint8_t CONTROL_BYTE_READ, uint8_t data)
{
	uint8_t result;
	LED1_ON;
	LED2_OFF;
	WRITE_PAGE(CONTROL_BYTE_WRITE, data);
	LED1_OFF;
	LED2_ON;
	_delay_ms(5); // МИНИМАЛЬНОЕ ВРЕМЯ ЗАПИСИ В ПАМЯТЬ
	
	result = READ_ONE_SEQUENTIAL(CONTROL_BYTE_READ, data);
	return result;
}