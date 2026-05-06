#include "preference.h"
#include "LM96000_Driver.h"
volatile short int count = 0;		 // переменная режима
uint8_t sin_count=0;             // счётчик импульсов
uint16_t CLK_SYNREF=0;           // частота SYNREF для формулы в микросекундах

//J1:
//	+12V_VCC(2,19) 
//	+3.3V_VDD(3,20)
//	OUT1-6-DO1(PE6)
//	OUT2-7-DO2(PF1)
//	IN-OUT1-23-DI1(PA0)

//J2:
//	+2.5V (33)
//	VCCP_IN (49)
//	TACH2_F (61)
//	TACH1_F (62)
//	TACH3_F (63)
//	TACH4_F (64)

//J3:
//	SCL_F (10)
//	SDA_F (11)
//	PWM1_F (12)
//	PWM2_F (13)
//	PWM3_F (14)
//	ADDRESS_ENABLE (15)
//	GND (16)
//	5V (32)

//IN-OUT1-23-DI1(PA0)- Выход сигнала готовности принять данные от формулы если на PA0-лог.1 мы готовы, лог.0 мы не готовы

// Реле 1- PC5 переключает SDA, SCL на формулу,по умолчанию на МК
// Реле 2- PC3 переключает TACH1, TACH2 на формулу, по умолчанию на МК
// Реле 3- PC4 переключает TACH3, TACH4 на формулу, по умолчанию на МК
// Реле 4- PC0 переключает PWM1 на SMA, по умолчанию разрыв
// Реле 5- PC1 переключает PWM2 на SMA, по умолчанию разрыв
// Реле 6- PC2 переключает PWM3 на SMA, по умолчанию разрыв

// PA6-Светодиод WRITE
// PA7-Светодиод READ

// Режимы:
//        0) Ожидание команд от пользователя
//        1) Опрос температуры с локального датчика
//        2) Опрос температуры с первого внешнего датчика
//        3) Опрос температуры с второго внешнего датчика
//        4) Установка температурного диапазона для 0°С
//        5) Установка температурного диапазона для +25°С
//        6) Установка температурного диапазона для +85°С
//        7) Измерение нижнего уровня PWM
//        8) Измерение нижнего уровня SDA
//        9) ФК
//        10) Включение реле 1
//        11) Включение реле 2
//        12) Включение реле 3
//        13) Включение реле 4
//        14) Включение реле 5
//        15) Включение реле 6
//        16) Выключение всех реле

