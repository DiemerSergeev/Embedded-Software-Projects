#include "preference.h"
#include "LM95214_Driver.h"
volatile short int count = 0;		 // переменная режима

//J1:
//	+3.3V_VCC(2,19) 
//	+3.3V_VDD(3,20)
//	OUT1-6-DO1(PE6)
//	OUT2-7-DO2(PF1)
//	IN-OUT1-23-DI1(PA0)

//J2:
//	SDA_F    (54)
//	SCL_F    (53)
//	TCRIT3_F (62)
//	TCRIT2_F (63)
//	TCRIT1_F (64)

//IN-OUT1-23-DI1(PA0)- Выход сигнала готовности принять данные от формулы если на PA0-лог.1 мы готовы, лог.0 мы не готовы

// Реле 1- PA1 переключает SDA, SCL на формулу,по умолчанию на МК
// Реле 2- PA2 переключает TCRIT1, TCRIT2 на формулу, по умолчанию подтяжка к VDD
// Реле 3- PA3 переключает TCRIT3 на формулу, по умолчанию подтяжка к VDD

// PC5-Светодиод WRITE
// PC3-Светодиод READ

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
	//PA1-выходит на оснастку (X3-20) RELE1
	//PA2-выходит на оснастку (X3-19) RELE2
	//PA3-выходит на оснастку (X3-18) RELE3
	//PA4-выходит на оснастку (X3-17) 
	//PA5-выходит на оснастку (X3-16) 
	//PA6-выходит на оснастку (X3-15)
	//PA7-выходит на оснастку (X3-14)
	PORTA = 0x00;
	DDRA = (0<<PA7)|(0<<PA6)|(0<<PA5)|(0<<PA4)|(1<<PA3)|(1<<PA2)|(1<<PA1)|(1<<PA0);
	
	//PB0-выходит на оснастку (X2-14) MR SPI CS
	//PB1-выходит на оснастку (X2-13) MR SPI CLK - FLASH CLK
	//PB2-выходит на оснастку (X2-17) MR SPI DO - FLASH DI
	//PB3-выходит на оснастку (X2-18) MR SPI DI - FLASH DO
	//PB4-выходит на оснастку (X3-2)
	//PB5-выходит на оснастку (X3-3)
	//PB6-выходит на оснастку (X3-4)
	//PB7-BOOT LED
	PORTB = 0x00;
	DDRB = (1<<PB7)|(0<<PB6)|(0<<PB5)|(0<<PB4)|(0<<PB3)|(0<<PB2)|(0<<PB1)|(0<<PB0);
	
	//PC0-выходит на оснастку (X3-6)
	//PC1-выходит на оснастку (X3-7)
	//PC2-выходит на оснастку (X3-8)
	//PC3-выходит на оснастку (X3-9)  LED READ
	//PC4-выходит на оснастку (X3-10) 
	//PC5-выходит на оснастку (X3-11) LED WRITE
	//PC6-выходит на оснастку (X3-12)
	//PC7-выходит на оснастку (X3-13)
	PORTC = 0x00;
	DDRC = (0<<PC7)|(0<<PC6)|(1<<PC5)|(0<<PC4)|(1<<PC3)|(0<<PC2)|(0<<PC1)|(0<<PC0);
	
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
	//PE5-выходит на оснастку (X2-4)
	//PE6-выходит на оснастку (X3-25) INT6, DO1 FORMULA
	//PE7-выходит на оснастку (X3-1)
	PORTE = 0x00;
	DDRE = (0<<PE7)|(0<<PE6)|(0<<PE5)|(0<<PE4)|(0<<PE3)|(0<<PE2)|(0<<PE1)|(0<<PE0);
	
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
	DDRA  |= 0b00001110;	// PA1, PA2, PA3
	PORTA &= 0b11110001;    // реле выключены
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
/*-------------------MAIN------------------------------*/
/*******************************************************/
int main(void)
{
	Ready_OFF;
////////////////////////	
	float temp = 0;
	int8_t data_msb = 0;
	uint8_t data_lsb = 0;
////////////////////////	
	Port_Init();
	_delay_ms(100);
////////////////////////	
	Rele_init();
	_delay_ms(100);
////////////////////////
	I2C_INIT();
	_delay_ms(100);
////////////////////////	
	LED_READ_ON;
	LED_WRITE_ON;
////////////////////////
	LCD_Init();
	_delay_ms(100);
////////////////////////
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("HELLO");
	LCD_Setpos(0, 1);
	LCD_Str("LM95214");
	_delay_ms(500);
////////////////////////	
	LM95214_CHECK();
	_delay_ms(100);
////////////////////////
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("LM95214");
	LCD_Setpos(0, 1);
	LCD_Str("Found");
	_delay_ms(500);
////////////////////////
	INT_ini();
	_delay_ms(100);
////////////////////////
	count = 0;
////////////////////////////////////////////////////////////////
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("STATUS 0");
	LCD_Setpos(0, 1);
	data_lsb = READ_BYTE(COMMON_STATUS);
	sprintf(buffer_LCD,"%.02X", data_lsb);
	LCD_Str(buffer_LCD);
	_delay_ms(500);
////////////////////////////////////////////////////////////////
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("STATUS 1");
	LCD_Setpos(0, 1);
	data_lsb = READ_BYTE(STATUS1);
	sprintf(buffer_LCD,"%.02X", data_lsb);
	LCD_Str(buffer_LCD);
	_delay_ms(500);
////////////////////////////////////////////////////////////////
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("STATUS 2");
	LCD_Setpos(0, 1);
	data_lsb = READ_BYTE(STATUS2);
	sprintf(buffer_LCD,"%.02X", data_lsb);
	LCD_Str(buffer_LCD);
	_delay_ms(500);
////////////////////////////////////////////////////////////////
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("STATUS 3");
	LCD_Setpos(0, 1);
	data_lsb = READ_BYTE(STATUS3);
	sprintf(buffer_LCD,"%.02X", data_lsb);
	LCD_Str(buffer_LCD);
	_delay_ms(500);
////////////////////////////////////////////////////////////////
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("STATUS 4");
	LCD_Setpos(0, 1);
	data_lsb = READ_BYTE(STATUS4);
	sprintf(buffer_LCD,"%.02X", data_lsb);
	LCD_Str(buffer_LCD);
	_delay_ms(500);
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
			_delay_ms(100);
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
			_delay_ms(100);	
////////////////////////////////////////////////////////////////
			WRITE_BYTE(CHANNEL_CONVERSION_ENABLE, 0x01);
			_delay_ms(10);
////////////////////////////////////////////////////////////////			
			while (count == 1)
			{
				LCD_Clear();
				LCD_Setpos(0, 0);
				LCD_Str("LOCAL");
				LCD_Setpos(0, 1);
				
				data_msb = READ_BYTE(LOCAL_TEMP_MSB);
				data_lsb = READ_BYTE(LOCAL_TEMP_LSB);
				
     			data_msb = TEMP_CONVERSION(data_msb);
//				data_lsb = TEMP_CONVERSION(data_lsb);
				
//				temp = (int)data_msb + ((int)data_lsb*0.00390625);
				
				temp = data_msb + (data_lsb>>5)*0.125f;
				
				sprintf(buffer_LCD,"%.3f", temp);
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
			_delay_ms(100);
////////////////////////////////////////////////////////////////
			WRITE_BYTE(CHANNEL_CONVERSION_ENABLE, 0x02);
			_delay_ms(10);			
//			WRITE_BYTE(REMOTE1_OFFSET, 0x00);
//			_delay_ms(10);
////////////////////////////////////////////////////////////////
			while (count == 2)
			{
				LCD_Clear();
				LCD_Setpos(0, 0);
				LCD_Str("REMOTE 1");
				LCD_Setpos(0, 1);
				
				data_msb = READ_BYTE(REMOTE_TEMP1_MSB_SIGNED);
				data_lsb = READ_BYTE(REMOTE_TEMP1_LSB_SIGNED);
				
				data_msb = (float)TEMP_CONVERSION(data_msb);
//				data_lsb = (float)(TEMP_CONVERSION(data_lsb)/256);
				
//				temp = data_msb + data_lsb;
				
				temp = data_msb + (data_lsb>>5)*0.125f;
				
				sprintf(buffer_LCD,"%.03f", temp);
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
			_delay_ms(100);
////////////////////////////////////////////////////////////////
			WRITE_BYTE(CHANNEL_CONVERSION_ENABLE, 0x04);
			_delay_ms(10);
//			WRITE_BYTE(REMOTE2_OFFSET, 0x00);
//			_delay_ms(10);
////////////////////////////////////////////////////////////////
			while (count == 3)
			{
				LCD_Clear();
				LCD_Setpos(0, 0);
				LCD_Str("REMOTE 2");
				LCD_Setpos(0, 1);
				
				data_msb = READ_BYTE(REMOTE_TEMP2_MSB_SIGNED);
				data_lsb = READ_BYTE(REMOTE_TEMP2_LSB_SIGNED);
				
				data_msb = (float)TEMP_CONVERSION(data_msb);
//				data_lsb = (float)(TEMP_CONVERSION(data_lsb)/256);
				
//				temp = data_msb + data_lsb;
				
				temp = data_msb + (data_lsb>>5)*0.125f;
				
				sprintf(buffer_LCD,"%.03f", temp);
				LCD_Str(buffer_LCD);
				_delay_ms(500);
			}
		}
/************************************************************************/
/*   Режим 4 - Чтение температуры на 3ьем внешнем датчике                */
/************************************************************************/
		if (count == 4)
		{
			Ready_ON;
			sei();
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode:4");
			LCD_Setpos(0, 1);
			LCD_Str("TMP READ");
			_delay_ms(100);
////////////////////////////////////////////////////////////////
			WRITE_BYTE(CHANNEL_CONVERSION_ENABLE, 0x08);
			_delay_ms(10);
//			WRITE_BYTE(REMOTE3_OFFSET, 0x00);
//			_delay_ms(10);
////////////////////////////////////////////////////////////////
			while (count == 4)
			{
				LCD_Clear();
				LCD_Setpos(0, 0);
				LCD_Str("REMOTE 3");
				LCD_Setpos(0, 1);
		
				data_msb = READ_BYTE(REMOTE_TEMP3_MSB_SIGNED);
				data_lsb = READ_BYTE(REMOTE_TEMP3_LSB_SIGNED);
		
				data_msb = (float)TEMP_CONVERSION(data_msb);
//				data_lsb = (float)(TEMP_CONVERSION(data_lsb)/256);
		
//				temp = data_msb + data_lsb;
	
				temp = data_msb + (data_lsb>>5)*0.125f;
		
				sprintf(buffer_LCD,"%.03f", temp);
				LCD_Str(buffer_LCD);
				_delay_ms(500);
			}
		}
/************************************************************************/
/*   Режим 5 - Чтение температуры на 4ом внешнем датчике                */
/************************************************************************/
		if (count == 5)
		{
			Ready_ON;
			sei();
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode:5");
			LCD_Setpos(0, 1);
			LCD_Str("TMP READ");
			_delay_ms(100);
////////////////////////////////////////////////////////////////
			WRITE_BYTE(CHANNEL_CONVERSION_ENABLE, 0x10);
			_delay_ms(10);
//			WRITE_BYTE(REMOTE4_OFFSET, 0x00);
//			_delay_ms(10);
////////////////////////////////////////////////////////////////
			while (count == 5)
			{
				LCD_Clear();
				LCD_Setpos(0, 0);
				LCD_Str("REMOTE 4");
				LCD_Setpos(0, 1);
		
				data_msb = READ_BYTE(REMOTE_TEMP4_MSB_SIGNED);
				data_lsb = READ_BYTE(REMOTE_TEMP4_LSB_SIGNED);
		
				data_msb = (float)TEMP_CONVERSION(data_msb);
//				data_lsb = (float)(TEMP_CONVERSION(data_lsb)/256);
		
//				temp = data_msb + data_lsb;
	
				temp = data_msb + (data_lsb>>5)*0.125f;
					
				sprintf(buffer_LCD,"%.03f", temp);
				LCD_Str(buffer_LCD);
				_delay_ms(500);
			}
		}
/************************************************************************/
/*   Режим 6 - Измерение температуры при -40°С                          */
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
			LCD_Str("TEMP -40");
			_delay_ms(100);			
////////////////////////////////////////////////////////////////	
			int8_t limit = 0x0F;
			int8_t offset = 0x78;
			int8_t hyst = 0x0A;
////////////////////////////////////////////////////////////////	
			WRITE_BYTE(TCRIT1_MASK, 0x00);
			_delay_ms(10);
			WRITE_BYTE(TCRIT2_MASK, 0x00);
			_delay_ms(10);
			WRITE_BYTE(TCRIT3_MASK, 0x00);
			_delay_ms(10);
////////////////////////////////////////////////////////////////					
			WRITE_BYTE(LOCAL_TCRIT_LIMIT, limit);
			_delay_ms(10);
			WRITE_BYTE(REMOTE1_TCRIT1_LIMIT, limit);
			_delay_ms(10);
			WRITE_BYTE(REMOTE2_TCRIT1_LIMIT, limit);
			_delay_ms(10);
			WRITE_BYTE(REMOTE3_TCRIT_LIMIT, limit);
			_delay_ms(10);
			WRITE_BYTE(REMOTE4_TCRIT_LIMIT, limit);
			_delay_ms(10);
			WRITE_BYTE(REMOTE1_TCRIT2_TCRIT3_LIMIT, limit);
			_delay_ms(10);
			WRITE_BYTE(REMOTE2_TCRIT2_TCRIT3_LIMIT, limit);
			_delay_ms(10);
////////////////////////////////////////////////////////////////				
			WRITE_BYTE(COMMON_TCRIT_HYSTERESIS, hyst);
			_delay_ms(10);
////////////////////////////////////////////////////////////////									
			WRITE_BYTE(REMOTE1_OFFSET, offset);
			_delay_ms(10);
			WRITE_BYTE(REMOTE2_OFFSET, offset);
			_delay_ms(10);
			WRITE_BYTE(REMOTE3_OFFSET, offset);
			_delay_ms(10);
			WRITE_BYTE(REMOTE4_OFFSET, offset);
			_delay_ms(10);
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
			data_lsb = READ_BYTE(TCRIT1_MASK);
			if (data_lsb==0x00);
			else Problem(TCRIT1_MASK, data_lsb);
			
			data_lsb = READ_BYTE(TCRIT2_MASK);
			if (data_lsb==0x00);
			else Problem(TCRIT2_MASK, data_lsb);
			
			data_lsb = READ_BYTE(TCRIT3_MASK);
			if (data_lsb==0x00);
			else Problem(TCRIT3_MASK, data_lsb);
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
			data_lsb = READ_BYTE(LOCAL_TCRIT_LIMIT);
			if (data_lsb==limit);
			else Problem(LOCAL_TCRIT_LIMIT, data_lsb);
			
			data_lsb = READ_BYTE(REMOTE1_TCRIT1_LIMIT);
			if (data_lsb==limit);
			else Problem(REMOTE1_TCRIT1_LIMIT, data_lsb);
			
			data_lsb = READ_BYTE(REMOTE2_TCRIT1_LIMIT);
			if (data_lsb==limit);
			else Problem(REMOTE2_TCRIT1_LIMIT, data_lsb);

			data_lsb = READ_BYTE(REMOTE3_TCRIT_LIMIT);
			if (data_lsb==limit);
			else Problem(REMOTE3_TCRIT_LIMIT, data_lsb);
			
			data_lsb = READ_BYTE(REMOTE4_TCRIT_LIMIT);
			if (data_lsb==limit);
			else Problem(REMOTE4_TCRIT_LIMIT, data_lsb);
			
			data_lsb = READ_BYTE(REMOTE1_TCRIT2_TCRIT3_LIMIT);
			if (data_lsb==limit);
			else Problem(REMOTE1_TCRIT2_TCRIT3_LIMIT, data_lsb);

			data_lsb = READ_BYTE(REMOTE2_TCRIT2_TCRIT3_LIMIT);
			if (data_lsb==limit);
			else Problem(REMOTE2_TCRIT2_TCRIT3_LIMIT, data_lsb);
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
			data_lsb = READ_BYTE(REMOTE1_OFFSET);
			if (data_lsb==offset);
			else Problem(REMOTE1_OFFSET, data_lsb);
////////////////////////////////////////////////////////////////
			data_lsb = READ_BYTE(REMOTE2_OFFSET);
			if (data_lsb==offset);
			else Problem(REMOTE2_OFFSET, data_lsb);
////////////////////////////////////////////////////////////////
			data_lsb = READ_BYTE(REMOTE3_OFFSET);
			if (data_lsb==offset);
			else Problem(REMOTE3_OFFSET, data_lsb);
////////////////////////////////////////////////////////////////
			data_lsb = READ_BYTE(REMOTE4_OFFSET);
			if (data_lsb==offset);
			else Problem(REMOTE4_OFFSET, data_lsb);
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
			data_lsb = READ_BYTE(COMMON_TCRIT_HYSTERESIS);
			if (data_lsb==hyst);
			else Problem(COMMON_TCRIT_HYSTERESIS, data_lsb);
////////////////////////////////////////////////////////////////				
			sei();		
			_delay_ms(10);	
			count = 0;
		}
/************************************************************************/
/*   Режим 7 - Измерение температуры при 0°С                            */
/************************************************************************/
if (count == 7)
{
		Ready_OFF;
		cli();
////////////////////////////////////////////////////////////////
		LCD_Clear();
		LCD_Setpos(0, 0);
		LCD_Str("Mode:7");
		LCD_Setpos(0, 1);
		LCD_Str("TEMP 0");
		_delay_ms(100);
////////////////////////////////////////////////////////////////
		int8_t limit = 0x37;
		int8_t offset = 0x78;
		int8_t hyst = 0x0A;
////////////////////////////////////////////////////////////////
		WRITE_BYTE(TCRIT1_MASK, 0x00);
		_delay_ms(10);
		WRITE_BYTE(TCRIT2_MASK, 0x00);
		_delay_ms(10);
		WRITE_BYTE(TCRIT3_MASK, 0x00);
		_delay_ms(10);
////////////////////////////////////////////////////////////////
		WRITE_BYTE(LOCAL_TCRIT_LIMIT, limit);
		_delay_ms(10);
		WRITE_BYTE(REMOTE1_TCRIT1_LIMIT, limit);
		_delay_ms(10);
		WRITE_BYTE(REMOTE2_TCRIT1_LIMIT, limit);
		_delay_ms(10);
		WRITE_BYTE(REMOTE3_TCRIT_LIMIT, limit);
		_delay_ms(10);
		WRITE_BYTE(REMOTE4_TCRIT_LIMIT, limit);
		_delay_ms(10);
		WRITE_BYTE(REMOTE1_TCRIT2_TCRIT3_LIMIT, limit);
		_delay_ms(10);
		WRITE_BYTE(REMOTE2_TCRIT2_TCRIT3_LIMIT, limit);
		_delay_ms(10);
////////////////////////////////////////////////////////////////
		WRITE_BYTE(COMMON_TCRIT_HYSTERESIS, hyst);
		_delay_ms(10);
////////////////////////////////////////////////////////////////
		WRITE_BYTE(REMOTE1_OFFSET, offset);
		_delay_ms(10);
		WRITE_BYTE(REMOTE2_OFFSET, offset);
		_delay_ms(10);
		WRITE_BYTE(REMOTE3_OFFSET, offset);
		_delay_ms(10);
		WRITE_BYTE(REMOTE4_OFFSET, offset);
		_delay_ms(10);
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
		data_lsb = READ_BYTE(TCRIT1_MASK);
		if (data_lsb==0x00);
		else Problem(TCRIT1_MASK, data_lsb);

		data_lsb = READ_BYTE(TCRIT2_MASK);
		if (data_lsb==0x00);
		else Problem(TCRIT2_MASK, data_lsb);

		data_lsb = READ_BYTE(TCRIT3_MASK);
		if (data_lsb==0x00);
		else Problem(TCRIT3_MASK, data_lsb);
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
		data_lsb = READ_BYTE(LOCAL_TCRIT_LIMIT);
		if (data_lsb==limit);
		else Problem(LOCAL_TCRIT_LIMIT, data_lsb);

		data_lsb = READ_BYTE(REMOTE1_TCRIT1_LIMIT);
		if (data_lsb==limit);
		else Problem(REMOTE1_TCRIT1_LIMIT, data_lsb);

		data_lsb = READ_BYTE(REMOTE2_TCRIT1_LIMIT);
		if (data_lsb==limit);
		else Problem(REMOTE2_TCRIT1_LIMIT, data_lsb);

		data_lsb = READ_BYTE(REMOTE3_TCRIT_LIMIT);
		if (data_lsb==limit);
		else Problem(REMOTE3_TCRIT_LIMIT, data_lsb);

		data_lsb = READ_BYTE(REMOTE4_TCRIT_LIMIT);
		if (data_lsb==limit);
		else Problem(REMOTE4_TCRIT_LIMIT, data_lsb);

		data_lsb = READ_BYTE(REMOTE1_TCRIT2_TCRIT3_LIMIT);
		if (data_lsb==limit);
		else Problem(REMOTE1_TCRIT2_TCRIT3_LIMIT, data_lsb);

		data_lsb = READ_BYTE(REMOTE2_TCRIT2_TCRIT3_LIMIT);
		if (data_lsb==limit);
		else Problem(REMOTE2_TCRIT2_TCRIT3_LIMIT, data_lsb);
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
		data_lsb = READ_BYTE(REMOTE1_OFFSET);
		if (data_lsb==offset);
		else Problem(REMOTE1_OFFSET, data_lsb);
////////////////////////////////////////////////////////////////
		data_lsb = READ_BYTE(REMOTE2_OFFSET);
		if (data_lsb==offset);
		else Problem(REMOTE2_OFFSET, data_lsb);
////////////////////////////////////////////////////////////////
		data_lsb = READ_BYTE(REMOTE3_OFFSET);
		if (data_lsb==offset);
		else Problem(REMOTE3_OFFSET, data_lsb);
		////////////////////////////////////////////////////////////////
		data_lsb = READ_BYTE(REMOTE4_OFFSET);
		if (data_lsb==offset);
		else Problem(REMOTE4_OFFSET, data_lsb);
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
		data_lsb = READ_BYTE(COMMON_TCRIT_HYSTERESIS);
		if (data_lsb==hyst);
		else Problem(COMMON_TCRIT_HYSTERESIS, data_lsb);
		////////////////////////////////////////////////////////////////
		sei();
		_delay_ms(10);
		count = 0;
}
/************************************************************************/
/*   Режим 8 - Измерение температуры при 120°С                          */
/************************************************************************/
		if (count == 8)
		{
			Ready_OFF;
			cli();
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode:8");
			LCD_Setpos(0, 1);
			LCD_Str("TEMP 120");
			_delay_ms(1000);
////////////////////////////////////////////////////////////////
			int8_t limit = 0x78;
			int8_t offset = 0x00;
			int8_t hyst = 0x0A;
////////////////////////////////////////////////////////////////
			WRITE_BYTE(TCRIT1_MASK, 0x00);
			_delay_ms(10);
			WRITE_BYTE(TCRIT2_MASK, 0x00);
			_delay_ms(10);
			WRITE_BYTE(TCRIT3_MASK, 0x00);
			_delay_ms(10);
////////////////////////////////////////////////////////////////
			WRITE_BYTE(LOCAL_TCRIT_LIMIT, limit);
			_delay_ms(10);
			WRITE_BYTE(REMOTE1_TCRIT1_LIMIT, limit);
			_delay_ms(10);
			WRITE_BYTE(REMOTE2_TCRIT1_LIMIT, limit);
			_delay_ms(10);
			WRITE_BYTE(REMOTE3_TCRIT_LIMIT, limit);
			_delay_ms(10);
			WRITE_BYTE(REMOTE4_TCRIT_LIMIT, limit);
			_delay_ms(10);
			WRITE_BYTE(REMOTE1_TCRIT2_TCRIT3_LIMIT, limit);
			_delay_ms(10);
			WRITE_BYTE(REMOTE2_TCRIT2_TCRIT3_LIMIT, limit);
			_delay_ms(10);
////////////////////////////////////////////////////////////////
			WRITE_BYTE(COMMON_TCRIT_HYSTERESIS, hyst);
			_delay_ms(10);
////////////////////////////////////////////////////////////////
			WRITE_BYTE(REMOTE1_OFFSET, offset);
			_delay_ms(10);
			WRITE_BYTE(REMOTE2_OFFSET, offset);
			_delay_ms(10);
			WRITE_BYTE(REMOTE3_OFFSET, offset);
			_delay_ms(10);
			WRITE_BYTE(REMOTE4_OFFSET, offset);
			_delay_ms(10);
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
			data_lsb = READ_BYTE(TCRIT1_MASK);
			if (data_lsb==0x00);
			else Problem(TCRIT1_MASK, data_lsb);

			data_lsb = READ_BYTE(TCRIT2_MASK);
			if (data_lsb==0x00);
			else Problem(TCRIT2_MASK, data_lsb);

			data_lsb = READ_BYTE(TCRIT3_MASK);
			if (data_lsb==0x00);
			else Problem(TCRIT3_MASK, data_lsb);
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
			data_lsb = READ_BYTE(LOCAL_TCRIT_LIMIT);
			if (data_lsb==limit);
			else Problem(LOCAL_TCRIT_LIMIT, data_lsb);

			data_lsb = READ_BYTE(REMOTE1_TCRIT1_LIMIT);
			if (data_lsb==limit);
			else Problem(REMOTE1_TCRIT1_LIMIT, data_lsb);

			data_lsb = READ_BYTE(REMOTE2_TCRIT1_LIMIT);
			if (data_lsb==limit);
			else Problem(REMOTE2_TCRIT1_LIMIT, data_lsb);

			data_lsb = READ_BYTE(REMOTE3_TCRIT_LIMIT);
			if (data_lsb==limit);
			else Problem(REMOTE3_TCRIT_LIMIT, data_lsb);

			data_lsb = READ_BYTE(REMOTE4_TCRIT_LIMIT);
			if (data_lsb==limit);
			else Problem(REMOTE4_TCRIT_LIMIT, data_lsb);

			data_lsb = READ_BYTE(REMOTE1_TCRIT2_TCRIT3_LIMIT);
			if (data_lsb==limit);
			else Problem(REMOTE1_TCRIT2_TCRIT3_LIMIT, data_lsb);

			data_lsb = READ_BYTE(REMOTE2_TCRIT2_TCRIT3_LIMIT);
			if (data_lsb==limit);
			else Problem(REMOTE2_TCRIT2_TCRIT3_LIMIT, data_lsb);
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
			data_lsb = READ_BYTE(REMOTE1_OFFSET);
			if (data_lsb==offset);
			else Problem(REMOTE1_OFFSET, data_lsb);
////////////////////////////////////////////////////////////////
			data_lsb = READ_BYTE(REMOTE2_OFFSET);
			if (data_lsb==offset);
			else Problem(REMOTE2_OFFSET, data_lsb);
////////////////////////////////////////////////////////////////
			data_lsb = READ_BYTE(REMOTE3_OFFSET);
			if (data_lsb==offset);
			else Problem(REMOTE3_OFFSET, data_lsb);
////////////////////////////////////////////////////////////////
			data_lsb = READ_BYTE(REMOTE4_OFFSET);
			if (data_lsb==offset);
			else Problem(REMOTE4_OFFSET, data_lsb);
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
			data_lsb = READ_BYTE(COMMON_TCRIT_HYSTERESIS);
			if (data_lsb==hyst);
			else Problem(COMMON_TCRIT_HYSTERESIS, data_lsb);
////////////////////////////////////////////////////////////////
			sei();
			_delay_ms(10);
			count = 0;
		}
/************************************************************************/
/* Режим 9 - Включение реле 1                                          */
/************************************************************************/
		if (count == 9)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode 9");
			LCD_Setpos(0, 1);
			LCD_Str("RELE 1");
			rele_SDA_SCL_ON;
			_delay_ms(100);
			count = 0;
		}
