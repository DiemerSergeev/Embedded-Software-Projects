//#include "preference.h"
#include "lcd.h"
char buffer [32];				 // для вывода на дисплей
volatile short int count=0;		 // переменная режима
char adress=0;

//J1:+5V_VCC(2,19)-WH1602/WH1604, +5V_VDD(3,20)-BACKLIGHT 5V, +5V_CONST(16,33)-MK+Vo, OUT1-(6)-DO1(PD2), OUT2-(7)-DO2(PD3); IN-OUT1-(23)-DI1(PB3) порты на шине
//J3: PIN01-18-DB0, PIN02-19-DB1, PIN03-20-DB2, PIN04-21-DB3, PIN05-22-DB4, PIN06-23-DB5, PIN07-24-DB6, PIN08-25-DB7

//IN-OUT1-23-DI1(PB3)- Выход сигнала готовности принять данные от формулы если на PB3-лог.1 мы готовы, лог.0 мы не готовы

// Реле 1- PD7 переключает DB0,DB1 на формулу,по умолчанию на МК
// Реле 2- PD6 переключает DB2,DB3 на формулу,по умолчанию на МК
// Реле 3- PD5 переключает DB4,DB5 на формулу,по умолчанию на МК
// Реле 4- PD4 переключает DB6,DB7 на формулу,по умолчанию на МК

// PD0-Светодиод 1
// PD1-Светодиод 2

// PA0..PA7-DB_LCD
// PB0 - RS
// PB1 - RW
// PB2 - EN


//Режимы:										      OUT1-6-DO1(PD2)                     OUT2-7-DO2(PD3)
//0- измерение токов потребления BL и LCD+ФК)		  -									  -
//1- Измерение Uoh (1 на все DB и реле на формулу)	  -									  -
//2- Измерение Uol (0 на все DB и реле на формулу)    -								      -


