#include "preference.h"
#include "24LC16BT_Driver.h"

volatile short int count = 0;		 // переменная режима

/*  
	J1:
		SDA   (1,18) DATA ПИН I2C
		SCL   (2,19) ПИН ТАКТИРОВАНИЯ
		WP    (3,20) ПИН WRITE PROTECT
		A0    (4,21) НЕ ИСПОЛЬЗУЕТСЯ
		A1    (5,22) НЕ ИСПОЛЬЗУЕТСЯ
		A2    (6,23) НЕ ИСПОЛЬЗУЕТСЯ

	Реле 1- PA2 переключает SDA,SCL на формулу,по умолчанию на 3.3V MC
	Реле 2- PA4 переключает ADDR0 и ADDR1 на формулу,по умолчанию на GND
	Реле 3- PA6 переключает ADDR2 и WP на формулу,по умолчанию по умолчанию на GND  и 3.3V MC соответственно

	PA3-Светодиод 1
	PA1-Светодиод 2
	
	Режимы:									
		0 - МК выключен: Ничего не подаём (ждущий режим 24LC16BT)
		1 - Операция записи во все блоки памяти значения 0х00
		2 - Операция записи во все блоки памяти значения 0хFF
		3 - Операция чтения всех блоков памяти
		4 - Включение реле 1
		5 - Включение реле 2
		6 - Включение реле 3
		7 - Выключение всех реле

*/

