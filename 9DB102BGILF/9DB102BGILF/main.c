#include "Preference.h"
#include "9DB102_Driver.h"
static int count = 0;

//Реле 1 - SCL & SDA to MC/FK
//Реле 2 - PCI_EX0 to NC/FK
//Реле 3 - PCI_EX1 to NC/FK
//Реле 4 - CLK_IN1 to NC/OSC
//Реле 5 - PCI_O to NC/GND
//Реле 6 - PLL_BW to GND/+3.3

//LED WRITE - PC5
//LED READ - PC3

void Port_Init(void)
{
	//PA0-выходит на оснастку (X3-21) DI1_MC
	//PA1-выходит на оснастку (X3-20) RELE5
	//PA2-выходит на оснастку (X3-19) RELE1
	//PA3-выходит на оснастку (X3-18) RELE3
	//PA4-выходит на оснастку (X3-17) RELE2
	//PA5-выходит на оснастку (X3-16) RELE4
	//PA6-выходит на оснастку (X3-15) RELE6
	//PA7-выходит на оснастку (X3-14)
	PORTA = 0x00;
	DDRA = (0<<PA7)|(1<<PA6)|(1<<PA5)|(1<<PA4)|(1<<PA3)|(1<<PA2)|(1<<PA1)|(1<<PA0);
	
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
	//PC3-выходит на оснастку (X3-9)  READ_LED
	//PC4-выходит на оснастку (X3-10)
	//PC5-выходит на оснастку (X3-11) WRITE_LED
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
	EIFR  |= (1<<INTF6);			   // сброс флага прерывания
}
/*******************************************************/
void Rele_init(void)
{
	DDRA  |= 0b01111110;	  // реле
	PORTA &= 0b10000001;      // все реле выключены
}
/*******************************************************/
ISR(INT6_vect)	// прерывание при приходе на PE6 лог. 1
{
	cli();
	Ready_OFF;                              // Мы не готовы принимать данные от формулы
	READ_OFF;                               // Выключить светодиод LED1
	WRITE_ON;                                // Включить Led2
	
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
	Ready_OFF;
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
	LCD_Init();
	_delay_ms(100);
	rele_4_ON;
	rele_2_ON;
	_delay_ms(1000);
////////////////////////	
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("9DB102");
	LCD_Setpos(0, 1);
	LCD_Str("CHECK");
	_delay_ms(500);
////////////////////////		
	ICS9DB102_CHECK();
	_delay_ms(100);	
////////////////////////
	LCD_Clear();
	LCD_Setpos(0, 0);
	LCD_Str("9DB102");
	LCD_Setpos(0, 1);
	LCD_Str("FOUND");
	_delay_ms(500);
////////////////////////

	INT_ini();
	_delay_ms(10);
////////////////////////
	count = 0;
	sei();	

    while (1) 
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
			WRITE_OFF;
			READ_OFF;
////////////////////////////////////////////////////////////////
			while (count==0)
			{
				asm("nop");
				asm("nop");
				if(count!=0) break;
			}
		}
/************************************************************************/
/* Режим 1 - Rele1 ON												    */
/************************************************************************/
		if (count == 1)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 1");
			LCD_Setpos(0, 1);
			LCD_Str("RELE 1");
			_delay_ms(500);

			rele_1_ON;
			count = 0;
		}
/************************************************************************/
/* Режим 2 - Rele2 ON                                                   */
/************************************************************************/
		if (count == 2)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 2");
			LCD_Setpos(0, 1);
			LCD_Str("RELE 2");
			_delay_ms(500);

			rele_2_ON;
			count = 0;
		}
/************************************************************************/
/* Режим 3 - Rele3 ON                                                   */
/************************************************************************/
		if (count == 3)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 3");
			LCD_Setpos(0, 1);
			LCD_Str("RELE 3");
			_delay_ms(500);

			rele_3_ON;
			count = 0;
		}
/************************************************************************/
/* Режим 4 - Rele4 ON													*/
/************************************************************************/
		if (count == 4)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 4");
			LCD_Setpos(0, 1);
			LCD_Str("RELE 4");
			_delay_ms(500);

			rele_4_ON;
			count = 0;
		}
/************************************************************************/
/* Режим 5 - Rele5 ON												    */
/************************************************************************/
		if (count == 5)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 5");
			LCD_Setpos(0, 1);
			LCD_Str("RELE 5");
			_delay_ms(500);

			rele_5_ON;
			count = 0;
}
/************************************************************************/
/* Режим 6 - Rele6 ON                                                   */
/************************************************************************/
		if (count==6)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 6");
			LCD_Setpos(0, 1);
			LCD_Str("RELE 6");
			_delay_ms(500);

			rele_6_ON;
			count = 0;
		}
/************************************************************************/
/* Режим 7 - отключения реле                                            */
/************************************************************************/
		if (count == 7)
		{
			LCD_Clear();
			LCD_Setpos(0, 0);
			LCD_Str("Mode: 7");
			LCD_Setpos(0, 1);
			LCD_Str("RELE OFF");
			_delay_ms(500);

			rele_1_OFF;
			rele_2_OFF;
			rele_3_OFF;
			rele_4_OFF;
			rele_5_OFF;
			rele_6_OFF;

			count = 0;
		}
/************************************************************************/
/* Режим Ошибок                                                        */
/************************************************************************/
		if (count > 7)
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

