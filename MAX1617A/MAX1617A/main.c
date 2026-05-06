#include "preference.h"
#include "MAX1617A_Driver.h"
char buffer_LCD [32];			 // для вывода на дисплей
volatile short int count=0;		 // переменная режима


//J1:
//	+3.3V_VCC(2,19)
//	+3.3V_VDD(3,20)
//	OUT1-6-DO1(PE6)
//	OUT2-7-DO2(PF1)
//	IN-OUT1-23-DI1(PA0)

//J3:
//	PIN1-ALERT_F
//	PIN2-ADD1_F
//	PIN3-ADD0_F
//	PIN4-SDA_F
//	PIN5-SCL_F
//	PIN6-nSTBY_MAX

//IN-OUT1-23-DI1(PA0)- Выход сигнала готовности принять данные от формулы если на PA0-лог.1 мы готовы, лог.0 мы не готовы

// Реле 1- PA2 переключает SDA,SCL на формулу,по умолчанию на МК
// Реле 2- PD4 переключает ALARM на формулу, по умолчанию включены LED
// Реле 3- PD6 переключает EVENT на формулу, по умолчанию на МК

// PС3-Светодиод 1
// PС5-Светодиод 2

// Режимы:
//0		- МК выключен
//1		- Чтение регистров
//2		- Запись в регистр ALARM_L значения 0x00
//3		- Запись в регистр ALARM_L значения 0xFF
//4		- Запись в регистр ALARM_L и ETC_L одинаковых значений 0x16
//5		- Активация EVENT на 5 секунд
//6		- Переключение реле SDA&SCL на формулу, по умолчанию на мк
//7		- Переключение реле ALARM на формулу, по умолчанию подтяжка
//8		- Переключение реле EVENT на формулу, по умолчанию на мк
//9		- Отключение всех реле
//10	- Очистка памяти


/*-----------------------Подпрограммы--------------------------------*/

