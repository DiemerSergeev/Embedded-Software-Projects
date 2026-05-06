#include "SPI.h"
#include "Preference.h"

//nCS - PB4
//MO  - PB5 (выход МК)
//MI  - PB6 (вход МК)
//SCK - PB7

void MemBuf_WRITE(void)
{
	for (int i = 0; i<128; i++)
	{
		mem_buff1[i]=(uint8_t)(rand()%256);
		_delay_us(5);
	}
}
void MemBuf_WRITE0(void)
{
	for (int i = 0; i<128; i++)
	{
		mem_buff1[i]=0x00;
	}
}
void MemBuf_WRITE1(void)
{
	for (int i = 0; i<128; i++)
	{
		mem_buff1[i]=0xFF;
	}
}
	
void SPI_Off(void)
{
	//   SPI ENABLE|MSB first| Master  |    Mode:0,0       |  Fosc/4=2Mhz
	SPCR = (0<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0);
	SPSR = (0<<SPI2X); // скорость без множителя

    DDRB |=  0b10110000;	   // DDRB  SPI
	DDRB  &= 0b10111111;
	
	PORTB &= 0b00001111;
	PORTB |= 0b00010000;
     
}
void SPI_INI(void)
{
	LED1_ON;
	LED2_ON;

	DDRB  |= 0b10110000;	   // DDRB  SPI
	DDRB  &= 0b10111111;
	
	PORTB &= 0b00001111;
	PORTB |= 0b00010000;
	//   SPI ENABLE|MSB first| Master  |    Mode:0,0       |  Fosc/4=2Mhz
	SPCR = (1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0);
	SPSR = (0<<SPI2X); // скорость без множителя
	
	LED1_OFF;
	LED2_OFF;
	
}

void READY(void)
{
	LED1_ON;
	LED2_ON;
	uint8_t ready_constant = 0x00;
/************************************************************************/
/*                                                                      */
/************************************************************************/	
	nCS_ON;	
	SPDR = WREN;
	while(!(SPSR & (1<<SPIF)));	
	nCS_OFF;
/************************************************************************/
/*                                                                      */
/************************************************************************/	
	_delay_ms(1);
	do
	{
		nCS_ON;
		SPDR = RDSR;                      //RDSR
		while(!(SPSR & (1<<SPIF)));
		
		SPDR = 0x00;				      //RDSR
		while(!(SPSR & (1<<SPIF)));
		
		ready_constant = SPDR;
		nCS_OFF;
	} while ((ready_constant & 0x83) != 0x02);                //RDY/BSY Status
/************************************************************************/
/*                                                                      */
/************************************************************************/
	_delay_ms(1);
	LED1_OFF;
	LED2_OFF;
}

void WRITE_PAGE(uint16_t address)
{
	uint8_t ready_constant = 0x00;
	LED1_ON;
	LED2_OFF;
	READY();
	
/************************************************************************/
/* ОПЕРАЦИЯ ЗАПИСИ ДАННЫХ                                               */
/************************************************************************/	
	nCS_ON;
	SPDR = WRITE_CMD;
	while(!(SPSR & (1<<SPIF)));

	SPDR = (uint8_t)((address&0xFF00)>>8);  // Старший полубайт адреса
	while(!(SPSR & (1<<SPIF)));
	
	SPDR = (uint8_t)(address&0x00FF);      //  Младший полубайт адреса
	while(!(SPSR & (1<<SPIF)));

	for(int i=0; i<128;i++)           // ЗАПИСЬ ОДНОЙ СТРАНИЦЫ 128 БАЙТ
	{
		SPDR = mem_buff1[i];
		while(!(SPSR & (1<<SPIF)));
	}
/************************************************************************/
/* ОПЕРАЦИЯ ЗАВЕРШЕНА, ВСЕМ СПАСИБО                                     */
/************************************************************************/
	nCS_OFF;
	_delay_ms(5);
	
	do 
	{
		nCS_ON;
	
		SPDR = RDSR;                      //RDSR
		while(!(SPSR & (1<<SPIF)));
	
		SPDR = 0x00;				      //RDSR
		while(!(SPSR & (1<<SPIF)));
		
	
		ready_constant = SPDR;
		nCS_OFF;
	} while ((ready_constant & 0x01) != 0x00);
	
	_delay_ms(1);
	LED1_OFF;
	LED2_OFF;
}

