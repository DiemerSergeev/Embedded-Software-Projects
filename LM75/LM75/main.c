#include "preference.h"
#include "LM75_Driver.h"
char buffer_LCD [32];			 // для вывода на дисплей
volatile short int count=0;		 // переменная режима


//J1:
//	+5V_VCC(2,19)
//	+5V_VDD(3,20)
//	OUT1-6-DO1(PE6)
//	OUT2-7-DO2(PF1)
//	IN-OUT1-23-DI1(PA0)
//J2: 
//	PIN55-SDA_F
//	PIN56-OS

//IN-OUT1-23-DI1(PA0)- Выход сигнала готовности принять данные от формулы если на PA0-лог.1 мы готовы, лог.0 мы не готовы

// Реле 1- PA2 переключает SDA,OS на формулу,по умолчанию на МК


// PA3-Светодиод 1
// PA1-Светодиод 2

// Режимы:										 OUT1-6-DO1(PE6)                     OUT2-7-DO2(PF1)
//0- МК выключен:								 Ничего не подаём                    Ничего не подаём (ждущий режим DS7505U+)
//1- Опрос температуры:							 Удерживаем 1 пока идёт запись       (запись: 1-опрос температуры)
//2- Vol -55:		                             Удерживаем 1 пока идёт запись       (запись: 2-Vol OS -55)
//3- Vol 0:		                                 Удерживаем 1 пока идёт запись       (запись: 3-Vol OS 0)
//4- Vol 125:		                             Удерживаем 1 пока идёт запись       (запись: 4-Vol OS 125)
//5- Vol ALL 127С:                               Удерживаем 1 пока идёт запись       (запись: 5-Vol OS 127)
//6- Включение реле  SDA,OS (PA2):		         Удерживаем 1 пока идёт запись       (запись: 6-реле SDA,OS-формула)
//7- Выключение всех реле:		                 Удерживаем 1 пока идёт запись       (запись: 7-реле выключены все)




/*-----------------------Подпрограммы--------------------------------*/