///////////////////////////////////////////////////////////////////////
/*-----------------------Подпрограммы--------------------------------*/
///////////////////////////////////////////////////////////////////////
void Port_Init(void)
{
	//PA0-выходит на оснастку (X3-21) DI1_MC
	//PA1-выходит на оснастку (X3-20) VID4
	//PA2-выходит на оснастку (X3-19) VID3
	//PA3-выходит на оснастку (X3-18) VID2
	//PA4-выходит на оснастку (X3-17) VID1
	//PA5-выходит на оснастку (X3-16) VID0
	//PA6-выходит на оснастку (X3-15) WRITE LED
	//PA7-выходит на оснастку (X3-14) READ LED
	PORTA = 0x00;
	DDRA = (1<<PA7)|(1<<PA6)|(1<<PA5)|(1<<PA4)|(1<<PA3)|(1<<PA2)|(1<<PA1)|(1<<PA0);
	
	//PB0-выходит на оснастку (X2-14) MR SPI CS
	//PB1-выходит на оснастку (X2-13) MR SPI CLK - FLASH CLK
	//PB2-выходит на оснастку (X2-17) MR SPI DO - FLASH DI
	//PB3-выходит на оснастку (X2-18) MR SPI DI - FLASH DO
	//PB4-выходит на оснастку (X3-2) TACH1_MC
	//PB5-выходит на оснастку (X3-3) TACH2_MC
	//PB6-выходит на оснастку (X3-4) TACH3_MC
	//PB7-BOOT LED
	PORTB = 0x00;
	DDRB = (1<<PB7)|(1<<PB6)|(1<<PB5)|(1<<PB4)|(0<<PB3)|(0<<PB2)|(0<<PB1)|(0<<PB0);
	
	//PC0-выходит на оснастку (X3-6) RELE4
	//PC1-выходит на оснастку (X3-7) RELE5
	//PC2-выходит на оснастку (X3-8) RELE6
	//PC3-выходит на оснастку (X3-9) RELE2
	//PC4-выходит на оснастку (X3-10) RELE3
	//PC5-выходит на оснастку (X3-11) RELE1
	//PC6-выходит на оснастку (X3-12)
	//PC7-выходит на оснастку (X3-13)
	PORTC = 0x00;
	DDRC = (0<<PC7)|(0<<PC6)|(1<<PC5)|(1<<PC4)|(1<<PC3)|(1<<PC2)|(1<<PC1)|(1<<PC0);
	
	//PD0-выходит на оснастку (X2-15) МК I2C SCL
	//PD1-выходит на оснастку (X2-16) МК I2C SDA
	//PD2-BOOT МК UART RX1 - RX CP2102
	//PD3-BOOT МК UART TX1 - TX CP2102
	//PD4-выходит на оснастку (X2-11)
	//PD5-выходит на оснастку (X2-12)
	//PD6-выходит на оснастку (X2-10)
	//PD7-выходит на оснастку (X2-9)
	PORTD = 0x00;
	DDRD = (0<<PD7)|(0<<PD6)|(0<<PD5)|(0<<PD4)|(1<<PD3)|(0<<PD2)|(1<<PD1)|(1<<PD0);
	
	//PE0-выходит на оснастку (X2-19) МК UART RX0, МК ISP MOSI
	//PE1-выходит на оснастку (X2-20) МК UART TX0, МК ISP MISO
	//PE2-выходит на оснастку (X3-27), BOOT BUTTON (def. pull up VCC_ATM64)
	//PE3-BOOT SWITCH 2 (def. pull up VCC_ATM64) (SWITCH 2=ON FLASH=ON PE3=0, SWITCH 2=OFF FLASH=OFF, PE3=1)
	//PE4-if USB connect PE4=1 else PE4=0
	//PE5-выходит на оснастку (X2-4) TACH4_MC
	//PE6-выходит на оснастку (X3-25) INT6, DO1 FORMULA
	//PE7-выходит на оснастку (X3-1)
	PORTE = 0x00;
	DDRE = (0<<PE7)|(0<<PE6)|(1<<PE5)|(0<<PE4)|(0<<PE3)|(0<<PE2)|(0<<PE1)|(0<<PE0);
	
	//PF0-выходит на оснастку (X2-6)
	//PF1-выходит на оснастку (X3-23), DO2 FORMULA
	//PF2-ON USB (PF2=1 USB=OFF, PF2=0 USB=ON) (def. pull down => USB=ON)
	//PF3-ON UART (PF3=1 UART=OFF, PF3=0 UART=ON) (def. pull down => UART=ON)
	//PF4-ON VCE (PF4=1 VCE=OFF, PF4=0 VCE=ON) (def. pull up => VCE=OFF)
	//PF5-nCS FLASH (no pull up or pull down)
	//PF6-LCD RS
	//PF7-LCD ENABLE
	PORTF = 0x00; 
	DDRF = (0<<PF7)|(0<<PF6)|(0<<PF5)|(0<<PF4)|(0<<PF3)|(0<<PF2)|(0<<PF1)|(0<<PF0);  //DDRF
	
	//PG0-LCD DATA4
	//PG1-LCD DATA5
	//PG2-LCD DATA6
	//PG3-LCD DATA7
	//PG4-SEL USB (PG4=1 USB connect VCE, PG4=0 USB connect UART) (def. pull down => USB connect UART)
	PORTG = 0x00;
	DDRG = (0<<PG4)|(0<<PG3)|(0<<PG2)|(0<<PG1)|(0<<PG0);  //DDRG
}
/*******************************************************/
void INT_ini(void)
{
	PORTE&=0b10111111;				   // PE6 на вход
	PORTF&=0b11111101;				   // PF1 на вход
	
	EICRB |= (1<<ISC61) | (1<<ISC60);  // прерывание по фронту
	EIMSK |= (1<<INT6);				   // PE6- INT6
	EIFR  |= (1<<INTF6);				   // сброс флага прерывания
}
/*******************************************************/
void Rele_init(void)
{
	DDRC  |= 0b00111111;	//PC0, PC1, PC2, PC3, PC4, PC5
	PORTC &= 0b11000000;  // реле выключены
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
void Timer1_init (uint8_t param)
{
	DDRB|= (1 << PB6)|(1 << PB5);

	if (param>0)
	{
		cli();
////////////////////////////////////////////////		
		TCCR1A = 0;
		TCCR1B = 0;
		TCNT1L  = 0;
		TCNT1H  = 0;
////////////////////////////////////////////////		
		OCR1AL = 0x15; // 22.5 kHz
		OCR1AH = 0x00;
////////////////////////////////////////////////		
		ICR1L = 0x15; // 22.5 kHz
		ICR1H = 0x00;
////////////////////////////////////////////////
		OCR1BL = 0x15; // 22.5 kHz
		OCR1BH = 0x00;
////////////////////////////////////////////////			
		TCCR1A = (0 << COM1A1)|(1 << COM1A0)|(0 << COM1B1)|(1 << COM1B0)|(0 << WGM11)|(0 << WGM10); // CCT, PRESCALER = 8
////////////////////////////////////////////////		
		TCCR1B = (0 << WGM13)|(1 << WGM12)|(0 << CS12)|(1 << CS11)|(0 << CS10); // CCT, PRESCALER = 8
////////////////////////////////////////////////
		sei();
	}
	else
	{
		cli();
		TCCR1A = 0;
		TCCR1B = 0;
		TCNT1 = 0;
		OCR1A = 0;
		sei();
	}
}
/*******************************************************/
void Timer3_init (uint8_t param)
{
	DDRE|=0b00100000;
	
	if (param>0)
	{
		cli();
////////////////////////////////////////////////
		TCCR3A = 0;
		TCCR3B = 0;
////////////////////////////////////////////////		
		TCNT3L  = 0;
		TCNT3H  = 0;
////////////////////////////////////////////////
		OCR3CL = 0x15; // 22.5 kHz
		OCR3CH = 0x00;
////////////////////////////////////////////////		
		ICR3L = 0x15; // 22.5 kHz
		ICR3H = 0x00;
////////////////////////////////////////////////
		TCCR3A = (0 << COM3C1)|(1 << COM3C0)|(0 << WGM31)|(0 << WGM30);  // CCT, PRESCALER = 8
////////////////////////////////////////////////
		TCCR3B = (1 << WGM33)|(1 << WGM32)|(0 << CS32)|(1 << CS31)|(0 << CS30);  // CCT, PRESCALER = 8
////////////////////////////////////////////////
		sei();
	}
	else
	{
		cli();
		TCCR3A = 0;
		TCCR3B = 0;
		TCNT3 = 0;
		OCR3A = 0; 
		sei();
	}
}
/*******************************************************/
void Timer0_init (uint8_t param)
{
	DDRB|=0b00010000;
	
	if (param>0)
	{
		cli();
////////////////////////////////////////////////
		TCCR0 = 0;
		TCNT0 = 0;
////////////////////////////////////////////////
		OCR0 = 0x15; // 22.5 kHz
////////////////////////////////////////////////
		TCCR0 = (1 << WGM01)|(0 << COM01)|(1 << COM00)|(0 << WGM00)|(0 << CS02)|(1 << CS01)|(0 << CS00); // CCT, PRESCALER = 8
////////////////////////////////////////////////
		sei();
	}
	else
	{
		cli();
		TCCR0 = 0;
		TCNT0 = 0;
		OCR0 = 0;
		sei();
	}
	
}
/*******************************************************/
/*-------------------MAIN------------------------------*/
/*******************************************************/
int main(void)
{
	Ready_OFF;
////////////////////////	
	int8_t temp = 0;
	float voltage = 0;
	uint8_t data = 0;
////////////////////////	
	Port_Init();
	_delay_ms(500);
////////////////////////	
	Rele_init();
	_delay_ms(500);
////////////////////////
	I2C_INIT();
	_delay_ms(500);
////////////////////////	
	LED_READ_ON;
	LED_WRITE_ON;
////////////////////////
	LCD_Init();
	_delay_ms(500);
////////////////////////	
	LM96000_CHECK();
	_delay_ms(1000);
////////////////////////
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("LM96000");
	LCD_Setpos(0, 1);
	LCD_Str("Found");
	_delay_ms(500);
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
////////////////////////////////////////////////////////////////			
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 0");
			LCD_Setpos(0, 1);
			LCD_Str("MK Off");
			_delay_ms(1000);
////////////////////////////////////////////////////////////////			
			sei();
			Ready_ON;
////////////////////////////////////////////////////////////////			
			LED_WRITE_OFF;
			LED_READ_OFF;
////////////////////////////////////////////////////////////////			
			while (count==0)
			{		
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
			_delay_ms(500);	
////////////////////////////////////////////////////////////////			
			while (count == 1)
			{
				LCD_Clear();
				LCD_Setpos(0, 0);
				LCD_Str("LOCAL");
				LCD_Setpos(0, 1);
				temp = READ_BYTE(REG_INTERNAL_ZONE2_TEMP);
				temp = TEMP_CONVERSION(temp);
				sprintf(buffer_LCD,"T=%.1d C", temp);
				LCD_Str(buffer_LCD);
				_delay_ms(500);
			}				
		}
/************************************************************************/
/*   Режим 2 - Чтение температуры на 1ом внешнем датчике                */
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
			_delay_ms(500);
////////////////////////////////////////////////////////////////
			while (count == 2)
			{
				LCD_Clear();
				LCD_Setpos(0, 0);
				LCD_Str("REMOTE 1");
				LCD_Setpos(0, 1);
				temp = READ_BYTE(REG_PROCESS_ZONE1_TEMP);
				temp = TEMP_CONVERSION(temp);
				sprintf(buffer_LCD,"T=%.1d C", temp);
				LCD_Str(buffer_LCD);
				_delay_ms(500);
			}
		}
/************************************************************************/
/*   Режим 3 - Чтение температуры на 2ом внешнем датчике                */
/************************************************************************/
		if (count == 3)
		{
			Ready_ON;
			sei();
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode:3");
			LCD_Setpos(0, 1);
			LCD_Str("TMP READ");
			_delay_ms(500);
////////////////////////////////////////////////////////////////
			while (count == 3)
			{
				LCD_Clear();
				LCD_Setpos(0, 0);
				LCD_Str("REMOTE 2");
				LCD_Setpos(0, 1);
				temp = READ_BYTE(REG_REMOTE_ZONE3);
				temp = TEMP_CONVERSION(temp);
				sprintf(buffer_LCD,"T=%.1d C", temp);
				LCD_Str(buffer_LCD);
				_delay_ms(500);
			}
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
			Set_THIGH(127);
			_delay_ms(500);
////////////////////////////////////////////////////////////////			
			Set_TLOW(40);
			_delay_ms(500);
////////////////////////////////////////////////////////////////
			data = READ_BYTE(REG_INT1);
			if ((data&0x0E)==0x0E);
			else Problem(REG_INT1, data);
////////////////////////////////////////////////////////////////				
			sei();		
			_delay_ms(1000);	
			count = 0;
		}
/************************************************************************/
/*   Режим 5 - Измерение температуры при 25°С                            */
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
			Set_THIGH(0);
			_delay_ms(500);
////////////////////////////////////////////////////////////////
			Set_TLOW(-127);
			_delay_ms(500);
////////////////////////////////////////////////////////////////
			data = READ_BYTE(REG_INT1);
			if ((data&0x0E)==0x0E);
			else Problem(REG_INT1, data);
////////////////////////////////////////////////////////////////
			sei();
			_delay_ms(1000);
			count = 0;
		}
