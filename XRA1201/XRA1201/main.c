#include "preference.h"
#include "XRA1201Driver.h"
volatile short int count=0;		 // переменная режима


//J1:+3.3V_VCC(2,19)-XRA1201, +3.3V_VDD(3,20)-МК, OUT1-6-DO1(PE6), OUT2-7-DO2(PF1); IN-OUT1-23-DI1(PA0) порты на шине
//J3: PIN62-38-nCS, PIN63-37-SCK, PIN64-36-SO

//IN-OUT1-23-DI1(PA0)- Выход сигнала готовности принять данные от формулы если на PA6-лог.1 мы готовы, лог.0 мы не готовы


// PA3-Светодиод 1
// PA1-Светодиод 2


//Режимы:										 OUT1-6-DO1(PE6)                     OUT2-7-DO2(PF1)
//0- МК выключен:								 Ничего не подаём                    Ничего не подаём (ждущий режим)
//1- Функциональный тест:						 Удерживаем 1 пока идёт запись       (запись: 1-функциональный тест)
//2- Выдать 1 на P0-P15:		                 Удерживаем 1 пока идёт запись       (запись: 2-выдать 1 на P0-P15)
//3- Выдать 0 на P0-P15:		                 Удерживаем 1 пока идёт запись       (запись: 3-выдать 0 на P0-P15)
//4- Режим входа на P0-P15:		                 Удерживаем 1 пока идёт запись       (запись: 4-режим входа на P0-P15)
//5- Ток потребления (входы, 400 кГц):           Удерживаем 1 пока идёт запись       (запись: 5-ток потребления)
//6- Включение реле SDA,SCL (PA2):		         Удерживаем 1 пока идёт запись       (запись: 6-реле SDA,SCL-формула)
//7- Включение реле nIRQ,ADDR0 (PA4):		     Удерживаем 1 пока идёт запись       (запись: 7-реле nIRQ,ADDR0-формула)
//8- Включение реле ADDR1,ADDR2 (PA6):		     Удерживаем 1 пока идёт запись       (запись: 8-реле ADDR1,ADDR2-формула)
//9- Выключение всех реле:		                 Удерживаем 1 пока идёт запись       (запись: 9-реле на МК все)



/*-----------------------Подпрограммы--------------------------------*/
void Port_init(void)
{
	PORTA=0x00;
	DDRA= (0<<7)|(1<<6)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0);  //DDRA
	
	PORTB=0x00;
	DDRB= (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  //DDRB
	
	PORTC=0x00;
	DDRC= (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  //DDRC
	
	PORTD=0x00;
	DDRD= (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(1<<0);  //DDRD
	
	PORTE=0x00;
	DDRE= (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(1<<1)|(0<<0);  //DDRE
	
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
	DDRA|=0b01010100;	// PA2-переключает SDA,SCL на формулу; PA4-переключает nIRQ, ADDR0 на формулу; PA6-переключает ADDR1, ADDR2 на формулу;
	PORTA&=0b10101011;  // все реле выключены
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
	I2C_INIT();
	UART_init(9600);
	UART_Transmit_Str("Start XRA1201", end_string);
	
	Rele_init();					//Инициализируем порты реле

	Ready_OFF;                      //Мы не готовы принять данные от формулы
	LED1_OFF;                       //Выключить светодиод LED1
	LED2_OFF;                       //Выключить светодиод LED2
	_delay_ms(10);
	
	if (XRA1201_Chip()!=0x01) Problem(); // Поиск XRA1201
	XRA1201_Reset();                     // Сброс регистров
	
	
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
			UART_Transmit_Str("Mode: 0", end_string);
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
		if (count==1)				    // Режим 1 функциональный тест
		{
			
			cli();
			UART_Transmit_Str("Mode: 1", end_string );
			
			uint8_t REG1_XRA=0;
			uint8_t REG2_XRA=0;
			uint16_t OUT_XRA=0;
			uint8_t FLAG_XRA=0;
			XRA1201_SET_ZERO();
			XRA1201_SET_INPUT();
			_delay_ms(1);
			for (uint8_t i=0; i<100; i++)
			{
				REG1_XRA=READ_REGISTER(GSR1_XRA);
				_delay_ms(1);
				REG2_XRA=READ_REGISTER(GSR1_XRA);
				OUT_XRA=(uint16_t)((REG2_XRA<<8)+REG1_XRA);
				UART_Transmit_Hex (OUT_XRA);
				if ((REG1_XRA==0xFF) && (REG2_XRA==0xFF)) {FLAG_XRA=1; break;}
				_delay_ms(10);
			}
			
			if (FLAG_XRA==0) Problem();

			count=0;
			sei();
		}


		//----------Режим2-----------
		if (count==2)              // Режим 2, выдать на P0-P15 лог. 1
		{
			cli();
			UART_Transmit_Str("Mode: 2", end_string );
			XRA1201_SET_ONE();
			count=0;
			sei();
		}
		
		//----------Режим3-----------
		if (count==3)              // Режим 3, выдать на P0-P15 лог. 0
		{
			cli();
			UART_Transmit_Str("Mode: 3", end_string );
			XRA1201_SET_ZERO();
			count=0;
			sei();
		}
		
		//----------Режим4-----------
		if (count==4)              // Режим 4, режим входа
		{
			cli();
			UART_Transmit_Str("Mode: 4", end_string );
			XRA1201_SET_ZERO();
			XRA1201_SET_INPUT();
			_delay_ms(4000);
			XRA1201_SET_ZERO();
			XRA1201_SET_INPUT();
			count=0;
			sei();
		}


		//----------Режим5-----------
		if (count==5)              // Режим 5, ток потребления
		{
			cli();
			UART_Transmit_Str("Mode: 5", end_string );
			XRA1201_SET_ZERO();
			XRA1201_SET_INPUT();
			_delay_ms(1);
			
			for (uint16_t i=0; i<4000; i++)
			{
				READ_REGISTER(GSR1_XRA);
			   _delay_ms(1);
			}

			count=0;
			sei();
		}		


		//----------Режим6-----------
		if (count==6)              // Режим 6, реле SDA,SCL на формулу
		{
			cli();
			UART_Transmit_Str("Mode: 6", end_string );
			rele_SDASCL_ON;
			count=0;
			sei();
		}
		
		
		//----------Режим7-----------
		if (count==7)              // Режим 7, реле nIRQ, ADDR0 на формулу
		{
			cli();
			UART_Transmit_Str("Mode: 7", end_string );
			rele_NIRQAD0_ON;
			count=0;
			sei();
		}


		//----------Режим8-----------
		if (count==8)              // Режим 8, реле ADDR1, ADDR2 на формулу
		{
			cli();
			UART_Transmit_Str("Mode: 8", end_string );
			rele_AD1AD2_ON;
			count=0;
			sei();
		}


		//----------Режим9-----------
		if (count==9)              // Режим 9, выключить все реле
		{
			cli();
			UART_Transmit_Str("Mode: 9", end_string );
			rele_SDASCL_OFF;
			rele_NIRQAD0_OFF;
			rele_AD1AD2_OFF;
			count=0;
			sei();
		}


	
		//----------Ошибки-----------
		if (count>9)              // Обработчик ошибок если от формулы пришла команда больше 9
		{
			cli();
			UART_Transmit_Str("Wrong mode", end_string );
			_delay_ms(500);
			count=0;
			sei();
		}


	}// конец while(1)


}// конец int main(void)









