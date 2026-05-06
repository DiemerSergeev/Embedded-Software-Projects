 // там частота тактирования
#include "i2c.h"        

#if defined(__AVR_ATmega328P__)|(__AVR_ATmega328__)
#else
//#error "Change I2C Port pleeeease"
#endif

uint8_t error_message=0;// сюда вернем ошибку
/*------------------------------------------------------------------------------------------
Функции:
I2C_Init (250000);                 Инициализация I2C,частота 250000 Гц по умолчанию SDA - PC4, SCL - PC5 
I2C_Start();                       Старт кадр
I2C_Stop();                        Стоп кадр
I2C_TransmitByte(0x07);            Отправка 8 бит данных
x=I2C_ReceiveByte();               Приём 8 бит данных + ACK, ACK-мы ждём ещё данных
y=I2C_ReceiveLastByte();           Приём 8 бит данных + NACK, NACK-приняли последние 8 бит,данные больше не требуются
z=I2C_ErrorMessage();              Сообщения об ошибках в шине I2C, обязательно проверять
//------------------------------------------------------------------------------------------
Значения i2c_clock_HZ для I2C:
MIN для кварца 8 МГц - 15209 Гц
MIN для кварца 16 МГц - 30418 Гц
MAX для кварца 8 МГц - 500000 Гц
MAX для кварца 16 МГц - 1000000 Гц
-------------------------------------------------------------------------------------------*/
void I2C_INIT(void)
{
	DDRC|=0b00000011;	// SDA - PC1, SCL - PC0 на выход
	
	TWSR &= (0<<TWPS0)|(0<<TWPS0); // биты предделителя настроены на 1
	TWBR=0x48;                     //Настраиваем частоту шины на 50 кГц

}

uint8_t I2C_Start(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	for(uint16_t i=0; i<0xFFFF; i++)    // проверяем установку TWINT-аппаратно выставляемый флаг завершения операции
	{
		_delay_us(10);
		if	((TWCR & (1<<TWINT))) break;
	}
	//while(!(TWCR&(1<<TWINT)));
	error_message=TWSR & 0xF8;
	return error_message;
}

void I2C_Stop(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

void I2C_TransmitByte(unsigned char c)
{
	TWDR=c;
	TWCR = (1<<TWINT)|(1<<TWEN);
	for(uint16_t i=0; i<0xFFFF; i++)    // проверяем установку TWINT-аппаратно выставляемый флаг завершения операции
	{
		_delay_us(10);
		if	((TWCR & (1<<TWINT))) break;
	}
	//while(!(TWCR&(1<<TWINT)));
	error_message=TWSR & 0xF8;
}


unsigned char I2C_ReceiveByte(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	for(uint16_t i=0; i<0xFFFF; i++)       // проверяем установку TWINT-аппаратно выставляемый флаг завершения операции
	{
		_delay_us(10);	
		if	((TWCR & (1<<TWINT))) break;
	}
	//while(!(TWCR & (1<<TWINT)));
	error_message=TWSR & 0xF8;
	return TWDR;
}

unsigned char I2C_ReceiveLastByte(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN);
	for(uint16_t i=0; i<0xFFFF; i++)       // проверяем установку TWINT-аппаратно выставляемый флаг завершения операции
	{
		_delay_us(10);
		if	((TWCR & (1<<TWINT))) break;
	}
	//while(!(TWCR&(1<<TWINT)));
	error_message=TWSR & 0xF8;
	return TWDR;
}

unsigned char I2C_ErrorMessage(void)
{
/*---------------Коды ошибок возвращаемые шиной I2C в error_message-----------------*/	
// Bus_Fail           0x00  // Аппаратная ошибка шины,ошибка
// Start              0x08  // Был сделан старт
// ReStart            0x10  // Был обнаружен повторный старт
// SLA_W_ACK          0x18  // Отправили адрес с битом записи, а в ответ получили ACK от ведомого,всё хорошо
// SLA_W_NACK         0x20  // Мы отправили адрес с битом записи, а нас послали NACK,ошибка
// Byte_ACK           0x28  // Мы послали байт и получили подтверждение, что ведомый его принял
// Byte_NACK          0x30  // Мы послали байт, но подтверждение не получили,ошибка
// Collision          0x38  // Пришел другой мастер, по хамски нас перебил, да так, что мы от возмущения аж заткнулись,ошибка
// SLA_R_ACK          0x40  // Послали адрес с битом на чтение и ведомый отозвался
// SLA_R_NACK         0x48  // На запрос адреса с битом чтения никто не откликнулся,ошибка
// Receive_Byte       0x50  // Мы приняли байт. И думаем что бы ответить ведомому. ACK или NACK.
// Receive_Byte_NACK  0x58  // Мы приняли байт от ведомого и сказали ему «иди NACK!» И он обиженый ушел, освободив шину
	
	return error_message;
}