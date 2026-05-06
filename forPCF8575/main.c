/*
 * forPCF8575.c
 *
 * Created: 22.07.2022 9:48:07
 * Author : Kolosov
 * for module M1-SPI-ATM64 to control chip PCF8575CDW
 // перед командой чтения нужно выставить единицы в порт
 *Режимы:										                     
 *	0- МК выключен:								                
 *	1- Все порты в низкое:
 *	2- Все порты в высокое:							 
 *	3- ВЫСТАВЛЯЕМ ФОРМУЛОЙ НИЗКИЕ УРОВНИ И ЧИТАЕМ  
 *  4- ВЫСТАВЛЯЕМ ФОРМУЛОЙ ВЫСОКИЕ И ЧИТАЕМ    
 */ 

#include "main.h"
#include "PCF8575.h"

short int count = 0;		 // переменная режима
uint8_t PCF8575_ADRESS=0;
 
void Port_init(void)
{
	PORTF=0x00;
	DDRF= (0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<3)|(1<<2)|(0<<1)|(0<<0);  // led ok and led error = output
	
	PORTB=0x00;
	DDRB= (1<<7)|(1<<6)|(1<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  // PB4-PB6 Input Formula   /// 7 - BOOT_LED FOR TEST
																	// PB6 -5 - output to formula - send error
	PORTE=0x00;
	DDRE= (0<<7)|(0<<6)|(0<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);  // pe7 inter for formula
	
}
void INT_ini(void)
{

	EICRB |= (1<<ISC71) | (1<<ISC70);  // прерывание по фронту
	EIMSK |= (1<<INT7);				   // Pe7- INT7  - formula
	EIFR  |= (1<<INTF7);				   // сброс флага прерывания
	
}

//-------------------------Прерывания--------------------------------------
ISR(INT7_vect)	// прерывание при приходе на Pe7 лог. 1
{
	cli();
	ERROR_TO_FORMULA;                              // Мы не готовы принимать данные от формулы
	
	if (!(PINE&0b10000000)) goto END;       // проверяем  PE7 на лог 1
	_delay_ms(8);
	if (!(PINE&0b10000000)) goto END;      // проверяем  PE7 на лог 1 через 5 мс если есть то проверяем Рb5
	count=0;
	while ((PINE&0b10000000))              // сидим в цикле пока на Pe7 лог. 1
	{
		if ((PINB&0b00010000))             // проверяем  Pb4 на лог 1
		{
			_delay_ms(5);
			if ((PINB&0b00010000))
			{
					++count;        // проверяем  Pb4 на лог 1 через 5 мс если есть то count+1 и ждём спада импульса
					BOOT_LED_ON;
			}
			while ((PINB&0b00010000)) _delay_ms(5);
		}
		
		asm("nop");
	}
	
	END:
	EIFR |= (1<<INTF7);  // сброс флага прерывания
	sei();
}
void CheckChip(uint8_t adress)          // Проверка наличия датчика
{
	//ds1621_init(adress);     // Инициализация датчика
	if ((I2C_ErrorMessage())==48)	// Проверка наличия датчика по ошибкам на I2C шине
	{
		ERROR_LED_ON; 
		ERROR_TO_FORMULA                  // Знак того что датчик ненайден
	}
	InitDS:
	_delay_ms(100);
	//ds1621_init(DS1621_ADRESS);     // Инициализация датчика
	if ((I2C_ErrorMessage())==48) goto InitDS;
	//DS_FND;                         // Знак того что датчик найден
	ERROR_LED_OFF;
	_delay_ms(200);	
}

void blink(uint8_t n)          // Проверка наличия датчика
{
	for (uint8_t i= 0;i<n;i++ )
	{
		ERROR_LED_ON
		_delay_ms(200);
		ERROR_LED_OFF;
		_delay_ms(200);		
	}
	
}



int main(void)
{
	/*---------------Инициализация и настройки периферии---------------------*/
	uint8_t k;
	Port_init();
	ERROR_LED_ON;
	_delay_ms(100);
	ERROR_TO_FORMULA;  // не готов принимать команды от формулы
	ERROR_LED_OFF;
	BOOT_LED_ON;
	INT_ini();						//Инициализируем прерывания
		
	I2C_INIT();
	//UART_init(9600); 
//	UART_Transmit_Str("START",1);
	//UART_Transmit_Hex(I2C_COMAND(I2C_START));
	ERROR_LED_ON;
/*	while(1)					   // Бесконечный цикл
	{
		cli();
		ERROR_TO_FORMULA;// both data = 0
		_delay_ms(50);
	//	WRITE_REGISTER(0xFFFF);	 // перед чтением записать в PCF8575 единицы
		uint16_t data = 1;
	//	data = READ_REGISTER();
		count = 0;
		BOOT_LED_OFF;
	//	if (data == 0xFFFF)   // должны выставить формулой низкие уровни на ногах
		//{
			
			DATA_1_H ;
			//DATA_2_L;
			_delay_ms(50);
		
	}
	*/
/*	while(1)					   // Бесконечный цикл
	{
		asm("nop");
		OK_LED_ON;
		_delay_ms(500);
		WRITE_REGISTER(0xffff);
		_delay_ms(500);
		READ_REGISTER();
		_delay_ms(500);
		WRITE_REGISTER(0x0000);
		OK_LED_OFF;
		_delay_ms(500);
	//	WRITE_REGISTER(0xAA55);
	// READ_REGISTER();		
	}
	*/
		
	ERROR_LED_OFF;
//	_delay_ms(100);
	BOOT_LED_OFF;
	OK_LED_ON;
	count=0;
//	sei();
	
	/*----------------------------------------------------------------------*/
	
	//if(FindChip() == 0x20)
	//{
		//ERROR_LED_ON;
		//count = 6;
	//} 
	
    /* Replace with your application code */
    	//----------------------------------------------- Основной цикл-----------------------------------------------
    while(1)					   // Бесконечный цикл
    {
	    	//----------Режим0-----------
			
	    	if (count==0)              // Режим 0 мк не работает
	    	{	    		      	
		    	Ready_ON; // data2 = 1 // Мы готовы принять данные от формулы
				sei();          
				      
		    	while (count==0)
		    	{
			    	asm("nop");
			    	asm("nop");
					//UART_Transmit_Str("state1",1);
					OK_LED_ON;
					k=0;   
					while ((count==0)&&(k<40))
					{
					 _delay_ms(5);
					 k++;
					}
					//_delay_ms(200);
					OK_LED_OFF;
					k=0;
					while ((count==0)&(k<40))
					{
						_delay_ms(5);
						k++;
					}
					//_delay_ms(200);
					//UART_Transmit_Hex(I2C_COMAND(I2C_START));
			    	if(count!=0) break;
		    	}
	    	}
	    	
	    	//----------Режим1-----------
	    	if (count==1)			 // Режим 1, выставляем порты в низкий ур   
	    	{
				cli();
				BOOT_LED_OFF;
				WRITE_REGISTER(0x0000);
				//blink(count) ;	
				count=0;
				sei();
	    	}

	    	//----------Режим2-----------
	    	if (count==2)              // Режим 2, выставить порты в высокий уровень
	    	{
				cli();
				BOOT_LED_OFF;
				//blink(count) ;	
				WRITE_REGISTER(0xFFFF);	 
		    	count=0;
				sei();
	    	}
	    	
	    	//----------Режим3-----------
	    	if (count==3)              // Режим 3 читаем два регистора из порта
	    	{
				cli();
				ERROR_TO_FORMULA;
				//blink(count) ;	
				WRITE_REGISTER(0xFFFF);	 // перед чтением записать в PCF8575 единицы
				_delay_ms(10);	
				uint16_t data = 1;	
				data = READ_REGISTER();	
				count = 0;
				BOOT_LED_OFF;
				if (data == 0xFFFF)   // должны выставить формулой низкие уровни на ногах
				{
				
					DATA_1_H;
					_delay_ms(10);
					DATA_2_L;
					while ((PINB&0b00010000)) _delay_ms(5); //ждем низкого на  Pb4 на лог 1
					
				}
				else
				{
					ERROR_TO_FORMULA;  // both data1 and 2 = 0
					ERROR_LED_ON;
					while ((PINB&0b00010000)) _delay_ms(5); //проверяем  Pb4 на лог 1
				}
				 
				sei();
	    	}
			//----------Режим4-----------
			if (count==4)              // Режим 3 читаем два регистора из порта
			{
				cli();
				ERROR_TO_FORMULA;// both data =0
				WRITE_REGISTER(0xFFFF);
				uint16_t data = 1;
				data = READ_REGISTER();
				count = 0;
				BOOT_LED_OFF;
				if (data == 0x0000)   // должны выставить формулой такие уровни на ногах
				{
					DATA_2_H ;
					_delay_ms(10);
					DATA_1_H;
					while ((PINB&0b00010000)) _delay_ms(5); //проверяем  Pb4 на лог 1
				}
				else
				{
					ERROR_TO_FORMULA;
					ERROR_LED_ON;
					while ((PINB&0b00010000)) _delay_ms(5); //проверяем  Pb4 на лог 1
				}
				sei();
			}
	    	
	    	//----------Ошибки-----------
	    	if (count>4)              // Обработчик ошибок если от формулы пришла команда больше 11
	    	{
		    	cli();
		    	ERROR_LED_ON;
		    	sei();
	    	}

    }// конец while(1)
}