void Port_init(void)
{
	PORTA = 0x00;
	DDRA = (0<<7)|(1<<6)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0);  //DDRA
	
	PORTB = 0x00;
	DDRB = (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  //DDRB
	
	PORTC = 0x00;
	DDRC = (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  //DDRC
	
	PORTD = 0x00;
	DDRD = (0<<7)|(0<<6)|(1<<5)|(0<<4)|(0<<3)|(0<<2)|(1<<1)|(1<<0);  //DDRD
	
	PORTE = 0x00;
	DDRE = (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  //DDRE
	
	PORTF = 0x00;
	DDRF = (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  //DDRF
	
	PORTG = 0x00;
	DDRG = (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  //DDRG
	
}

void INT_ini(void)
{
	PORTE&=0b10111111;				   // PE6 на вход
	PORTF&=0b11111101;				   // PF1 на вход
	
	EICRB |= (1<<ISC61) | (1<<ISC60);  // прерывание по фронту
	EIMSK |= (1<<INT6);				   // PE6- INT6
	EIFR  |= (1<<INTF6);			   // сброс флага прерывания
}

void Rele_init(void)
{
	DDRA|=0b01010100;	
	PORTA&=0b10101011;  // все реле выключены
}

void Problem(void)
{
	LED2_OFF;                              //Выключить светодиод LED2
	Ready_OFF;                             //Мы не готовы принять данные от формулы
	while(1)
	{
		LED1_ON;
		LED2_ON;                                 // Включить светодиод LED1
		_delay_ms(300);
		LED1_OFF;
		LED2_OFF;                                // Выключить светодиод LED1
		_delay_ms(300);
	}
}

ISR(INT6_vect)	// прерывание при приходе на PE6 лог. 1
{
	cli();
	Ready_OFF;                              // Мы не готовы принимать данные от формулы
	LED1_OFF;                               // Выключить светодиод LED1
	LED2_ON;                                // Включить Led2
	
	if (!(PINE&0b01000000)) goto END;       // проверяем  PE6 на лог 1
	_delay_ms(8);
	if (!(PINE&0b01000000)) goto END;       // проверяем  PE6 на лог 1 через 5 мс если есть то проверяем РD6
	count=0;
	while ((PINE&0b01000000))               // сидим в цикле пока на PE6 лог. 1
	{
		if ((PINF&0b00000010))              // проверяем  PF1 на лог 1
		{
			_delay_ms(5);
			if ((PINF&0b00000010))	++count;        // проверяем  PF1 на лог 1 через 5 мс если есть то count+1 и ждём спада импульса
			while ((PINF&0b00000010)) _delay_ms(5);
		}
		
		asm("nop");
	}
	
	END:
	EIFR |= (1<<INTF6);  // сброс флага прерывания
	sei();
}

int main(void)
{
/************************************************************************/
/* Инициализация и настройки периферии                                  */
/************************************************************************/
	Port_init();					// Инициализируем порты
	_delay_ms(10);
	UART_init(9600);                // Отладка с помощью UART
	UART_Transmit_Str("Start 24LC16BT", end_string);
	_delay_ms(10);
	Rele_init();					// Инициализируем порты реле
	_delay_ms(10);
	I2C_INIT();
	_delay_ms(10);
	Ready_OFF;                      // Мы не готовы принять данные от формулы
	LED1_OFF;                       // Выключить светодиод LED1
	LED2_OFF;                       // Выключить светодиод LED2
	_delay_ms(10);
	
/********************************************************************************/
/* Проверка наличия ЭКБ                                                         */
/* Проверка осуществляется записью и чтением в первый блок памяти значений 0xFF */
/********************************************************************************/	
	uint8_t data = 0;
	
	uint8_t chip_find = CHECK_SCHEME(WRITE_FIRST_BLOCK, READ_FIRST_BLOCK, 0xFF); // Поиск 24LC16BT
		if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
		chip_find = CHECK_SCHEME(WRITE_SECOND_BLOCK, READ_SECOND_BLOCK, 0xFF); // Поиск 24LC16BT
		if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
		chip_find = CHECK_SCHEME(WRITE_THIRD_BLOCK, READ_THIRD_BLOCK, 0xFF); // Поиск 24LC16BT
		if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
		chip_find = CHECK_SCHEME(WRITE_FOURTH_BLOCK, READ_FOURTH_BLOCK, 0xFF); // Поиск 24LC16BT
		if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
		chip_find = CHECK_SCHEME(WRITE_FIFTH_BLOCK, READ_FIFTH_BLOCK, 0xFF); // Поиск 24LC16BT
		if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
		chip_find = CHECK_SCHEME(WRITE_SIXTH_BLOCK, READ_SIXTH_BLOCK, 0xFF); // Поиск 24LC16BT
		if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
		chip_find = CHECK_SCHEME(WRITE_SEVENTH_BLOCK, READ_SEVENTH_BLOCK, 0xFF); // Поиск 24LC16BT
		if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
		chip_find = CHECK_SCHEME(WRITE_EIGHTH_BLOCK, READ_EIGHTH_BLOCK, 0xFF); // Поиск 24LC16BT
		if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует

	LED1_OFF;
	LED2_OFF;
	
	_delay_ms(100);

/************************************************************************/
/* Схема обнаружена, старт работы режимов                               */
/************************************************************************/
	count = 0;						//После включения попадаем на 0 режим
	INT_ini();						//Инициализируем прерывания
	sei();
	
	while(1)					   // Бесконечный цикл
	{
/************************************************************************/
/*	Режим 0 - ожидание выбора команды прерываниями от Формулы           */
/************************************************************************/
		if (count == 0)
		{
			cli();
			UART_Transmit_Str("Mode: 0", end_string);
			sei();
			Ready_ON;              // Мы готовы принять данные от формулы
			LED1_OFF;               // Включить Led1
			LED2_OFF;              // Выключить Led2
			
			while (count == 0) // Остаемся в цикле до тех пор, пока не поступит прерывание
			{
				asm("nop");
				asm("nop");
				if(count != 0) break;
			}
		}
/************************************************************************/
/* Режим 1 - Запись значений 0х00 в память                              */
/************************************************************************/
		if (count == 1)
		{
			cli();
			UART_Transmit_Str("Mode: 1", end_string);
			_delay_ms(100);
			sei();
			_delay_ms(100);
			LED1_ON;                      // Индикация ИДЕТ ЗАПИСЬ
			LED2_OFF;
			_delay_ms(100);
			data = 0x00;
			cli();
			WRITE_PAGE(WRITE_FIRST_BLOCK, data);   // Запись начинается с первого блока
			WRITE_PAGE(WRITE_SECOND_BLOCK, data);  // Запись начинается с первого блока
			WRITE_PAGE(WRITE_THIRD_BLOCK, data);   // Запись начинается с первого блока
			WRITE_PAGE(WRITE_FOURTH_BLOCK, data);  // Запись начинается с первого блока
			WRITE_PAGE(WRITE_FIFTH_BLOCK, data);   // Запись начинается с первого блока
			WRITE_PAGE(WRITE_SIXTH_BLOCK, data);   // Запись начинается с первого блока
			WRITE_PAGE(WRITE_SEVENTH_BLOCK, data); // Запись начинается с первого блока
			WRITE_PAGE(WRITE_EIGHTH_BLOCK, data);  // Запись начинается с первого блока	
			sei();
			
			Ready_ON;                     // Мы готовы принять данные от формулы
			LED1_ON;                      // Включить светодиод Led1
			LED2_ON;                     // Выключить Led2
			_delay_ms(1000);
			Ready_OFF;                    // Мы не готовы принимать данные от формулы
				
			asm("nop");
			
			count = 0;
		}
/************************************************************************/
/* Режим 2 - Запись значений 0хFF в память                              */
/************************************************************************/
		if (count == 2)
		{
			cli();
			UART_Transmit_Str("Mode: 2", end_string);
			_delay_ms(100);
			sei();
			_delay_ms(100);
			LED1_ON;                      // Индикация ИДЕТ ЗАПИСЬ
			LED2_OFF;
			_delay_ms(100);
			data = 0xFF;
			cli();
			WRITE_PAGE(WRITE_FIRST_BLOCK, data);   // Запись начинается с первого блока
			WRITE_PAGE(WRITE_SECOND_BLOCK, data);  // Запись начинается с первого блока
			WRITE_PAGE(WRITE_THIRD_BLOCK, data);   // Запись начинается с первого блока
			WRITE_PAGE(WRITE_FOURTH_BLOCK, data);  // Запись начинается с первого блока
			WRITE_PAGE(WRITE_FIFTH_BLOCK, data);   // Запись начинается с первого блока
			WRITE_PAGE(WRITE_SIXTH_BLOCK, data);   // Запись начинается с первого блока
			WRITE_PAGE(WRITE_SEVENTH_BLOCK, data); // Запись начинается с первого блока
			WRITE_PAGE(WRITE_EIGHTH_BLOCK, data);  // Запись начинается с первого блока
			sei();
			
			Ready_ON;                     // Мы готовы принять данные от формулы
			LED1_ON;                      // Включить светодиод Led1
			LED2_ON;                     // Выключить Led2
			_delay_ms(1000);
			Ready_OFF;                    // Мы не готовы принимать данные от формулы
			
			asm("nop");
			
			count = 0;
			
		}
/************************************************************************/
/* Режим 3 - Чтение данных из памяти                                    */
/************************************************************************/
		if (count == 3)
		{
			uint8_t read_data;
			cli();
			UART_Transmit_Str("Mode: 3", end_string);
			_delay_ms(100);
			sei();
			_delay_ms(100);
			LED1_OFF;                      // Индикация ИДЕТ ЧТЕНИЕ
			LED2_ON;
			_delay_ms(100);
			
			cli();
			for (int i=0; i<30; i++)
			{
				read_data = READ_ONE_SEQUENTIAL(READ_FIRST_BLOCK, data);
				UART_Transmit_Hex(read_data);
				_delay_ms(10);
			}
			
			sei();
			
			Ready_ON;                     // Мы готовы принять данные от формулы
			LED1_ON;                      // Включить светодиод Led1
			LED2_ON;                     // Выключить Led2
			_delay_ms(1000);
			Ready_OFF;                    // Мы не готовы принимать данные от формулы
			
			asm("nop");
			
			count = 0;
			
		} 
/************************************************************************/
/* Режим 4 - Включение реле 1                                           */
/************************************************************************/
		if (count == 4)
		{		
			cli();
			UART_Transmit_Str("Mode: 4", end_string);
			rele_SDASCL_ON;
			_delay_ms(100);
			sei();
			count = 0;
		}
/************************************************************************/
/* Режим 5 - Включение реле 2                                           */
/************************************************************************/
		if (count == 5)
		{
			cli();
			UART_Transmit_Str("Mode: 5", end_string);
			rele_ADDR01_ON;
			_delay_ms(100);
			sei();
			count = 0;
		}
/************************************************************************/
/* Режим 6 - Включение реле 3                                           */
/************************************************************************/
		if (count == 6)
		{
			cli();
			UART_Transmit_Str("Mode: 6", end_string);
			rele_ADDR2_WPP_ON;
			_delay_ms(100);
			sei();
			count = 0;
		}		
/************************************************************************/
/* Режим 7 - Выключение реле                                            */
/************************************************************************/
		if (count == 7)
		{
			cli();
			UART_Transmit_Str("Mode: 7", end_string);
			rele_SDASCL_OFF;
			_delay_ms(100);
			rele_ADDR01_OFF;
			_delay_ms(100);
			rele_ADDR2_WPP_OFF
			_delay_ms(100);
			sei();
			count = 0;
		}
/************************************************************************/
/* Режим 8 - Функциональный контроль                                    */
/************************************************************************/
		if (count == 8)
		{
			cli();
			UART_Transmit_Str("Mode: 8", end_string);

			chip_find = CHECK_SCHEME(WRITE_FIRST_BLOCK, READ_FIRST_BLOCK, 0xFF); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			chip_find = CHECK_SCHEME(WRITE_SECOND_BLOCK, READ_SECOND_BLOCK, 0xFF); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			chip_find = CHECK_SCHEME(WRITE_THIRD_BLOCK, READ_THIRD_BLOCK, 0xFF); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			chip_find = CHECK_SCHEME(WRITE_FOURTH_BLOCK, READ_FOURTH_BLOCK, 0xFF); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			chip_find = CHECK_SCHEME(WRITE_FIFTH_BLOCK, READ_FIFTH_BLOCK, 0xFF); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			chip_find = CHECK_SCHEME(WRITE_SIXTH_BLOCK, READ_SIXTH_BLOCK, 0xFF); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			chip_find = CHECK_SCHEME(WRITE_SEVENTH_BLOCK, READ_SEVENTH_BLOCK, 0xFF); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			chip_find = CHECK_SCHEME(WRITE_EIGHTH_BLOCK, READ_EIGHTH_BLOCK, 0xFF); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			
			chip_find = CHECK_SCHEME(WRITE_FIRST_BLOCK, READ_FIRST_BLOCK, 0x00); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			chip_find = CHECK_SCHEME(WRITE_SECOND_BLOCK, READ_SECOND_BLOCK, 0x00); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			chip_find = CHECK_SCHEME(WRITE_THIRD_BLOCK, READ_THIRD_BLOCK, 0x00); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			chip_find = CHECK_SCHEME(WRITE_FOURTH_BLOCK, READ_FOURTH_BLOCK, 0x00); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			chip_find = CHECK_SCHEME(WRITE_FIFTH_BLOCK, READ_FIFTH_BLOCK, 0x00); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			chip_find = CHECK_SCHEME(WRITE_SIXTH_BLOCK, READ_SIXTH_BLOCK, 0x00); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			chip_find = CHECK_SCHEME(WRITE_SEVENTH_BLOCK, READ_SEVENTH_BLOCK, 0x00); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			chip_find = CHECK_SCHEME(WRITE_EIGHTH_BLOCK, READ_EIGHTH_BLOCK, 0x00); // Поиск 24LC16BT
			if (chip_find == 0) Problem(); // Если данные записи и чтения не совпадают, схема повреждена или отсутствует
			
			sei();
			count = 0;
		}
/************************************************************************/
/* Режим ошибок                                                         */
/************************************************************************/
			if (count > 8)
			{
				cli();
				UART_Transmit_Str("Wrong mode", end_string);
				_delay_ms(100);
				count=0;
				sei();
			}
	}// конец while(1)
}// конец int main(void)