/************************************************************************/
/*   Режим 6 - Измерение температуры при 85°С                          */
/************************************************************************/
		if (count == 6)
		{
			Ready_OFF;
			cli();
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode:6");
			LCD_Setpos(0, 1);
			LCD_Str("T LIMIT");
			_delay_ms(1000);
////////////////////////////////////////////////////////////////
			Set_THIGH(60);
			_delay_ms(500);
////////////////////////////////////////////////////////////////
			Set_TLOW(40);
			_delay_ms(500);
////////////////////////////////////////////////////////////////
			data = READ_BYTE(REG_INT1);
			if ((data&0x0E)==0x0E);
			else Problem(REG_INT1, data);
////////////////////////////////////////////////////////////////
			sei();
			_delay_ms(1000);
			count = 0;
		}
/************************************************************************/
/* Режим 7 - ФК PWM 25%                                                 */
/************************************************************************/
		if (count == 7)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode:7");
			LCD_Setpos(0, 1);
			LCD_Str("PWM 25%");
			_delay_ms(1000); 
////////////////////////////////////////////////////////////////
			WRITE_BYTE(REG_PWM1, 0x40);
			WRITE_BYTE(REG_PWM2, 0x40);
			WRITE_BYTE(REG_PWM3, 0x40);
			_delay_ms(1000);