// Инициализация портов
void Port_Init(void)
{
	//PA0-выходит на оснастку (X3-21) DI1_MC
	//PA1-выходит на оснастку (X3-20) 
	//PA2-выходит на оснастку (X3-19) RELE1
	//PA3-выходит на оснастку (X3-18)
	//PA4-выходит на оснастку (X3-17) ADDR2
	//PA5-выходит на оснастку (X3-16)
	//PA6-выходит на оснастку (X3-15) ADDR1
	//PA7-выходит на оснастку (X3-14)
	PORTA=0x00;
	DDRA= (0<<PA7)|(1<<PA6)|(0<<PA5)|(1<<PA4)|(0<<PA3)|(1<<PA2)|(1<<PA1)|(1<<PA0);  //DDRA
	
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
	//PC1-выходит на оснастку (X3-7)
	//PC2-выходит на оснастку (X3-8)
	//PC3-выходит на оснастку (X3-9) LED_READ
	//PC4-выходит на оснастку (X3-10) 
	//PC5-выходит на оснастку (X3-11) LED_WRITE
	//PC6-выходит на оснастку (X3-12)
	//PC7-выходит на оснастку (X3-13) ADDR0
	PORTC=0x00;
	DDRC= (1<<PC7)|(0<<PC6)|(1<<PC5)|(0<<PC4)|(1<<PC3)|(0<<PC2)|(0<<PC1)|(0<<PC0);  //DDRC
	
	//PD0-выходит на оснастку (X2-15) МК I2C SCL
	//PD1-выходит на оснастку (X2-16) МК I2C SDA
	//PD2-BOOT МК UART RX1 - RX CP2102
	//PD3-BOOT МК UART TX1 - TX CP2102
	//PD4-выходит на оснастку (X2-11)
	//PD5-выходит на оснастку (X2-12)
	//PD6-выходит на оснастку (X2-10)
	//PD7-выходит на оснастку (X2-9)
	PORTD=0x00;
	DDRD= (0<<PD7)|(0<<PD6)|(0<<PD5)|(0<<PD4)|(1<<PD3)|(0<<PD2)|(1<<PD1)|(1<<PD0);  //DDRD
	
	//PE0-выходит на оснастку (X2-19) МК UART RX0, МК ISP MOSI
	//PE1-выходит на оснастку (X2-20) МК UART TX0, МК ISP MISO
	//PE2-выходит на оснастку (X3-27), BOOT BUTTON (def. pull up VCC_ATM64)
	//PE3-BOOT SWITCH 2 (def. pull up VCC_ATM64) (SWITCH 2=ON FLASH=ON PE3=0, SWITCH 2=OFF FLASH=OFF, PE3=1)
	//PE4-if USB connect PE4=1 else PE4=0
	//PE5-выходит на оснастку (X2-4)
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
	PORTE&=0b10111111;				   // PE6 на вход
	PORTF&=0b11111101;				   // PF1 на вход
	
	EICRB |= (1<<ISC61) | (1<<ISC60);  // прерывание по фронту
	EIMSK |= (1<<INT6);				   // PE6- INT6
	EIFR |= (1<<INTF6);				   // сброс флага прерывания
}
/*******************************************************/
void Rele_init(void)
{
	DDRA |=0b00000100;	
	PORTA&=0b11111011;  // все реле выключены
}
/*******************************************************/
void Problem(void)
{
	LED_WRITE_OFF;
	Ready_OFF;
	while(1)
	{
		LED_READ_ON;
		_delay_ms(300);
		LED_READ_OFF;
		_delay_ms(300);
	}
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
int main(void)
{
	Port_Init();
	_delay_ms(10);
	UART_Init(1,9600);
	UART_Transmit_Str("Start LM75", end_string);
	
	Rele_init();
	
	ADDR0_LOW;
	ADDR1_LOW;
	ADDR2_LOW;
	
	Ready_OFF;
	LED_READ_OFF;
	LED_WRITE_OFF;
	_delay_ms(10);
	LCD_Init();
	
	uint8_t chip_find = LM75_Init();
	
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("LM75");
	LCD_Setpos(0, 1);
	if (chip_find!=1) 
	{
		LCD_Str("Not Fnd");
		UART_Transmit_Str("Wrong reg", end_string); Problem();
	} 
	else
	{
		LCD_Str("Found");
		UART_Transmit_Str("Chip found", end_string);
	}
	_delay_ms(400);
	
	
	LM75_Set_TOS(80);       // Значения по умолчанию по даташиту
	_delay_ms(100);
	LM75_Set_THYST(75);
	_delay_ms(100);
	uint16_t Conf_Val=READ_ONE_BYTE_REG(Conf_Reg);
	uint16_t TOS_Val=READ_TWO_BYTE_REG(TOS_Reg);
	uint16_t THYST_Val=READ_TWO_BYTE_REG(THyst_Reg);
	UART_Transmit_Str("TOS", end_string);
	UART_Transmit_Hex (TOS_Val,8);
	UART_Transmit_Str("THYST", end_string);
	UART_Transmit_Hex (THYST_Val,8);
	
	TOS_Val = TempConverter(TOS_Val);
	THYST_Val = TempConverter(THYST_Val);
	
	LCD_Clear();
	LCD_Setpos(0, 0);
	//sprintf(buffer_LCD,"0x%04X",  TOS_Val);
	sprintf(buffer_LCD,"TOS %d",  TOS_Val);
	LCD_Str(buffer_LCD);
	LCD_Setpos(0, 1);
	sprintf(buffer_LCD,"THY %d",  THYST_Val);
	LCD_Str(buffer_LCD);
	_delay_ms(1000);
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("ConfReg");
	LCD_Setpos(0, 1);
	sprintf(buffer_LCD,"0x%04X",  Conf_Val);
	LCD_Str(buffer_LCD);
	_delay_ms(1000);
	
	//-------------------------------------------------------------------------

	count=0;						//После включения попадаем на 0 режим
	INT_ini();						//Инициализируем прерывания
	sei();
	
	//----------------------------------------------- Основной цикл-----------------------------------------------
	while(1)					   // Бесконечный цикл
	{
/**************************************************/
		if (count==0)              // Режим 0 мк не работает
		{
			
			cli();
			UART_Transmit_Str("Mode: 0", end_string);
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 0");
			LCD_Setpos(0, 1);
			LCD_Str("MK Off");
			sei();
			Ready_ON;
			LED_WRITE_OFF;
			LED_READ_OFF;
			while (count==0)
			{
				asm("nop");
				asm("nop");
				if(count!=0) break;
			}
		}
/**************************************************/
/*	Режим 1 режим измерения температуры	*/
		if (count==1)				    
		{
			
			cli();
			UART_Transmit_Str("Mode: 1", end_string);
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode:1");
			sei();
			
			while (count==1)
			{
				cli();
				LCD_Clear();
				LCD_Setpos(0, 0);
				LCD_Str("Mode:1");
				LCD_Setpos(0, 1);
				sprintf(buffer_LCD,"T=%.1fC", LM75_Read_Temp());
				LCD_Str(buffer_LCD);
				sei();
				Ready_ON;
				_delay_ms(700);
				Ready_OFF;
				
			}
			
			asm("nop");
		}
/**************************************************/
/*	Режим 2, OS Vol -40	*/
		if (count==2)
		{
			cli();
			UART_Transmit_Str("Mode: 2", end_string);
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 2");
			LCD_Setpos(0, 1);
			LCD_Str("Vol -40");
			LM75_Set_TOS(-45);
			_delay_ms(100);
			LM75_Set_THYST(-47);
			_delay_ms(100);
			
			uint16_t data = LM75_ReadConf();
			_delay_ms(100);
			data = TempConverter(data);
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("ConfReg");
			LCD_Setpos(0, 1);
			sprintf(buffer_LCD,"0x%04X",  data);
			LCD_Str(buffer_LCD);
			_delay_ms(100);
			
			LM75_Set_TOS(80);
			_delay_ms(100);
			LM75_Set_THYST(75);
			count=0;
			sei();
		}
/**************************************************/
/*	Режим 3, OS Vol 0	*/
		if (count==3)
		{
			cli();
			UART_Transmit_Str("Mode: 3", end_string);
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 3");
			LCD_Setpos(0, 1);
			LCD_Str("Vol 0");
			LM75_Set_TOS(-4);
			_delay_ms(100);
			LM75_Set_THYST(-6);
			_delay_ms(100);
			
			uint16_t data = LM75_ReadConf();
			_delay_ms(100);
			data = TempConverter(data);
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("ConfReg");
			LCD_Setpos(0, 1);
			sprintf(buffer_LCD,"0x%04X",  data);
			LCD_Str(buffer_LCD);
			_delay_ms(100);
			
			LM75_Set_TOS(80);
			_delay_ms(100); 
			LM75_Set_THYST(75);
			count=0;
			sei();
		}
/**************************************************/
/*	Режим 4, OS Vol 125	*/
		if (count==4)
		{
			cli();
			UART_Transmit_Str("Mode: 4", end_string);
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 4");
			LCD_Setpos(0, 1);
			LCD_Str("Vol 125");
			LM75_Set_TOS(120);
			_delay_ms(100);
			LM75_Set_THYST(118);
			_delay_ms(100);
			
			uint16_t data = LM75_ReadConf();
			_delay_ms(100);
			data = TempConverter(data);
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("ConfReg");
			LCD_Setpos(0, 1);
			sprintf(buffer_LCD,"0x%04X",  data);
			LCD_Str(buffer_LCD);
			_delay_ms(100);
			
			LM75_Set_TOS(80);
			_delay_ms(100);
			LM75_Set_THYST(75);
			count=0;
			sei();
		}
/**************************************************/
/*	Режим 5, OS Vol ALL	*/
		if (count==5)
		{
			cli();
			UART_Transmit_Str("Mode: 5", end_string);
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 5");
			LCD_Setpos(0, 1);
			LCD_Str("Vol ALL");
			LM75_Set_TOS(-60);
			_delay_ms(100);
			LM75_Set_THYST(-62);
			_delay_ms(100);
		
			uint16_t data = LM75_ReadConf();
			_delay_ms(100);
			data = TempConverter(data);
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("ConfReg");
			LCD_Setpos(0, 1);
			sprintf(buffer_LCD,"0x%04X",  data);
			LCD_Str(buffer_LCD);
			_delay_ms(100);
			
			LM75_Set_TOS(80);
			_delay_ms(100);
			LM75_Set_THYST(75);
			count=0;
			sei();
		}
/**************************************************/
/*	Режим 6, переключить реле SDA,OS на формулу */
		if (count==6)
		{
			cli();
			UART_Transmit_Str("Mode: 6", end_string);
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 6");
			LCD_Setpos(0, 1);
			LCD_Str("SDASCL=F");
			rele_SDAOS_ON;
			_delay_ms(500);
			count=0;
			sei();
		}
/**************************************************/
/*	Режим 7, выключить все реле */	

		if (count==7)
		{
			cli();
			UART_Transmit_Str("Mode: 7", end_string);
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 7");
			LCD_Setpos(0, 1);
			LCD_Str("RELE OFF");
			rele_SDAOS_OFF;
			_delay_ms(500);
			count=0;
			sei();
		}
/**************************************************/
/*	Обработчик ошибок если от формулы пришла команда больше 7 */
		if (count>7)
		{
			cli();
			UART_Transmit_Str("Wrong mode", end_string);
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Wrong");
			LCD_Setpos(0, 1);
			LCD_Str("Data");
			_delay_ms(500);
			count=0;
			sei();
		}
	}// конец while(1)
}// конец int main(void)









