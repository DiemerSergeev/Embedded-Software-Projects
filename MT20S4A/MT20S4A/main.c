#include "lcd.h"
#include "preference.h"
#include <time.h>

char buffer [32];				 // для вывода на дисплей
volatile short int count = 0;	 // переменная режима
char adress = 0;

//J1:
//	+5V_VCC(2,19)-WH1602/WH1604
//	+5V_VDD(3,20)-BACKLIGHT 5V
//	+5V_CONST(16,33)-MK+Vo
//	OUT1-(6)-DO1(PD2)
//	OUT2-(7)-DO2(PD3)
//	IN-OUT1-(23)-DI1(PB3) порты на шине

//J3: 
//	PIN01-18-DB0
//	PIN02-19-DB1
//	PIN03-20-DB2
//	PIN04-21-DB3
//	PIN05-22-DB4
//	PIN06-23-DB5
//	PIN07-24-DB6
//	PIN08-25-DB7

// IN-OUT1-23-DI1(PB3)
// Выход сигнала готовности принять данные от формулы 
// если на PB3-лог.1 мы готовы, лог.0 мы не готовы

// Реле 1 - PA5 переключает DB0,DB1 на формулу,по умолчанию на МК
// Реле 2 - PA6 переключает DB2,DB3 на формулу,по умолчанию на МК
// Реле 3 - PA4 переключает DB4,DB5 на формулу,по умолчанию на МК
// Реле 4 - PA2 переключает DB6,DB7 на формулу,по умолчанию на МК
// Реле 5 - PA7 переключает контрастность, по умолчанию разомкнута

// PA3-Светодиод 1
// PA1-Светодиод 2

// PC0..PC7 - DB_LCD
// PB3 - A0
// PD1 - RW
// PB0 - EN

//Режимы:										      OUT1-6-DO1(PD2)                     OUT2-7-DO2(PD3)
//0- измерение токов потребления BL и LCD+ФК)		  -									  -
//1- Измерение Uoh (1 на все DB и реле на формулу)	  -									  -
//2- Измерение Uol (0 на все DB и реле на формулу)    -								      -