////////////////////////////////////////////////////////////////
			count = 0;
		}
/************************************************************************/
/* Режим 8 - ФК PWM 75%                                                 */
/************************************************************************/
		if (count == 8)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode:8");
			LCD_Setpos(0, 1);
			LCD_Str("PWM 75%");
			_delay_ms(1000);
////////////////////////////////////////////////////////////////
			WRITE_BYTE(REG_PWM1, 0xC0);
			WRITE_BYTE(REG_PWM2, 0xC0);
			WRITE_BYTE(REG_PWM3, 0xC0);
			_delay_ms(1000);
////////////////////////////////////////////////////////////////
			count = 0;
		}
/************************************************************************/
/* Режим 9 - ФК                                                         */
/************************************************************************/
		if (count == 9)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("WAIT");
			LCD_Setpos(0, 1);
			LCD_Str("CHECK");

/************************************************************************/
/* VID CHECK                                                            */
/************************************************************************/
			VID0_LOW;
			VID1_LOW;
			VID2_LOW;
			VID3_LOW;
			VID4_LOW;
////////////////////////////////////////////////////////////////			
			data = READ_BYTE(REG_VID);
			if ((data&0x1F)==0x00);
			else Problem(REG_VID, data);
////////////////////////////////////////////////////////////////			
			VID0_HIGH;
			VID1_HIGH;
			VID2_HIGH;
			VID3_HIGH;
			VID4_HIGH;
