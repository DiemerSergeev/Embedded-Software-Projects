#include "preference.h"
#include "DriverLM57.h"
char buffer [32];				 // для вывода на дисплей
volatile short int count=0;		 // переменная режима


//J1:+5.0V_VCC(2,19)-LM57, +5V_VDD(3,20)-МК, OUT1-6-DO1(PD2), OUT2-7-DO2(PD3); IN-OUT1-23-DI1(PA6) порты на шине
//J3: PIN01-18-TRIP_TEST, PIN02-19-VOUT, PIN03-20-TOVER_F, PIN04-21-nTOVER_F,  

//IN-OUT1-23-DI1(PA6)- Выход сигнала готовности принять данные от формулы если на PA6-лог.1 мы готовы, лог.0 мы не готовы


// Реле 1- PA5 включает VOUT, TRIP_TEST на МК формулу
// Реле 2- PD5 включает Tover, nTover на МК формулу
// PD0-Светодиод1
// PD1-Светодиод2

// PB4-nCS,PB5-MOSI,PB6-MISO,PB7-SCK
// PB0..PB3,PA0..PA2-LCD



//Режимы:										 OUT1-6-DO1(PD2)                     OUT2-7-DO2(PD3)
//0- МК выключен:								 Ничего не подаём                    Ничего не подаём (ждущий режим DS60R)
//1- Опрос температуры:							 Удерживаем 1 пока идёт запись       (запись: 1-опрос температуры)
//2- Включение реле  вкл. Vo (PA5) F:		     Удерживаем 1 пока идёт запись       (запись: 2-реле вкл. Vo,TRIP_TEST на формулу)
//3- Включение реле  выкл. Vo (PA5) MC:		     Удерживаем 1 пока идёт запись       (запись: 3-реле выкл. Vo,TRIP_TEST на МК)
//4- Включение реле  вкл. Tover (PD5) F:		 Удерживаем 1 пока идёт запись       (запись: 4-реле вкл. Tover,nTover на формулу)
//5- Включение реле  выкл. Tover (PD5) MC:		 Удерживаем 1 пока идёт запись       (запись: 5-реле выкл. Tover,nTover на МК)
//6- Включение реле  вкл. TRIP_TEST:		     Удерживаем 1 пока идёт запись       (запись: 6-реле вкл. TRIP_TEST)
//7- Включение реле  выкл. TRIP_TEST:		     Удерживаем 1 пока идёт запись       (запись: 7-реле выкл. TRIP_TEST)