///////////////////////////////////////////////////////////////////////
/*-----------------------Подпрограммы--------------------------------*/
///////////////////////////////////////////////////////////////////////
void Port_init(void)
{
	PORTA = 0x00;
	DDRA  = (1<<7)|(1<<6)|(1<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0);  //DDRA
	
	PORTB = 0x00;
	DDRB  = (0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<3)|(0<<2)|(0<<1)|(1<<0);  //DDRB
	
	PORTC = 0x00;
	DDRC  = (1<<7)|(1<<6)|(1<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0);  //DDRC
	
	PORTD = 0x00;
	DDRD  = (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(1<<1)|(0<<0);  //DDRD
	
	PORTE = 0x00;
	DDRE  = (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  //DDRE
	
	PORTF = 0x00;
	DDRF  = (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  //DDRF
	
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
	PORTA &= 0b00000000;    // все реле включены
	_delay_ms(50);
	PORTA |= 0b10000000;    // все реле выключены, кроме контрастности
	_delay_ms(50);
	PORTA &= 0b10000000;    // все реле выключены, кроме контрастности
	//PORTA &= 0b00000000;    // все реле выключены, кроме контрастности
	_delay_ms(50);
}

void blink1 (uint8_t count)
{
	for (uint8_t i=0; i<count; i++) 
	{
		LED1_ON;
		LED2_ON;
		_delay_ms(200);
		LED1_OFF;
		LED2_OFF;
		_delay_ms(200);
	}
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
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
/************************************************************************/
/* Основная программа                                                   */
/************************************************************************/
int main(void)
{
	
	Port_init();					//Инициализируем порты
	LCD_init();						//Инициализируем дисплей
	Rele_init();					//Инициализируем порты реле
	INT_ini();
		
	Ready_OFF;
	LED1_ON;
	LED2_ON;
	_delay_ms(100);
	
/************************************************************************/
/* ФУНКЦИОНАЛЬНЫЙ КОНТРОЛЬ (ИГРУШКИ)                                    */
/************************************************************************/	

	clearlcd();
	setpos(3, 0);
	str_lcd("PACMAN READY");
	_delay_ms(500);
	clearlcd();
	setpos(4, 0);
	str_lcd("GAME START");
	_delay_ms(200);
	setpos(9, 1);
	str_lcd("1");
	_delay_ms(200);
	setpos(9, 2);
	str_lcd("2");
	_delay_ms(200);
	setpos(9, 3);
	str_lcd("3");
	_delay_ms(200);
		
	clearlcd();

	for (int i=0; i<4; i++)
	{
		for (int j=0; j<20; j++)
		{
			setpos(j, i);
			WRITE(0x91);	
		}
	}
	setpos(0, 0);
	WRITE(0xA9);
	_delay_ms(500);
		
	for (int i=0; i<4; i++)
	{
		for (int j=1; j<20; j++)
		{
			setpos(j, i);
			WRITE(0xA9);
			_delay_ms(10);
						
			setpos(j-1, i);
			WRITE(0x20);
			_delay_ms(10);
		}
		setpos(19, i);
		WRITE(0x20);
	}

	clearlcd();
	setpos(0, 0);
	str_lcd("GAME OVER");
	setpos(0, 1);
	str_lcd("START WORKING");
	_delay_ms(500);
	
	count = 0;
	sei();
	
	 while(1)
	{
/************************************************************************/
/* Выключенный режим, ожидание команд                                   */
/************************************************************************/
		if (count==0)
		{	
			cli();
			
			A00;
			_delay_ms(2);
			
			READWRITE0;
			_delay_ms(2);

			ENABLE0;
			_delay_ms(2);
			
			clearlcd();
			setpos(0, 0);
			str_lcd("MODE 0");

			setpos(0, 1);
			str_lcd("Ждем команды сверху,");

			setpos(0, 2);
			str_lcd("А пока отдыхаем");

			setpos(0, 3);
			str_lcd("И пьем чай");
			
			sei();
			Ready_ON;
			
			LED1_OFF;
			LED2_OFF;
			
			while (count==0)
			{
				asm("nop");
				asm("nop");
				if(count!=0) break;
			}
		}
/************************************************************************/
/* Режим 1 - Измерение Uoh                                              */
/************************************************************************/
		if (count==1)
		{
			cli();
			Ready_OFF;
			
			rele_1_OFF;
			rele_2_OFF;
			rele_3_OFF;
			rele_4_OFF;
			
			blink1(count); 
			LED1_ON;
			
			READWRITE0;
			_delay_ms(2);
			A00;
			_delay_ms(2);
			ENABLE0;
			_delay_ms(2);
			
			sei();
			
			clearlcd();
			setpos(0, 0);
			str_lcd("MODE 1");

			setpos(0, 1);
			str_lcd("ВЫСОКИЙ УРОВЕНЬ UOH");
			_delay_ms(500);
			
			clearlcd();
					
			adress = 0;
			for (uint8_t i=0; i<0x68; i++)
			{
				A00;
				_delay_ms(2);
				
				READWRITE0;
				_delay_ms(2);
				
				ENABLE1;
				_delay_ms(2);
				
				adress = i|0b10000000;
				sendbyte(adress);
				
				ENABLE0;
				_delay_ms(2);
				
				A00;
				_delay_ms(2);
				
				READWRITE0;
				_delay_ms(2);
				
				A01;
				_delay_ms(2);
				
				READWRITE0;
				_delay_ms(2);
				
				ENABLE1;
				_delay_ms(2);
				
				PORTC &= 0b00000000;
				PORTC |= 0xFF;
				
				ENABLE0;
				_delay_ms(2);
				
				A00;
				_delay_ms(2);
				
				READWRITE0;
				_delay_ms(2);
			}
				
			//DDRC &= 0x00;
			_delay_ms(100);
			
			A01;
			_delay_ms(2);
						
			READWRITE1;
			_delay_ms(2);
			
			ENABLE1;
			_delay_ms(2);
			
			rele_1_ON;
			_delay_ms(100);
			rele_2_ON;
			_delay_ms(100);
			rele_3_ON;
			_delay_ms(100);
			rele_4_ON;
			_delay_ms(100);
			
			Ready_ON;
			_delay_ms(3000);
			
			A00;
			_delay_ms(2);
			
			READWRITE0;
			_delay_ms(2);
						
			ENABLE0;
			_delay_ms(2);
			
			rele_1_OFF;
			_delay_ms(100);
			rele_2_OFF;
			_delay_ms(100);
			rele_3_OFF;
			_delay_ms(100);
			rele_4_OFF;
			_delay_ms(100);
					
			clearlcd();
			_delay_ms(1000);
			
			count = 0;
			LED1_OFF;
			LED2_OFF;
			sei();
		}

/************************************************************************/
/* Режим 2 - Измерение Uol                                              */
/************************************************************************/
		if (count==2)
		{

			cli();
			Ready_OFF;

			rele_1_OFF;
			rele_2_OFF;
			rele_3_OFF;
			rele_4_OFF;

			blink1(count);
			LED2_ON;

			READWRITE0;
			_delay_ms(2);
			A00;
			_delay_ms(2);
			ENABLE0;
			_delay_ms(2);

			sei();

			clearlcd();
			setpos(0, 0);
			str_lcd("MODE 2");

			setpos(0, 1);
			str_lcd("НИЗКИЙ УРОВЕНЬ UOL");
			_delay_ms(1000);
			clearlcd();

			adress = 0;
			for (uint8_t i=0; i<0x68; i++)
			{
				A00;
				_delay_ms(2);
	
				READWRITE0;
				_delay_ms(2);
	
				ENABLE1;
				_delay_ms(2);
	
				adress = i|0b10000000;
				sendbyte(adress);
	
				ENABLE0;
				_delay_ms(2);
	
				A00;
				_delay_ms(2);
	
				READWRITE0;
				_delay_ms(2);
	
				A01;
				_delay_ms(2);
	
				READWRITE0;
				_delay_ms(2);
	
				ENABLE1;
				_delay_ms(2);
	
				PORTC &= 0b00000000;
				PORTC |= 0x00;
	
				ENABLE0;
				_delay_ms(2);
	
				A00;
				_delay_ms(2);
	
				READWRITE0;
				_delay_ms(2);
			}

			//DDRC &= 0x00;
			_delay_ms(100);

			A01;
			_delay_ms(2);

			READWRITE1;
			_delay_ms(2);

			ENABLE1;
			_delay_ms(2);

			rele_1_ON;
			_delay_ms(100);
			rele_2_ON;
			_delay_ms(100);
			rele_3_ON;
			_delay_ms(100);
			rele_4_ON;
			_delay_ms(100);

			Ready_ON;
			_delay_ms(3000);

			A00;
			_delay_ms(2);

			READWRITE0;
			_delay_ms(2);

			ENABLE0;
			_delay_ms(2);

			rele_1_OFF;
			_delay_ms(100);
			rele_2_OFF;
			_delay_ms(100);
			rele_3_OFF;
			_delay_ms(100);
			rele_4_OFF;
			_delay_ms(100);

			clearlcd();
			_delay_ms(1000);

			count = 0;
			LED1_OFF;
			LED2_OFF;
			sei();
}

/************************************************************************/
/* Ошибки                                                               */
/************************************************************************/
		if (count>2)
		{
			cli();
			clearlcd();
			setpos(0, 0);
			str_lcd("Wrong");
			setpos(0, 1);
			str_lcd("Data");
			_delay_ms(500);
			count = 0;
			sei();
		}
	}
}