////////////////////////////////////////////////////////////////
			data = READ_BYTE(REG_VID);
			if ((data&0x1F)==0x1F);
			else Problem(REG_VID,data);
/************************************************************************/
/* VOLTAGE CHECK                                                        */
/************************************************************************/
			voltage = (float)READ_BYTE(REG_25V);
			data = voltage;
			voltage = voltage*3.32/256;
			if ((voltage<2)||(voltage>3.32)) Problem(REG_25V, data);
////////////////////////////////////////////////////////////////
			voltage = (float)READ_BYTE(REG_VCCP_IN);
			data = voltage;
			voltage = voltage*3/256;
			if ((voltage<1.8)||(voltage>3)) Problem(REG_VCCP_IN, data);
////////////////////////////////////////////////////////////////			
			voltage = (float)READ_BYTE(REG_33V);
			data = voltage;
			voltage = voltage*4.38/256;
			if ((voltage<2.5)||(voltage>4.38)) Problem(REG_33V, data);
////////////////////////////////////////////////////////////////
			voltage = (float)READ_BYTE(REG_5V);
			data = voltage;
			voltage = voltage*6.64/256;
			if ((voltage<4.5)||(voltage>6.64)) Problem(REG_5V, data);
////////////////////////////////////////////////////////////////
			voltage = READ_BYTE(REG_12V);
			data = voltage;
			voltage = voltage*16/256;
			if ((voltage<11)||(voltage>14)) Problem(REG_12V, data);
////////////////////////////////////////////////////////////////
			Set_THIGH(127);
			Set_TLOW(-127);
			_delay_ms(1000);
////////////////////////////////////////////////////////////////								
			READ_BYTE(REG_INT1);
			READ_BYTE(REG_INT2);
			
			data = READ_BYTE(REG_INT1);
			if (data == 0x00);
			else Problem(REG_INT1, data);
			
			data = READ_BYTE(REG_INT2);
			if (data == 0x00);
			else Problem(REG_INT2, data);
////////////////////////////////////////////////////////////////
			WRITE_BYTE(REG_25V_LOW_LIMIT, 0xFF);
			_delay_ms(100);
			WRITE_BYTE(REG_25V_HIGH_LIMIT, 0xFF);
			_delay_ms(100);
			WRITE_BYTE(REG_VCCP_LOW_LIMIT, 0xFF);
			_delay_ms(100);
			WRITE_BYTE(REG_VCCP_HIGH_LIMIT, 0xFF);
			_delay_ms(100);
			WRITE_BYTE(REG_33V_LOW_LIMIT, 0xFF);
			_delay_ms(100);
			WRITE_BYTE(REG_33V_HIGH_LIMIT, 0xFF);
			_delay_ms(100);
			WRITE_BYTE(REG_5V_LOW_LIMIT, 0xFF);
			_delay_ms(100);
			WRITE_BYTE(REG_5V_HIGH_LIMIT, 0xFF);
			_delay_ms(100);
			WRITE_BYTE(REG_12V_LOW_LIMIT, 0xFF);
			_delay_ms(100);
			WRITE_BYTE(REG_12V_HIGH_LIMIT, 0xFF);
			_delay_ms(100);