/************************************************************************/
/* Режим 10 - Включение реле 2                                          */
/************************************************************************/
		if (count == 10)
		{			
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode 10");
			LCD_Setpos(0, 1);
			LCD_Str("RELE 2");
			rele_TCRIT1_TCRIT2_ON;
			_delay_ms(100);
			count = 0;
		}
/************************************************************************/
/* Режим 11 - Включение реле 3                                          */
/************************************************************************/
		if (count == 11)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode 11");
			LCD_Setpos(0, 1);
			LCD_Str("RELE 3");
			rele_TCRIT3_ON;
			_delay_ms(100);
			count = 0;
		}
/************************************************************************/
/* Режим 12 - Выключение всех реле                                      */
/************************************************************************/
		if (count == 12)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode 12");
			LCD_Setpos(0, 1);
			LCD_Str("RELE OFF");
			rele_SDA_SCL_OFF;
			rele_TCRIT1_TCRIT2_OFF;
			rele_TCRIT3_OFF;
			_delay_ms(100);
			count = 0;
		}
/************************************************************************/
/* Режим 13 - ФК                                                        */
/************************************************************************/
		if (count == 13)
		{		
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("STATUS");
			LCD_Setpos(0, 1);
			data_lsb = READ_BYTE(COMMON_STATUS);
			sprintf(buffer_LCD,"%.02X", data_lsb);
			LCD_Str(buffer_LCD);
			_delay_ms(500);
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("STATUS 1");
			LCD_Setpos(0, 1);
			data_lsb = READ_BYTE(STATUS1);
			sprintf(buffer_LCD,"%.02X", data_lsb);
			LCD_Str(buffer_LCD);
			_delay_ms(500);
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("STATUS 2");
			LCD_Setpos(0, 1);
			data_lsb = READ_BYTE(STATUS2);
			sprintf(buffer_LCD,"%.02X", data_lsb);
			LCD_Str(buffer_LCD);
			_delay_ms(500);
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("STATUS 3");
			LCD_Setpos(0, 1);
			data_lsb = READ_BYTE(STATUS3);
			sprintf(buffer_LCD,"%.02X", data_lsb);
			LCD_Str(buffer_LCD);
			_delay_ms(500);
////////////////////////////////////////////////////////////////
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("STATUS 4");
			LCD_Setpos(0, 1);
			data_lsb = READ_BYTE(STATUS4);
			sprintf(buffer_LCD,"%.02X", data_lsb);
			LCD_Str(buffer_LCD);
			_delay_ms(500);
////////////////////////////////////////////////////////////////
			count = 0;
		}
/************************************************************************/
/*   Обработчик ошибок если от формулы пришла команда больше            */
/************************************************************************/
		if (count>13)
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