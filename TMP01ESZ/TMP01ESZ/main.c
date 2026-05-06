#include "preference.h"
#include "TMP01ESZ_Driver.h"

char buffer_LCD [32];			 // для вывода на дисплей
volatile short int count=0;		 // переменная режима


//J3:+2.5V_REF(1,18)-TMP01ESZ, V_PTAT(FORMULA) (2,19), UNDER_F(3,20), OVER_F(4,21)

//IN-OUT1-23-DI1(PA0)- Выход сигнала готовности принять данные от формулы если на PA0-лог.1 мы готовы, лог.0 мы не готовы


// Реле 1- PA6 переключает OVER/UNDER на формулу,по умолчанию на МК
// Реле 2- PA4 переключает VPTAT на формулу или АЦП,по умолчанию на АЦП (VREF весит в воздухе по умолчанию, переключается на формулу)



// PA3-Светодиод 1
// PA1-Светодиод 2


//Режимы:										 OUT1-6-DO1(PE6)                     OUT2-7-DO2(PF1)
//0- МК выключен:								 Ничего не подаём                    запись: 0 - Ничего не подаём ждущий режим 
//1- Terr:		                                 Удерживаем 1 пока идёт запись       запись: 1 - Ошибка определения температуры          
//2- Vol:		                                 Удерживаем 1 пока идёт запись       запись: 2 - Vol на OVER/UNDER            
//3- VREF:		                                 Удерживаем 1 пока идёт запись       запись: 3 - Опорное напряжение            
//4- Icc:                                        Удерживаем 1 пока идёт запись       запись: 4 - Ток потребления               