////////////////////////////////////////////////////////////////		
			data = READ_BYTE(REG_INT1);
			if (data == 0x8F);
			else Problem(REG_INT1, data);
		
			data = READ_BYTE(REG_INT2);
			if (data == 0x01);
			else Problem(REG_INT2, data);
////////////////////////////////////////////////////////////////
			WRITE_BYTE(REG_25V_LOW_LIMIT, 0x00);
			_delay_ms(100);
			WRITE_BYTE(REG_25V_HIGH_LIMIT, 0x00);
			_delay_ms(100);
			WRITE_BYTE(REG_VCCP_LOW_LIMIT, 0x00);
			_delay_ms(100);
			WRITE_BYTE(REG_VCCP_HIGH_LIMIT, 0x00);
			_delay_ms(100);
			WRITE_BYTE(REG_33V_LOW_LIMIT, 0x00);
			_delay_ms(100);
			WRITE_BYTE(REG_33V_HIGH_LIMIT, 0x00);
			_delay_ms(100);
			WRITE_BYTE(REG_5V_LOW_LIMIT, 0x00);
			_delay_ms(100);
			WRITE_BYTE(REG_5V_HIGH_LIMIT, 0x00);
			_delay_ms(100);
			WRITE_BYTE(REG_12V_LOW_LIMIT, 0x00);
			_delay_ms(100);
			WRITE_BYTE(REG_12V_HIGH_LIMIT, 0x00);
			_delay_ms(100);
////////////////////////////////////////////////////////////////		
			data = READ_BYTE(REG_INT1);
			if (data == 0x8F);
			else Problem(REG_INT1, data);

			data = READ_BYTE(REG_INT2);
			if (data == 0x01);
			else Problem(REG_INT2, data);
////////////////////////////////////////////////////////////////
			WRITE_BYTE(REG_25V_LOW_LIMIT, 0x00);
			_delay_ms(100);
			WRITE_BYTE(REG_25V_HIGH_LIMIT, 0xFF);
			_delay_ms(100);
			WRITE_BYTE(REG_VCCP_LOW_LIMIT, 0x00);
			_delay_ms(100);
			WRITE_BYTE(REG_VCCP_HIGH_LIMIT, 0xFF);
			_delay_ms(100);
			WRITE_BYTE(REG_33V_LOW_LIMIT, 0x00);
			_delay_ms(100);
			WRITE_BYTE(REG_33V_HIGH_LIMIT, 0xFF);
			_delay_ms(100);
			WRITE_BYTE(REG_5V_LOW_LIMIT, 0x00);
			_delay_ms(100);
			WRITE_BYTE(REG_5V_HIGH_LIMIT, 0xFF);
			_delay_ms(100);
			WRITE_BYTE(REG_12V_LOW_LIMIT, 0x00);
			_delay_ms(100);
			WRITE_BYTE(REG_12V_HIGH_LIMIT, 0xFF);
			_delay_ms(100);
////////////////////////////////////////////////////////////////	
			READ_BYTE(REG_INT1);
			READ_BYTE(REG_INT2);

			data = READ_BYTE(REG_INT1);
			if (data == 0x00);
			else Problem(REG_INT1, data);
////////////////////////////////////////////////////////////////
			data = READ_BYTE(REG_INT2);
			if (data == 0x00);
			else Problem(REG_INT2, data);	
/************************************************************************/
/* TACH CHECK                                                           */
/************************************************************************/
			Timer1_init(0);
			_delay_ms(100);
			
			Timer3_init(0);
			_delay_ms(100);
			
			Timer0_init(0);
			_delay_ms(100);
////////////////////////////////////////////////////////////////			
			data = READ_BYTE(REG_TACH1_LSB);
			if (data == 0xFF);
			else Problem(REG_TACH1_LSB, data);
			
			data = READ_BYTE(REG_TACH1_MSB);
			if (data == 0xFF);
			else Problem(REG_TACH1_MSB, data);
	
			data = READ_BYTE(REG_TACH2_LSB);
			if (data == 0xFF);
			else Problem(REG_TACH2_LSB, data);
			
			data = READ_BYTE(REG_TACH2_MSB);
			if (data == 0xFF);
			else Problem(REG_TACH2_MSB, data);
			
			data = READ_BYTE(REG_TACH3_LSB);
			if (data == 0xFF);
			else Problem(REG_TACH3_LSB, data);
			
			data = READ_BYTE(REG_TACH3_MSB);
			if (data == 0xFF);
			else Problem(REG_TACH3_MSB, data);
			
			data = READ_BYTE(REG_TACH4_LSB);
			if (data == 0xFF);
			else Problem(REG_TACH4_LSB, data);
			
			data = READ_BYTE(REG_TACH4_MSB);
			if (data == 0xFF);
			else Problem(REG_TACH4_MSB, data);
