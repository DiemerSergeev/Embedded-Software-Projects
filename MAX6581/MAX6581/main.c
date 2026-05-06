#include "preference.h"
#include "MAX6581_Driver.h"

char buffer_LCD [32];			 // для вывода на дисплей
volatile short int count=0;		 // переменная режима

const uint8_t OVERT_REMOTE_HIGH_LIMIT [7]   = {0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
const uint8_t ALERT_REMOTE_HIGH_LIMIT [7]   = {0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x18};
	
/*  
	J3:
	SDA   (1,18)
	SCL   (2,19)
	ALERT (3,20)
	OVERT (4,21)
	STBY  (5,22)
	
    J1: 
	DI1_MC (PIN 23)
	DO1_MC (PIN 6)
	DO2_MC (PIN 7)
	RST (PIN 8)

	Реле 1- PA6 переключает SDA,SCL на формулу,по умолчанию на 3.3V MC
	Реле 2- PA4 переключает STBY на формулу,по умолчанию на 3.3V MC
	Реле 3- PA2 переключает ALERT/OVERT на формулу,по умолчанию по умолчанию на 3.3V MC

	PA3-Светодиод 1
	PA1-Светодиод 2

	Режимы:										 OUT1-6-DO1(PE6)                     OUT2-7-DO2(PF1)
	0- МК выключен:								 Ничего не подаём                    Ничего не подаём (ждущий режим DS7505U+)
	1- Опрос температуры:							 Удерживаем 1 пока идёт запись       (запись: 1-опрос температуры)
	2- Vol -55:		                             Удерживаем 1 пока идёт запись       (запись: 2-Vol OS -55)
	3- Vol 0:		                                 Удерживаем 1 пока идёт запись       (запись: 3-Vol OS 0)
	4- Vol 125:		                             Удерживаем 1 пока идёт запись       (запись: 4-Vol OS 125)
	5- Vol ALL 127С:                               Удерживаем 1 пока идёт запись       (запись: 5-Vol OS 127)
	6- Включение реле  SDA,SCL (PA6):		         Удерживаем 1 пока идёт запись       (запись: 6-реле SDA,SCL-формула)
	7- Включение реле  A0,A1 (PA4):		         Удерживаем 1 пока идёт запись       (запись: 7-реле A0,A1-формула)
	8- Включение реле  A2,OS (PA2):		         Удерживаем 1 пока идёт запись       (запись: 8-реле A2,OS-формула)
	9- Выключение всех реле:		                 Удерживаем 1 пока идёт запись       (запись: 9-реле выключены все)
*/
////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////
void INT_ini(void)
{
	PORTE&=0b10111111;				   // PE6 на вход
	PORTF&=0b11111101;				   // PF1 на вход
	
	EICRB |= (1<<ISC61) | (1<<ISC60);  // прерывание по фронту
	EIMSK |= (1<<INT6);				   // PE6- INT6
	EIFR  |= (1<<INTF6);			   // сброс флага прерывания
}
////////////////////////////////////////////////////////////////////////////
void Rele_init(void)
{
	DDRA|=0b01010100;	
	PORTA&=0b10101011;  // все реле выключены
}
////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////
int main(void)
{
	Port_init();
	_delay_ms(10);
	Rele_init();
	_delay_ms(10);
	INT_ini();
	_delay_ms(10);
////////////////////////////////////////////////////////////////////////////
	Ready_OFF;
	LED1_OFF;
	LED2_OFF;
	_delay_ms(10);
////////////////////////////////////////////////////////////////////////////
	LCD_init();
	_delay_ms(10);
////////////////////////////////////////////////////////////////////////////		
	uint8_t chip_find = MAX6581_Init();
	if (chip_find == 0) 
	{
		clearlcd();
		setpos(0, 0);
		str_lcd("MAX6581");
		setpos(0, 1);
		str_lcd("Not Fnd");
		_delay_ms(3000);
		Problem();
	} 
	else
	{
		clearlcd();
		setpos(0, 0);
		str_lcd("MAX6581");
		setpos(0, 1);
		str_lcd("Found");
		_delay_ms(2000);
	}
////////////////////////////////////////////////////////////////////////////
	count = 0;
	sei();
////////////////////////////////////////////////////////////////////////////
	while(1)
	{
		if (count==0)
		{
			cli();
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 0");
			setpos(0, 1);
			str_lcd("MK Off");
			sei();
////////////////////////////////////////////////////////////////////////////			
			Ready_ON;
			LED1_OFF;
			LED2_OFF;
////////////////////////////////////////////////////////////////////////////			
			while (count==0)
			{
				asm("nop");
				asm("nop");
				if(count!=0) break;
			}
		}
/************************************************************************/
/* РЕЖИМ ИЗМЕРЕНИЯ ТЕМПЕРАТУРЫ НА LOCAL                                 */
/************************************************************************/
		if (count==1)
		{		
			sei();		
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode:1");
			setpos(0, 1);
			str_lcd("START");
			_delay_ms(2000);
////////////////////////////////////////////////////////////////////////////			
			while (count == 1)
			{
				clearlcd();
				setpos(0, 0);
				str_lcd("LOCAL");
				setpos(0, 1);
				sprintf(buffer_LCD,"T=%.03f",MAX6581_Read_Temp(LOCAL_TEMPERATURE, LOCAL_EXTENDED));
				str_lcd(buffer_LCD);
				_delay_ms(1000);
			}
////////////////////////////////////////////////////////////////////////////
			count = 0;
		}
/************************************************************************/
/* РЕЖИМ НИЗКОГО  УРОВНЯ НА OVERT                                       */
/************************************************************************/
		if (count==2)
		{
			cli();
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 2");
			setpos(0, 1);
			str_lcd("Uol");
			_delay_ms(1000);
////////////////////////////////////////////////////////////////////////////
			uint8_t overt_level;
			uint8_t TEMP_LIMIT = 0x00;
////////////////////////////////////////////////////////////////////////////
			overt_level = READ_ONE_BYTE_REG(OVERT_STATUS_READ);
			UART_Transmit_Hex(overt_level);
////////////////////////////////////////////////////////////////////////////
			clearlcd();
			setpos(0,0);
			str_lcd("OVERTOLD");
			setpos(0, 1);
			sprintf(buffer_LCD,"0x%X",overt_level);
			str_lcd(buffer_LCD);
			_delay_ms(100);
////////////////////////////////////////////////////////////////////////////
			for(int i=0;i<7;i++)
			{
				WRITE_ONE_BYTE_REG(OVERT_REMOTE_HIGH_LIMIT[i],TEMP_LIMIT);
				_delay_ms(100);
				WRITE_ONE_BYTE_REG(ALERT_REMOTE_HIGH_LIMIT[i],TEMP_LIMIT);
				_delay_ms(100);
			}
////////////////////////////////////////////////////////////////////////////
			WRITE_ONE_BYTE_REG(LOCAL_OVERT_HIGH_LIMIT,TEMP_LIMIT);
			_delay_ms(100);
			WRITE_ONE_BYTE_REG(LOCAL_ALERT_HIGH_LIMIT,TEMP_LIMIT);
			_delay_ms(100);
			WRITE_ONE_BYTE_REG(ALERT_LOW_LIMITS,TEMP_LIMIT);
			_delay_ms(100);
			WRITE_ONE_BYTE_REG(0x43,0x00);
			_delay_ms(100);
////////////////////////////////////////////////////////////////////////////
			clearlcd();
			setpos(0, 0);
			str_lcd("OVERTNEW");
			setpos(0, 1);
			sprintf(buffer_LCD,"0x%X",READ_ONE_BYTE_REG(OVERT_STATUS_READ));
			str_lcd(buffer_LCD);
			_delay_ms(2000);
			sei();
////////////////////////////////////////////////////////////////////////////
			count = 0;
		}
/************************************************************************/
/* Режим 3, переключить реле SDA,SCL на формулу                         */
/************************************************************************/
		if (count==3)
		{
			cli();
			
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 3");
			setpos(0, 1);
			str_lcd("SDASCL=F");
			
			rele_SDASCL_ON;
			_delay_ms(1000);
			sei();
			count = 0;
			
		}
/************************************************************************/
/* Режим 4, переключить реле SDA,SCL на формулу                         */
/************************************************************************/ 
		if (count==4)
		{		
			cli();
			
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 4");
			setpos(0, 1);
			str_lcd("SDASCL=MC");
			rele_SDASCL_OFF;
			_delay_ms(1000);
			
			sei();
			count = 0;	
		}
/************************************************************************/
/* Режим 5, переключить реле OVERT/ALERT на формулу                     */
/************************************************************************/		
		if (count==5) 
		{
			cli();
			UART_Transmit_Str("Mode: 5", end_string);
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 5");
			setpos(0, 1);
			str_lcd("OVERT=F");
			rele_ALERT_OVERT_ON;
			_delay_ms(100);
			sei();
			count = 0;
		}
/************************************************************************/
/* Режим 6, переключить реле OVERT/ALERT на MC                          */
/************************************************************************/		
		if (count==6)
		{
			cli();
			UART_Transmit_Str("Mode: 6", end_string);
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 6");
			setpos(0, 1);
			str_lcd("OVERT=MC");
			rele_ALERT_OVERT_OFF;
			_delay_ms(100);
			sei();
			count = 0;
		}		
/************************************************************************/
/* Режим 7, выключить все реле                                          */
/************************************************************************/		
		if (count==7)
		{
			cli();
			UART_Transmit_Str("Mode: 7", end_string);
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode: 7");
			setpos(0, 1);
			str_lcd("RELE OFF");
			rele_SDASCL_OFF;
			_delay_ms(100);
			rele_ALERT_OVERT_OFF;
			_delay_ms(100);
			rele_nSTBY_OFF
			_delay_ms(100);
			sei();
			while (count==7)
			
			count = 0;
		}
/************************************************************************/
/* Режим 8					                                            */
/************************************************************************/
		if(count == 8)
		{
			sei();
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode:8");
			setpos(0, 1);
			str_lcd("START");
			_delay_ms(2000);
			
			while (count==8)
			{
				clearlcd();
				setpos(0, 0);
				str_lcd("REMOTE 1");
				str_lcd(buffer_LCD);
				setpos(0, 1);
				sprintf(buffer_LCD,"T=%.03f",MAX6581_Read_Temp(REMOTE1_TEMPERATURE, REMOTE1_EXTENDED));
				str_lcd(buffer_LCD);
				_delay_ms(1000);
			}
			count = 0;
			asm("nop");
		}
/************************************************************************/
/* Режим 9					                                            */
/************************************************************************/
		if(count == 9)
		{
			sei();
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode:9");
			setpos(0, 1);
			str_lcd("START");
			_delay_ms(2000);
			
			while (count==9)
			{
				clearlcd();
				setpos(0, 0);
				str_lcd("REMOTE 2");
				str_lcd(buffer_LCD);
				setpos(0, 1);
				sprintf(buffer_LCD,"T=%.03f",MAX6581_Read_Temp(REMOTE2_TEMPERATURE, REMOTE2_EXTENDED));
				str_lcd(buffer_LCD);
				_delay_ms(1000);
			}
			count = 0;
			asm("nop");
		}
/************************************************************************/
/* Режим 10					                                            */
/************************************************************************/
		if(count == 10)
		{
			sei();
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode:10");
			setpos(0, 1);
			str_lcd("START");
			_delay_ms(2000);
			
			while (count==10)
			{
				clearlcd();
				setpos(0, 0);
				str_lcd("REMOTE 3");
				str_lcd(buffer_LCD);
				setpos(0, 1);
				sprintf(buffer_LCD,"T=%.03f",MAX6581_Read_Temp(REMOTE3_TEMPERATURE, REMOTE3_EXTENDED));
				str_lcd(buffer_LCD);
				_delay_ms(1000);
			}
			count = 0;
			asm("nop");
		}
/************************************************************************/
/* Режим 11					                                            */
/************************************************************************/
		if(count == 11)
		{
			sei();
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode:11");
			setpos(0, 1);
			str_lcd("START");
			_delay_ms(2000);
			
			while (count==11)
			{
				clearlcd();
				setpos(0, 0);
				str_lcd("REMOTE 4");
				str_lcd(buffer_LCD);
				setpos(0, 1);
				sprintf(buffer_LCD,"T=%.03f",MAX6581_Read_Temp(REMOTE4_TEMPERATURE, REMOTE4_EXTENDED));
				str_lcd(buffer_LCD);
				_delay_ms(1000);
			}
			count = 0;
			asm("nop");
		}
/************************************************************************/
/* Режим 12 				                                            */
/************************************************************************/
		if(count == 12)
		{
			sei();
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode:12");
			setpos(0, 1);
			str_lcd("START");
			_delay_ms(2000);
			
			while (count==12)
			{
				clearlcd();
				setpos(0, 0);
				str_lcd("REMOTE 5");
				str_lcd(buffer_LCD);
				setpos(0, 1);
				sprintf(buffer_LCD,"T=%.03f",MAX6581_Read_Temp(REMOTE5_TEMPERATURE, REMOTE5_EXTENDED));
				str_lcd(buffer_LCD);
				_delay_ms(1000);
			}
			count = 0;
			asm("nop");
		}
/************************************************************************/
/* Режим 13 				                                            */
/************************************************************************/
		if(count == 13)
		{
			sei();
			clearlcd();
			setpos(0, 0);
			str_lcd("Mode:13");
			setpos(0, 1);
			str_lcd("START");
			_delay_ms(2000);
			
			while (count==13)
			{
				clearlcd();
				setpos(0, 0);
				str_lcd("REMOTE 6");
				str_lcd(buffer_LCD);
				setpos(0, 1);
				sprintf(buffer_LCD,"T=%.03f",MAX6581_Read_Temp(REMOTE6_TEMPERATURE, REMOTE6_EXTENDED));
				str_lcd(buffer_LCD);
				_delay_ms(1000);
			}
			count = 0;
			asm("nop");
		}
/************************************************************************/
/* Режим 14 				                                            */
/************************************************************************/
			if(count == 14)
			{
				sei();
				clearlcd();
				setpos(0, 0);
				str_lcd("Mode:14");
				setpos(0, 1);
				str_lcd("START");
				_delay_ms(2000);
				
				while (count==14)
				{
					clearlcd();
					setpos(0, 0);
					str_lcd("REMOTE 7");
					str_lcd(buffer_LCD);
					setpos(0, 1);
					sprintf(buffer_LCD,"T=%.03f",MAX6581_Read_Temp(REMOTE7_TEMPERATURE, REMOTE7_EXTENDED));
					str_lcd(buffer_LCD);
					_delay_ms(1000);
				}
				count = 0;
				asm("nop");
			}
/************************************************************************/
/* Ошибки					                                            */
/************************************************************************/
			if (count>14)
			{
				cli();
				UART_Transmit_Str("Wrong mode", end_string);
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