/*-----------------------Подпрограммы--------------------------------*/
void Port_init(void)
{
	PORTA=0x00;
	DDRA= (0<<7)|(1<<6)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0);  //DDRA
	
	PORTB=0x00;
	DDRB= (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(1<<1)|(0<<0);  //DDRB
	
	PORTC=0x00;
	DDRC= (1<<7)|(1<<6)|(1<<5)|(1<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  //DDRC
	
	PORTD=0x00;
	DDRD= (1<<7)|(0<<6)|(0<<5)|(1<<4)|(1<<3)|(0<<2)|(1<<1)|(1<<0);  //DDRD
	
	PORTE=0x00;
	DDRE= (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  //DDRE
	
	PORTF=0x00;
	DDRF= (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  //DDRF
	
	PORTG=0x00;
	DDRG= (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  //DDRG
	
}

void INT_ini(void)
{
	
	PORTE&=0b10111111;				   // PE6 на вход
	PORTF&=0b11111101;				   // PF1 на вход
	
	EICRB |= (1<<ISC61) | (1<<ISC60);  // прерывание по фронту
	EIMSK |= (1<<INT6);				   // PE6- INT6
	EIFR |= (1<<INTF6);				   // сброс флага прерывания
}


void Rele_init(void)
{
	DDRA|=0b01010000;	
	PORTA&=0b10101111;  // все реле выключены
}


void Problem(void)
{
	LED2_OFF;                              //Выключить светодиод LED2
	Ready_OFF;                             //Мы не готовы принять данные от формулы
	while(1)
	{
		LED1_ON;                                // Включить светодиод LED1
		_delay_ms(300);
		LED1_OFF;                               // Выключить светодиод LED1
		_delay_ms(300);
	}
}



//-------------------------Прерывания--------------------------------------
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
	/*---------------Инициализация и настройки периферии---------------------*/
	Port_init();					//Инициализируем порты
	_delay_ms(10);
	UART_init(9600);
	UART_Transmit_Str("Start TMP01ESZ", end_string);
	
	Rele_init();					//Инициализируем порты реле

	Ready_OFF;                      //Мы не готовы принять данные от формулы
	LED1_OFF;                       //Выключить светодиод LED1
	LED2_OFF;                       //Выключить светодиод LED2
	_delay_ms(10);
	LCD_init();
	
	clearlcd();
	setpos(0, 0);
	str_lcd("Hello");
	_delay_ms(500);

		
	uint8_t chip_find = TMP01ESZ_Init(); // Поиск TMP01ESZ
	UART_Transmit_Str("Conf reg", end_string);
	UART_Transmit_Hex (chip_find);
	
	if (chip_find!=1) 
	{
		clearlcd();
		setpos(0, 0);
		str_lcd("TMP01ESZ");
		setpos(0, 1);
		str_lcd("Not Fnd");
		UART_Transmit_Str("Wrong reg", end_string); 
		Problem();

	} 
	else
	{
		clearlcd();
		setpos(0, 0);
		str_lcd("TMP01ESZ");
		setpos(0, 1);
		str_lcd("Found");
		_delay_ms(1000);
	}
	

	UART_Transmit_Str("TLOW=10", end_string);
	UART_Transmit_Str("THIGH=20", end_string);
	
	clearlcd();
	setpos(0, 0);
	str_lcd("TLOW=10");
	setpos(0, 1);
	str_lcd("THIGH=20");
	_delay_ms(1000);
	clearlcd();
	setpos(0, 0);
	_delay_ms(100);

	//-------------------------------------------------------------------------

	count = 0;						//После включения попадаем на 0 режим
	INT_ini();						//Инициализируем прерывания
	sei();
	
	
	//----------------------------------------------- Основной цикл-----------------------------------------------
	while(1)					   // Бесконечный цикл
	{
		//----------Режим0-----------
		if (count==0)              // Режим 0 мк не работает
		{
			
			cli();
			UART_Transmit_Str("Mode: 0", end_string);
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 0");
			setpos(0, 1);
			str_lcd("MK Off");
			sei();
			Ready_ON;                     // Мы готовы принять данные от формулы
			LED1_ON;                      // Включить Led1
			LED2_ON;                     
			_delay_ms(1000);
			LED2_OFF;                     // Выключить Led2
			while (count==0)
			{
				asm("nop");
				asm("nop");
				if(count!=0) break;
			}
		}
		
/**************************************************************************/
/* Режим 1 - режим измерения температуры                                  */
/**************************************************************************/
		if (count==1)			
		{
			
			cli();
			UART_Transmit_Str("Mode: 1", end_string);
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 1");
			sei();
			
			while (count==1)
			{
				cli();
				clearlcd();
				setpos(0, 0);
				str_lcd("Mode:1");
				setpos(0, 1);
				rele_VPTAT_OFF;
				sprintf(buffer_LCD,"T=%.02fC", TMP01ESZ_read());
				str_lcd(buffer_LCD);
				_delay_ms(500);
				sei();
				Ready_ON;                     // Мы готовы принять данные от формулы
				LED1_ON;                      // Включить светодиод Led1
				LED2_ON;                     // Выключить Led2
				_delay_ms(100);
				Ready_OFF;                    // Мы не готовы принимать данные от формулы
				_delay_ms(500);
				
			}
			asm("nop");
		}

/************************************************************************/
/* Режим 2 - режим измерения низкого уровня на UNDER                    */
/************************************************************************/
		if (count==2)
		{
	
			cli();
			UART_Transmit_Str("Mode: 2", end_string);
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 2");
			sei();
	
			while (count==2)
			{
				cli();
				clearlcd();
				setpos(0, 0);
				str_lcd("Mode:2");
				setpos(0, 1);
				str_lcd("LOWLEVEL");
				rele_OVER_UNDER_OFF;
				_delay_ms(5000);
				sei();
				Ready_ON;                     // Мы готовы принять данные от формулы
				LED1_ON;                      // Включить светодиод Led1
				LED2_ON;                     // Выключить Led2
				_delay_ms(1000);
				Ready_OFF;                    // Мы не готовы принимать данные от формулы
			}
			asm("nop");
		}

/**************************************************************************/
/* Режим 3 - считывание напряжения на VREF                                */
/**************************************************************************/
		if (count==3)
		{
	
			cli();
			UART_Transmit_Str("Mode: 3", end_string);
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 3");
			setpos(0, 1);
			str_lcd("VREF");
			sei();
	
			while (count==3)
			{
				cli();
				clearlcd();
				setpos(0, 0);
				str_lcd("Mode:3");
				setpos(0, 1);
				str_lcd("VREF");
				rele_VPTAT_ON;
				_delay_ms(5000);
				sei();
				Ready_ON;                     // Мы готовы принять данные от формулы
				LED1_ON;                      // Включить светодиод Led1
				LED2_ON;                      // Выключить Led2
				_delay_ms(1000);
				Ready_OFF;                    // Мы не готовы принимать данные от формулы
			}
			rele_VPTAT_OFF;
			asm("nop");
		}

/************************************************************************/
/* Режим Реле VPTAT                                                     */
/************************************************************************/
	if (count==4)
	{
		
		cli();
		UART_Transmit_Str("Mode: 4", end_string);
		clearlcd();
		setpos(0, 0);
		str_lcd("Mode: 4");
		sei();
		
		while (count==4)
		{
			cli();
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode:4");
			setpos(0, 1);
			str_lcd("VPTAT");
			rele_VPTAT_ON;
			_delay_ms(500);
			sei();
			Ready_ON;                     // Мы готовы принять данные от формулы
			LED1_ON;                      // Включить светодиод Led1
			LED2_ON;                     // Выключить Led2
			_delay_ms(100);
			Ready_OFF;                    // Мы не готовы принимать данные от формулы
			_delay_ms(500);
			
		}
		rele_VPTAT_OFF;
		asm("nop");
	}

/************************************************************************/
/* Обработчик ошибок если от формулы пришла команда больше 4            */
/************************************************************************/
		if (count>5)              //
		{
			cli();
			UART_Transmit_Str("Reset mode", end_string);
			clearlcd();
			setpos(0, 0);
			str_lcd("RESET");
			_delay_ms(500);
			count=0;
			sei();
		}
	}// конец while(1)


}// конец int main(void)