////////////////////////////////////////////////////////////////	
			Timer1_init(1);
			_delay_ms(100);
			
			Timer3_init(1);
			_delay_ms(100);
			
			Timer0_init(1);
			_delay_ms(100);
////////////////////////////////////////////////////////////////
			data = READ_BYTE(REG_TACH1_LSB);
			if (data != 0xFF);
			else Problem(REG_TACH1_LSB, data);

			data = READ_BYTE(REG_TACH1_MSB);
			if (data != 0xFF);
			else Problem(REG_TACH1_MSB, data);

			data = READ_BYTE(REG_TACH2_LSB);
			if (data != 0xFF);
			else Problem(REG_TACH2_LSB, data);

			data = READ_BYTE(REG_TACH2_MSB);
			if (data != 0xFF);
			else Problem(REG_TACH2_MSB, data);

			data = READ_BYTE(REG_TACH3_LSB);
			if (data != 0xFF);
			else Problem(REG_TACH3_LSB, data);

			data = READ_BYTE(REG_TACH3_MSB);
			if (data != 0xFF);
			else Problem(REG_TACH3_MSB, data);

			data = READ_BYTE(REG_TACH4_LSB);
			if (data != 0xFF);
			else Problem(REG_TACH4_LSB, data);

			data = READ_BYTE(REG_TACH4_MSB);
			if (data != 0xFF);
			else Problem(REG_TACH4_MSB, data);
////////////////////////////////////////////////////////////////		
			count = 0;
		}
/************************************************************************/
/* Режим 10 - Включение реле 1                                          */
/************************************************************************/
		if (count == 10)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode 10");
			LCD_Setpos(0, 1);
			LCD_Str("RELE 1");
			rele_SDA_SCL_ON;
			_delay_ms(500);
			count = 0;
		}
/************************************************************************/
/* Режим 11 - Включение реле 2                                          */
/************************************************************************/
		if (count == 11)
		{			
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode 11");
			LCD_Setpos(0, 1);
			LCD_Str("RELE 2");
			rele_TACH1_TACH2_ON;
			_delay_ms(500);
			count = 0;
		}
/************************************************************************/
/* Режим 12 - Включение реле 3                                          */
/************************************************************************/
		if (count == 12)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode 12");
			LCD_Setpos(0, 1);
			LCD_Str("RELE 3");
			rele_TACH3_TACH4_ON;
			_delay_ms(500);
			count = 0;
		}
/************************************************************************/
/* Режим 13 - Включение реле 4                                          */
/************************************************************************/
		if (count == 13)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode 13");
			LCD_Setpos(0, 1);
			LCD_Str("RELE 4");
			rele_PWM1_ON;
			_delay_ms(500);
			count = 0;
		}
/************************************************************************/
/* Режим 14 - Включение реле 5                                          */
/************************************************************************/
		if (count == 14)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode 14");
			LCD_Setpos(0, 1);
			LCD_Str("RELE 5");
			rele_PWM2_ON;
			_delay_ms(500);
			count = 0;
		}
/************************************************************************/
/* Режим 15 - Включение реле 6                                          */
/************************************************************************/
		if (count == 15)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode 15");
			LCD_Setpos(0, 1);
			LCD_Str("RELE 6");
			rele_PWM3_ON;
			_delay_ms(500);
			count = 0;
		}
/************************************************************************/
/* Режим 16 - Выключение всех реле                                      */
/************************************************************************/
		if (count == 16)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode 16");
			LCD_Setpos(0, 1);
			LCD_Str("RELE OFF");
			rele_SDA_SCL_OFF;
			rele_TACH1_TACH2_OFF;
			rele_TACH3_TACH4_OFF;
			rele_PWM1_OFF;
			rele_PWM2_OFF;
			rele_PWM3_OFF;
			_delay_ms(500);
			count = 0;
		}
/************************************************************************/
/*   Обработчик ошибок если от формулы пришла команда больше            */
/************************************************************************/
		if (count>16)
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