// Инициализация портов
void Port_Init(void)
{
	//PA0-выходит на оснастку (X3-21) DI1_MC
	//PA1-выходит на оснастку (X3-20) READ
	//PA2-выходит на оснастку (X3-19) WRITE
	//PA3-выходит на оснастку (X3-18)
	//PA4-выходит на оснастку (X3-17)
	//PA5-выходит на оснастку (X3-16)
	//PA6-выходит на оснастку (X3-15)
	//PA7-выходит на оснастку (X3-14)
	PORTA=0x00;
	DDRA= (0<<PA7)|(0<<PA6)|(0<<PA5)|(0<<PA4)|(0<<PA3)|(1<<PA2)|(1<<PA1)|(1<<PA0);  //DDRA
	
	//PB0-выходит на оснастку (X2-14) MR SPI CS
	//PB1-выходит на оснастку (X2-13) MR SPI CLK - FLASH CLK
	//PB2-выходит на оснастку (X2-17) MR SPI DO - FLASH DI
	//PB3-выходит на оснастку (X2-18) MR SPI DI - FLASH DO
	//PB4-выходит на оснастку (X3-2)
	//PB5-выходит на оснастку (X3-3)
	//PB6-выходит на оснастку (X3-4)
	//PB7-BOOT LED
	PORTB=0x00;
	DDRB= (1<<PB7)|(0<<PB6)|(0<<PB5)|(0<<PB4)|(0<<PB3)|(0<<PB2)|(0<<PB1)|(0<<PB0);  //DDRB
	
	//PC0-выходит на оснастку (X3-6)
	//PC1-выходит на оснастку (X3-7) RELE2
	//PC2-выходит на оснастку (X3-8) RELE3
	//PC3-выходит на оснастку (X3-9) RELE1
	//PC4-выходит на оснастку (X3-10) 
	//PC5-выходит на оснастку (X3-11)
	//PC6-выходит на оснастку (X3-12)
	//PC7-выходит на оснастку (X3-13)
	PORTC=0x00;
	DDRC= (0<<PC7)|(0<<PC6)|(0<<PC5)|(0<<PC4)|(1<<PC3)|(1<<PC2)|(1<<PC1)|(0<<PC0);  //DDRC
	
	//PD0-выходит на оснастку (X2-15) МК I2C SCL
	//PD1-выходит на оснастку (X2-16) МК I2C SDA
	//PD2-BOOT МК UART RX1 - RX CP2102
	//PD3-BOOT МК UART TX1 - TX CP2102
	//PD4-выходит на оснастку (X2-11) A1
	//PD5-выходит на оснастку (X2-12) A0
	//PD6-выходит на оснастку (X2-10)
	//PD7-выходит на оснастку (X2-9)
	PORTD=0x00;
	DDRD= (0<<PD7)|(0<<PD6)|(1<<PD5)|(1<<PD4)|(1<<PD3)|(0<<PD2)|(1<<PD1)|(1<<PD0);  //DDRD
	
	//PE0-выходит на оснастку (X2-19) МК UART RX0, МК ISP MOSI
	//PE1-выходит на оснастку (X2-20) МК UART TX0, МК ISP MISO
	//PE2-выходит на оснастку (X3-27), BOOT BUTTON (def. pull up VCC_ATM64)
	//PE3-BOOT SWITCH 2 (def. pull up VCC_ATM64) (SWITCH 2=ON FLASH=ON PE3=0, SWITCH 2=OFF FLASH=OFF, PE3=1)
	//PE4-if USB connect PE4=1 else PE4=0
	//PE5-выходит на оснастку (X2-4) ALERT
	//PE6-выходит на оснастку (X3-25) INT6, DO1 FORMULA
	//PE7-выходит на оснастку (X3-1)
	PORTE=0x00;
	DDRE= (0<<PE7)|(0<<PE6)|(0<<PE5)|(0<<PE4)|(0<<PE3)|(0<<PE2)|(0<<PE1)|(0<<PE0);  //DDRE
	
	//PF0-выходит на оснастку (X2-6)
	//PF1-выходит на оснастку (X3-23), DO2 FORMULA
	//PF2-ON USB (PF2=1 USB=OFF, PF2=0 USB=ON) (def. pull down => USB=ON)
	//PF3-ON UART (PF3=1 UART=OFF, PF3=0 UART=ON) (def. pull down => UART=ON)
	//PF4-ON VCE (PF4=1 VCE=OFF, PF4=0 VCE=ON) (def. pull up => VCE=OFF)
	//PF5-nCS FLASH (no pull up or pull down)
	//PF6-LCD RS
	//PF7-LCD ENABLE
	PORTF=0x00; 
	DDRF= (0<<PF7)|(0<<PF6)|(0<<PF5)|(0<<PF4)|(0<<PF3)|(0<<PF2)|(0<<PF1)|(0<<PF0);  //DDRF
	
	//PG0-LCD DATA4
	//PG1-LCD DATA5
	//PG2-LCD DATA6
	//PG3-LCD DATA7
	//PG4-SEL USB (PG4=1 USB connect VCE, PG4=0 USB connect UART) (def. pull down => USB connect UART)
	PORTG=0x00;
	DDRG= (0<<PG4)|(0<<PG3)|(0<<PG2)|(0<<PG1)|(0<<PG0);  //DDRG
}
/*******************************************************/
void INT_ini(void)
{
	PORTE&=0b10011111;				   // PE6 и PE5 на вход
	PORTF&=0b11111101;				   // PF1 на вход
	
	EICRB |= (1<<ISC61) | (1<<ISC60);  // прерывание по фронту
	EIMSK |= (1<<INT6);				   // PE6- INT6
	
	EICRB |= (1<<ISC51) | (0<<ISC50);  // прерывание по фронту
	EIMSK |= (1<<INT5);				   // PE5- INT5
	
	EIFR |= (1<<INTF6);				   // сброс флага прерывания
	EIFR |= (1<<INTF5);				   // сброс флага прерывания
}
/*******************************************************/
void Rele_init(void)
{
	DDRC |=0b00001110;	//PC1, PC2, PC3
	PORTC&=0b11110001;  // реле выключены
}
/*******************************************************/
ISR(INT6_vect)	// прерывание при приходе на PE6 лог. 1
{
	cli();
	Ready_OFF;                              // Мы не готовы принимать данные от формулы
	LED_READ_OFF;                               // Выключить светодиод LED1
	LED_WRITE_ON;                                // Включить Led2
	
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
/*******************************************************/
ISR(INT5_vect)	// прерывание при приходе на PE5 лог. 0
{
	cli();
	Ready_OFF;                              // Мы не готовы принимать данные от формулы
	LED_READ_ON;                               // Выключить светодиод LED1
	LED_WRITE_ON;                                // Включить Led2
	
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("ALERT");
	LCD_Setpos(0, 1);
	LCD_Str("FORMULA");
	
	rele_ALERT_ON;
	
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("STATUS");
	LCD_Setpos(0, 1);
	sprintf(buffer_LCD,"0x%02X", READ_BYTE(RSL));
	LCD_Str(buffer_LCD);
	_delay_ms(5000);
	
	Set_THIGH(127);
	Set_TLOW(-55);
	
	rele_ALERT_OFF;
	_delay_ms(500);
	
	READ_ALERT_RESPONSE();
	
	EIFR |= (1<<INTF5);  // сброс флага прерывания
	sei();
}
/*******************************************************/
int main(void)
{
	Ready_OFF;
////////////////////////	
	uint8_t data = 0x00;
	uint8_t chip_find = 0x00;
	int8_t temp = 0;
////////////////////////	
	Port_Init();
	_delay_ms(10);
////////////////////////	
	Rele_init();
	_delay_ms(10);
////////////////////////
	I2C_INIT();
	_delay_ms(10);
////////////////////////	
	LED_READ_ON;
	LED_WRITE_ON;
////////////////////////
	LCD_Init();
////////////////////////
	A0_LOW;
	A1_LOW;
////////////////////////
	START();
////////////////////////	
	data = 0x40;
	chip_find = MAX1617A_CHECK(data);
////////////////////////
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("MAX1617A");
	LCD_Setpos(0, 1);
	if (chip_find!=1) 
	{
		LCD_Str("Not Fnd");
		Problem();
	}
	else
	{
		LCD_Str("Found");
	}
	_delay_ms(500);
////////////////////////
	WRITE_BYTE(WCA, 0x00);
	_delay_ms(50);
////////////////////////
	WRITE_BYTE(WCRW, 0x04);
	_delay_ms(50);
////////////////////////
/*	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("CONV");
	LCD_Setpos(0, 1);
	sprintf(buffer_LCD,"0x%02X", READ_BYTE(RCRA));
	LCD_Str(buffer_LCD);
	_delay_ms(1000);
*/
////////////////////////
	count=0;
	INT_ini();
	sei();
////////////////////////
	while(1)
	{
/************************************************************************/
/*    Режим 0 мк не работает                                            */
/************************************************************************/
		if (count==0)
		{
			
			cli();
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 0");
			LCD_Setpos(0, 1);
			LCD_Str("MK Off");
			sei();
			Ready_ON;
			LED_WRITE_OFF;
			LED_READ_OFF;
			
	//		READ_ALERT_RESPONSE();
	//		WRITE_BYTE(WCA, 0x80);
	//		_delay_ms(100);
	//		WRITE_BYTE(WCA, 0x00);
	//		_delay_ms(100);
			
			while (count==0)
			{
/*				
				LCD_Clear();
				LCD_Setpos(0, 0);
				LCD_Str("STATUS");
				LCD_Setpos(0, 1);
				sprintf(buffer_LCD,"0x%02X", READ_BYTE(RSL));
				LCD_Str(buffer_LCD);
				_delay_ms(1000);
								
				LCD_Clear();
				LCD_Setpos(0, 0);
				LCD_Str("ALERT");
				LCD_Setpos(0, 1);
				sprintf(buffer_LCD,"0x%02X", READ_ALERT_RESPONSE());
				LCD_Str(buffer_LCD);
				_delay_ms(1000);
				
				LCD_Clear();
				LCD_Setpos(0, 0);
				LCD_Str("THIGH");
				LCD_Setpos(0, 1);
				sprintf(buffer_LCD,"0x%02X", READ_BYTE(RLHN));
				LCD_Str(buffer_LCD);
				_delay_ms(1000);				
				
				LCD_Clear();
				LCD_Setpos(0, 0);
				LCD_Str("TLOW");
				LCD_Setpos(0, 1);
				sprintf(buffer_LCD,"0x%02X", READ_BYTE(RLLI));
				LCD_Str(buffer_LCD);
				_delay_ms(1000);				
*/				
				asm("nop");
				asm("nop");
				if(count!=0) break;
			}
		}
/************************************************************************/
/*   Режим 1 - Чтение температуры на локальном датчике                  */
/************************************************************************/
		if (count == 1)
		{
			Ready_ON;
			sei();
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode:1");
			LCD_Setpos(0, 1);
			LCD_Str("TMP READ");
			_delay_ms(1000);	
////////////////////////////////////////////////////////////////			
			while (count == 1)
			{
				LCD_Clear();
				LCD_Setpos(0, 0);
				LCD_Str("LOCAL");
				LCD_Setpos(0, 1);
				temp = READ_BYTE(RLTS);
				temp = TEMP_CONVERSION(temp);
				sprintf(buffer_LCD,"T=%.1dC   ", temp);
				LCD_Str(buffer_LCD);
				_delay_ms(500);
			}				
		}
/************************************************************************/
/*   Режим 2 - Чтение температуры на внешнем датчике                    */
/************************************************************************/
		if (count == 2)
		{
			Ready_ON;
			sei();
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode:2");
			LCD_Setpos(0, 1);
			LCD_Str("TMP READ");
			_delay_ms(1000);
////////////////////////////////////////////////////////////////
			while (count == 2)
			{
				LCD_Clear();
				LCD_Setpos(0, 0);
				LCD_Str("REMOTE");
				LCD_Setpos(0, 1);
				temp = READ_BYTE(RRTE);
				temp = TEMP_CONVERSION(temp);
				sprintf(buffer_LCD,"T=%.1dC", temp);
				LCD_Str(buffer_LCD);
				_delay_ms(500);
			}
		}
/************************************************************************/
/*   Режим 3 - Измерение температуры при -40°С                          */
/************************************************************************/
		if (count == 3)
		{
			Ready_OFF;
			cli();
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode:3");
			LCD_Setpos(0, 1);
			LCD_Str("T LIMIT");
			_delay_ms(1000);			
////////////////////////////////////////////////////////////////			
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("THIGH");
			LCD_Setpos(0, 1);
			sprintf(buffer_LCD,"T=%.1dC", Set_THIGH(80));
			LCD_Str(buffer_LCD);
			_delay_ms(1000);
////////////////////////////////////////////////////////////////			
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("TLOW");
			LCD_Setpos(0, 1);
			sprintf(buffer_LCD,"T=%.1dC", Set_TLOW(-20));
			LCD_Str(buffer_LCD);
			_delay_ms(1000);
////////////////////////////////////////////////////////////////	
			sei();		
			_delay_ms(2000);	
			count = 0;
		}
/************************************************************************/
/*   Режим 4 - Измерение температуры при 0°С                            */
/************************************************************************/
		if (count == 4)
		{
			Ready_OFF;
			cli();
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode:4");
			LCD_Setpos(0, 1);
			LCD_Str("T LIMIT");
			_delay_ms(1000);
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("THIGH");
			LCD_Setpos(0, 1);
			sprintf(buffer_LCD,"T=%.1dC", Set_THIGH(-20));
			LCD_Str(buffer_LCD);
			_delay_ms(1000);
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("TLOW");
			LCD_Setpos(0, 1);
			sprintf(buffer_LCD,"T=%.1dC", Set_TLOW(-40));
			LCD_Str(buffer_LCD);
			_delay_ms(1000);
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("STATUS");
			LCD_Setpos(0, 1);
			sprintf(buffer_LCD,"0x%02X", READ_BYTE(RSL));
			LCD_Str(buffer_LCD);
			_delay_ms(1000);
////////////////////////////////////////////////////////////////
			count = 0;
			sei();
			_delay_ms(100);
			
		}
/************************************************************************/
/*   Режим 5 - Измерение температуры при 125°С                          */
/************************************************************************/
		if (count == 5)
		{
			Ready_OFF;
			cli();
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode:5");
			LCD_Setpos(0, 1);
			LCD_Str("T LIMIT");
			_delay_ms(1000);
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("THIGH");
			LCD_Setpos(0, 1);
			sprintf(buffer_LCD,"T=%.1dC", Set_THIGH(80));
			LCD_Str(buffer_LCD);
			_delay_ms(1000);
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("TLOW");
			LCD_Setpos(0, 1);
			sprintf(buffer_LCD,"T=%.1dC", Set_TLOW(40));
			LCD_Str(buffer_LCD);
			_delay_ms(1000);
////////////////////////////////////////////////////////////////
			sei();
			count = 0;
		}
/************************************************************************/
/* Режим 6 - Включение реле SDA&SCL                                     */
/************************************************************************/
		if (count == 6)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode 6");
			LCD_Setpos(0, 1);
			LCD_Str("RELE SDA");
			rele_SDAOS_ON;
			_delay_ms(500);
			count = 0;
		}
/************************************************************************/
/* Режим 7 - Включение реле ADD01                                       */
/************************************************************************/
		if (count == 7)
		{			
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode 7");
			LCD_Setpos(0, 1);
			LCD_Str("RELE ADD");
			rele_ADD01_ON;
			_delay_ms(500);
			count = 0;
		}
/************************************************************************/
/* Режим 8 - Включение реле ALERT                                       */
/************************************************************************/
		if (count == 8)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode 8");
			LCD_Setpos(0, 1);
			LCD_Str("RELE ALR");
			rele_ALERT_ON;
			_delay_ms(500);
			count = 0;
		}
/************************************************************************/
/* Режим 9 - Выключение всех реле                                       */
/************************************************************************/
		if (count == 9)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode 9");
			LCD_Setpos(0, 1);
			LCD_Str("RELE OFF");
			rele_SDAOS_OFF;
			rele_ADD01_OFF;
			rele_ALERT_OFF;
			_delay_ms(500);
			count = 0;
		}
/************************************************************************/
/*   Обработчик ошибок если от формулы пришла команда больше            */
/************************************************************************/
		if (count>9)
		{
			cli();
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Wrong");
			LCD_Setpos(0, 1);
			LCD_Str("Data");
			_delay_ms(500);
			count=0;
			sei();
		}
	}
}