/*-----------------------Подпрограммы--------------------------------*/
void Port_init(void)
{
	PORTA=0x00;
	DDRA= (1<<7)|(1<<6)|(1<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0);  //DDRA
	
	PORTB=0x00;
	DDRB= (0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0);  //DDRB
	
	PORTC=0x00;
	DDRC= (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  //DDRC
	
	PORTD=0x00;
	DDRD= (1<<7)|(1<<6)|(1<<5)|(1<<4)|(0<<3)|(0<<2)|(1<<1)|(1<<0);  //DDRD
	
}

void INT_ini(void)
{
	
	PORTD&=0b11110011;				   // PD2, PD3 на вход
	EICRA |= (1<<ISC01) | (1<<ISC00);  // прерывание по фронту
	EIMSK |= (1<<INT0);				   // PD2- INT0
	EIFR |= (1<<INTF0);				   // сброс флага прерывания
	
}


void Rele_init(void)
{
	DDRD|=0b11110011;	// PD4-PD7;
	PORTD&=0b00001100;  // все реле выключены
}

void blink1 (uint8_t count)
{
	for (uint8_t i=0; i<count; i++) 
	{
		LED1_ON;
		_delay_ms(200);
		LED1_OFF;
		_delay_ms(200);
	}
}

void blink2 (uint8_t count)
{
	for (uint8_t i=0; i<count; i++) {
		LED2_ON;
		_delay_ms(200);
		LED2_OFF;
	_delay_ms(200);}
}


//-------------------------Прерывания--------------------------------------
ISR(INT0_vect)	// прерывание при приходе на PD2 лог. 1
{
	cli();
	Ready_OFF;                              // Мы не готовы принимать данные от формулы
	LED1_OFF;                               // Выключить светодиод LED1
	//LED2_ON;                                // Включить Led2
	
	if (!(PIND&0b00000100)) goto END;       // проверяем  PD2 на лог 1
	_delay_ms(8);
	if (!(PIND&0b00000100)) goto END;       // проверяем  PD2 на лог 1 через 5 мс если есть то проверяем РD3
	count=0;
	while ((PIND&0b00000100))               // сидим в цикле пока на PD2 лог. 1
	{
		if ((PIND&0b00001000))              // проверяем  PD3 на лог 1
		{
			_delay_ms(5);
			if ((PIND&0b00001000))	++count;        // проверяем  PD3 на лог 1 через 5 мс если есть то count+1 и ждём спада импульса
			while ((PIND&0b00001000)) _delay_ms(5);
		}
		
		asm("nop");
	}
	
	END:
	EIFR |= (1<<INTF0);  // сброс флага прерывания
	sei();
}



int main(void)
{
	/*---------------Инициализация и настройки периферии---------------------*/
	Port_init();					//Инициализируем порты
	
	LCD_init();						//Инициализируем дисплей
	
	Rele_init();					//Инициализируем порты реле
		
	Ready_OFF;                      //Мы не готовы принять данные от формулы
	LED1_ON;                       //Выключить светодиод LED1
	LED2_ON;                       //Выключить светодиод LED2
	_delay_ms(10);

	//-------------------------------------------------------------------------
	
	clearlcd();
	setpos(0, 0);
	str_lcd("  ГК СНАБЖЕНИЕ");
	setpos(0, 1);
	str_lcd(" LOADING 90%...");
	_delay_ms(1000);
	
	clearlcd();
	setpos(0, 0);
	str_lcd("  ГК СНАБЖЕНИЕ");
	setpos(0, 1);
	str_lcd(" LOADING 95%...");
	_delay_ms(1000);
	
	clearlcd();
	setpos(0, 0);
	str_lcd("  ГК СНАБЖЕНИЕ");
	setpos(0, 1);
	str_lcd(" LOADING 99%...");
	_delay_ms(1000);
	
	count=0;						//После включения попадаем на 0 режим
	INT_ini();						//Инициализируем прерывания
	sei();
	
  
	//----------------------------------------------- Основной цикл-----------------------------------------------
	
	 while(1)					   // Бесконечный цикл
	{
		//----------Режим0-----------
		if (count==0)              // Режим 0 мк не работает
		{	
			cli();
			rele_1_OFF;
			rele_2_OFF;
			rele_3_OFF;
			rele_4_OFF;
			clearlcd();
			setpos(0, 0);
			adress=0;
			for (uint8_t i=0; i<0x60; i++)
				{
				adress=i|0b10000000;
				 sendbyte(adress, 0);
					rs1;
					e1; //включаем линию Е
					_delay_ms(2);
					PORTA&=0b00000000;
					PORTA|=0xFF;
					e0; //выключаем линию Е
					_delay_ms(2);
			}
			sei();
			Ready_ON;                     // Мы готовы принять данные от формулы
			LED1_ON;                      // Включить Led1
			LED2_OFF;                     // Выключить Led2
			while (count==0)
			{
				asm("nop");
				asm("nop");
				if(count!=0) break;
			}
		}
	
			
		//----------Режим1-----------
		if (count==1)				    // Режим 1 измерение Uoh
		{
			cli();
			rele_1_OFF;
			rele_2_OFF;
			rele_3_OFF;
			rele_4_OFF;
			blink2(count); 
			RW0;
			clearlcd();
			setpos(0, 0);
			sei();
				clearlcd();
				setpos(0, 0);
				for (uint8_t i=0; i<16; i++)
				{  for (uint8_t j=0; j<4; j++)
					{
						setpos(i, j);
						rs1;
						e1; //включаем линию Е
						_delay_ms(2);
						PORTA&=0b00000000;
						PORTA|=0xFF;
						e0; //выключаем линию Е
						_delay_ms(2);
					}
				}
				RW0;
				rs0;
				DDRA=0x00;
				RW1;
				rs1;
				e1;
				_delay_ms(100);
				rele_1_ON;
				rele_2_ON;
				rele_3_ON;
				rele_4_ON;
				Ready_ON;                     // Мы готовы принять данные от формулы
				_delay_ms(4000);
				e0;
				rele_1_OFF;
				rele_2_OFF;
				rele_3_OFF;
				rele_4_OFF;
				DDRA=0xFF;
				Ready_OFF;                    // Мы не готовы принимать данные от формулы
				count=0;
				LED1_ON;                      // Включить светодиод Led1
				LED2_OFF;                     // Выключить Led2
				sei();
		}

		//----------Режим2-----------
		if (count==2)				    // Режим 2 измерение Uol
			{
				cli();
				rele_1_OFF;
				rele_2_OFF;
				rele_3_OFF;
				rele_4_OFF;
				blink2(count);
				RW0;
				clearlcd();
				setpos(0, 0);
				sei();
				clearlcd();
				setpos(0, 0);
				for (uint8_t i=0; i<16; i++)
				{  for (uint8_t j=0; j<4; j++)
					{
						setpos(i, j);
						rs1;
						e1; //включаем линию Е
						_delay_ms(2);
						PORTA&=0b00000000;
						PORTA|=0x00;
						e0; //выключаем линию Е
						_delay_ms(2);
					}
				}
				RW0;
				rs0;
				DDRA=0x00;
				RW1;
				rs1;
				e1;
				_delay_ms(100);
				rele_1_ON;
				rele_2_ON;
				rele_3_ON;
				rele_4_ON;
				Ready_ON;                     // Мы готовы принять данные от формулы
				_delay_ms(4000);
				e0;
				rele_1_OFF;
				rele_2_OFF;
				rele_3_OFF;
				rele_4_OFF;
				DDRA=0xFF;
				Ready_OFF;                    // Мы не готовы принимать данные от формулы
				count=0;
				LED1_ON;                      // Включить светодиод Led1
				LED2_OFF;                     // Выключить Led2
				sei();
			}
		


		//----------Ошибки-----------
		if (count>2)              // Обработчик ошибок если от формулы пришла команда больше 2
		{
			cli();
			clearlcd();
			setpos(0, 0);
			str_lcd("Wrong");
			setpos(0, 1);
			str_lcd("Data");
			_delay_ms(500);
			count=0;
			sei();
		}
	}// конец while(1)


}// конец int main(void)