/*-----------------------Подпрограммы--------------------------------*/
void Port_init(void)
{
	PORTA=0x00;
	DDRA= (0<<7)|(1<<6)|(1<<5)|(0<<4)|(0<<3)|(1<<2)|(1<<1)|(1<<0);  //DDRA
	
	PORTB=0x00;
	DDRB= (1<<7)|(0<<6)|(1<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0);  //DDRB
	
	PORTC=0x00;
	DDRC= (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(1<<2)|(0<<1)|(0<<0);  //DDRC
	
	PORTD=0x00;
	DDRD= (0<<7)|(0<<6)|(1<<5)|(0<<4)|(0<<3)|(0<<2)|(1<<1)|(1<<0);  //DDRD
	
}

void INT_ini(void)
{
	
	PORTD&=0b11110011;				   // PD2, PD3 на вход
	
	MCUCR |= (1<<ISC01) | (1<<ISC00);  // прерывание по фронту
	GICR |= (1<<INT0);				   // PD2- INT0
	GIFR |= (1<<INTF0);				   // сброс флага прерывания
	
}


void Rele_init(void)
{
	DDRA|=0b00100000;	// PA5-переключает 
	PORTA&=0b11011111;  // все реле выключены
	
	DDRD|=0b00100000;	// PD5-переключает
	PORTD&=0b11011111;  // все реле выключены
}






//-------------------------Прерывания--------------------------------------
ISR(INT0_vect)	// прерывание при приходе на PD2 лог. 1
{
	cli();
	Ready_OFF;                             // Мы не готовы принимать данные от формулы
	LED1_OFF;                              // Выключить светодиод 1

	
	if (!(PIND&0b00000100)) goto END;       // проверяем  PD2 на лог 1
	_delay_ms(8);
	if (!(PIND&0b00000100)) goto END;      // проверяем  PD2 на лог 1 через 5 мс если есть то проверяем РD3
	count=0;
	while ((PIND&0b00000100))              // сидим в цикле пока на PD2 лог. 1
	{
		if ((PIND&0b00001000))             // проверяем  PD3 на лог 1
		{
			_delay_ms(5);
			if ((PIND&0b00001000))	++count;        // проверяем  PD3 на лог 1 через 5 мс если есть то count+1 и ждём спада импульса
			while ((PIND&0b00001000)) _delay_ms(5);
		}
		
		asm("nop");
	}
	
	END:
	GIFR |= (1<<INTF0);  // сброс флага прерывания
	sei();
}






int main(void)
{
	/*---------------Инициализация и настройки периферии---------------------*/
	Port_init();					//Инициализируем порты
	
	LCD_init();						//Инициализируем дисплей
	
	Rele_init();					//Инициализируем порты реле
	
	SPI_ini();                      //Инициализируем SPI
	
	Ready_OFF;                      //Мы не готовы принять данные от формулы
	LED1_OFF;                       //Выключить светодиод 1
	LED2_OFF;                       //Выключить светодиод 2
	_delay_ms(10);
	//-------------------------------------------------------------------------
	for(uint8_t i=0; i<10; i++) LM57_read(); //Прогон АЦП
	
	
	// Поиск микросхемы TMP36FSZ
	if ((LM57_read()<152) && (LM57_read()>-52))
	
	{
		clearlcd();
		setpos(0, 0);
		str_lcd("Mode 0");
		setpos(0, 1);
		str_lcd("LM Found");
		_delay_ms(300);
	}
	else
	{
		
		float found_sens=0;
		while (1)
		{
			_delay_ms(100);
			found_sens=LM57_read();
			if ((found_sens<152) && (found_sens>-52)) break;
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode 0");
			setpos(0, 1);
			str_lcd("No sens ");
			LED1_ON;
			LED2_ON;
			_delay_ms(300);
			LED1_OFF;
			LED2_OFF;
			
		}
		
		
		clearlcd();
		setpos(0, 0);
		str_lcd("Mode 0");
		setpos(0, 1);
		str_lcd("LM Found");
		_delay_ms(300);
		
	}
	

	
	

	
	//-------------------------------------------------------------------------
	
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
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 0");
			setpos(0, 1);
			str_lcd("MK Off");
			sei();
			Ready_ON;                     // Мы готовы принять данные от формулы
			LED1_ON;                      // Включить 1
			while (count==0)
			{
				asm("nop");
				asm("nop");
				if(count!=0) break;
			}
		}
		
		//----------Режим1-----------
		if (count==1)				    // Режим 1 режим измерения температуры
		{
			cli();
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 1");
			sei();
			
			
			while (count==1)
			{
				cli();
				clearlcd();
				setpos(0, 0);
				str_lcd("Mode: 1");
				
				float result=LM57_read();

				if ((result>152) || (result<-52))
				{
					setpos(0, 1);
					str_lcd("Not work");
				}
				else
				{
					setpos(0, 1);
					sprintf(buffer,"T=%.2fC", result);
					str_lcd(buffer);
				}
				
				sei();
				
				Ready_ON;                     // Мы готовы принять данные от формулы
				_delay_ms(500);
			}
			
			asm("nop");
		}


		//----------Режим2-----------
		if (count==2)              // Режим 2, включение Vout,TripTest на F (PA5)
		{
			cli();
			LED2_ON;
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 2");
			setpos(0, 1);
			str_lcd("VO_F");
			rele_VOUT_F;
			_delay_ms(500);
			LED2_OFF;
			count=0;
			sei();
		}
		
		//----------Режим3-----------
		if (count==3)              // Режим 3, включение выхода Vout,TripTest на МК (PA5) (по умолчанию)
		{
			cli();
			LED2_ON;
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 3");
			setpos(0, 1);
			str_lcd("VO_MC");
			rele_VOUT_MC;
			_delay_ms(500);
			LED2_OFF;
			count=0;
			sei();
		}
		
		//----------Режим4-----------
		if (count==4)              // Режим 4, включение rele_TOVER_MC на F (PD5)
		{
			cli();
			LED2_ON;
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 4");
			setpos(0, 1);
			str_lcd("TOVER_F");
			rele_TOVER_F;
			_delay_ms(500);
			LED2_OFF;
			count=0;
			sei();
		}
		
		//----------Режим5-----------
		if (count==5)              // Режим 5, включение выхода rele_TOVER_MC на МК (PD5) (по умолчанию)
		{
			cli();
			LED2_ON;
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 5");
			setpos(0, 1);
			str_lcd("TOVER_MC");
			rele_TOVER_MC;
			_delay_ms(500);
			LED2_OFF;
			count=0;
			sei();
		}
		
		//----------Режим6-----------
		if (count==6)              // Режим 6, включение TripTest
		{
			cli();
			LED2_ON;
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 6");
			setpos(0, 1);
			str_lcd("TRIP_ON");
			TRIP_T_ON;
			_delay_ms(500);
			LED2_OFF;
			count=0;
			sei();
		}
		
		//----------Режим7-----------
		if (count==7)              // Режим 7, выключение TripTest (по умолчанию)
		{
			cli();
			LED2_ON;
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 7");
			setpos(0, 1);
			str_lcd("TRIP_OFF");
			TRIP_T_OFF;
			_delay_ms(500);
			LED2_OFF;
			count=0;
			sei();
		}


		//----------Ошибки-----------
		if (count>7)              // Обработчик ошибок если от формулы пришла команда больше 7
		{
			cli();
			LED2_ON;
			clearlcd();
			setpos(0, 0);
			str_lcd("Wrong");
			setpos(0, 1);
			str_lcd("Data");
			_delay_ms(500);
			LED2_OFF;
			count=0;
			sei();
		}


	}// конец while(1)


} // конец int main(void)