void READ_BYTE(uint16_t address)
{
	LED1_OFF;
	LED2_ON;
	READY();
	_delay_us(5);
/************************************************************************/
/* ОПЕРАЦИЯ ЧТЕНИЯ                                                      */
/************************************************************************/
	
	SPDR = READ_CMD;
	while(!(SPSR & (1<<SPIF)));
	
	SPDR = (uint8_t)((address&0xFF00)>>8);                // Старший полубайт адреса
	while(!(SPSR & (1<<SPIF)));
	
	SPDR = (uint8_t)(address&0x00FF);                    //  Младший полубайт адреса
	while(!(SPSR & (1<<SPIF)));
	
	SPDR = 0x00;
	while(!(SPSR & (1<<SPIF)));
	
	buff_check = SPDR;

/************************************************************************/
/*  ОПЕРАЦИЯ ЗАВЕРШЕНА, ВСЕМ СПАСИБО                                    */
/************************************************************************/	
	nCS_OFF;
	_delay_ms(1);
	LED1_OFF;
	LED2_OFF;
}

void READ_PAGE(uint16_t address)
{
	uint8_t ready_constant = 0x00;
	LED1_OFF;
	LED2_ON;

	do
	{
		_delay_ms(10);
		nCS_ON;
		
		SPDR = RDSR;                      //RDSR
		while(!(SPSR & (1<<SPIF)));
		
		SPDR = 0x00;				      //RDSR
		while(!(SPSR & (1<<SPIF)));
		
		ready_constant = SPDR;
		nCS_OFF;
	} while ((ready_constant & 0x01) != 0x00);

/************************************************************************/
/* ОПЕРАЦИЯ ЧТЕНИЯ СТРАНИЦЫ НАЧАЛАСЬ                                    */
/************************************************************************/	
	_delay_ms(1);
	
	nCS_ON;
	
	SPDR = READ_CMD;                           // Read Sequence READ Opcode 03h
	while(!(SPSR & (1<<SPIF)));

	SPDR = (uint8_t)address&0xFF00;                 // Address first byte
	while(!(SPSR & (1<<SPIF)));
	
	SPDR = (uint8_t)(address&0x00FF)>>8;            // Address second byte
	while(!(SPSR & (1<<SPIF)));
	
	for(int i = 0; i<128; i++)           // ЗАПИСЫВАЕМ В СВОЙ БУФФФЕР ДАННЫЕ С МК
	{
		SPDR = 0x00;
		while(!(SPSR & (1<<SPIF)));
		mem_buff2[i] = SPDR;
	}
/************************************************************************/
/* ОПЕРАЦИЯ ЗАКОНЧЕНА, ВСЕМ СПАСИБО                                     */
/************************************************************************/
	nCS_OFF;
	_delay_ms(1);
	LED1_OFF;
	LED2_OFF;

}

uint8_t CHECK_PAGE(uint16_t address)
{
	LED1_OFF;
	LED2_OFF;
	uint8_t status = 0;

	READY();
	_delay_us(5);
	
	WRITE_PAGE(address);
	_delay_ms(5);
	READ_PAGE(address);
	
	for (int i=0; i<128; i++)
	{
		if (mem_buff1[i] == mem_buff2[i]) status = 1;
		else 
		{
			status = 0;
			break;
		}
	}
	_delay_ms(1);
	LED1_OFF;
	LED2_OFF;
	return(status);
	
}