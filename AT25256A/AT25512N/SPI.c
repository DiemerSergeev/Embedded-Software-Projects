#include "SPI.h"
#include "Preference.h"

//nCS - PB4
//MO  - PB5 (выход МК)
//MI  - PB6 (вход МК)
//SCK - PB7

void MemBuf_WRITE(void)
{
	for (int i = 0; i<64; i++)
	{
		mem_buff1[i]=(uint8_t)(rand()%256);
		_delay_us(5);
	}
}
void MemBuf_WRITE00(void)
{
	for (int i = 0; i<64; i++)
	{
		mem_buff1[i]=0x00;
	}
}
void MemBuf_WRITEFF(void)
{
	for (int i = 0; i<64; i++)
	{
		mem_buff1[i]=0xFF;
	}
}
void MemBuf_WRITEAA(void)
{
	for (int i = 0; i<64; i++)
	{
		mem_buff1[i]=0xAA;
	}
}
void MemBuf_WRITE55(void)
{
	for (int i = 0; i<64; i++)
	{
		mem_buff1[i]=0x55;
	}
}
	
void SPI_Off(void)
{
	//   SPI ENABLE|MSB first| Master  |    Mode:0,0       |  Fosc/4=2Mhz
	SPCR = (0<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0);
	SPSR = (0<<SPI2X); // скорость без множителя
	UCSRC |= (0<<UMSEL);

    DDRB |=  0b10110000;	   // DDRB  SPI
	DDRB  &= 0b10111111;
	
	PORTB &= 0b00001111;
	PORTB |= 0b00010000;
     
}
void SPI_INI(void)
{
	LED1_ON;
	LED2_ON;
	
	//DDRA  |= 0b00000000;	   // DDRB  SPI
	//DDRA  &= 0b11111110;
	
	//PORTA &= 0b00001111;
	//PORTA |= 0b00010000;
		
	DDRB  |= 0b10110000;	   // DDRB  SPI
	DDRB  &= 0b10111111;
	
	PORTB &= 0b00001111;
	PORTB |= 0b00010000;
	//   SPI ENABLE|MSB first| Master  |    Mode:0,0       |  Fosc/4=2Mhz
	SPCR = (1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(1<<SPR0);
	SPSR = (0<<SPI2X); // скорость без множителя
	//UCSRC |= (1<<UMSEL); // Синхронный режим операций
	
	LED1_OFF;
	LED2_OFF;
	
}

void READY(void)
{
	LED1_ON;
	LED2_ON;
	LED3_ON;
	uint8_t ready_constant = 0x00;
/************************************************************************/
/*                                                                      */
/************************************************************************/	
	nCS_ON;	
	SPDR = WREN;
	while(!(SPSR & (1<<SPIF)));	
	nCS_OFF;
	
	do
	{
		nCS_ON;
		SPDR = RDSR;                      //RDSR
		while(!(SPSR & (1<<SPIF)));
		
		SPDR = 0x00;				      //RDSR
		while(!(SPSR & (1<<SPIF)));
		
		ready_constant = SPDR;
		nCS_OFF;
	} while ((ready_constant & 0x0F) != 0x02);                //RDY/BSY Status
/************************************************************************/
/*                                                                      */
/************************************************************************/

	LED1_OFF;
	LED2_OFF;
}

void WRITE_PAGE(uint16_t address)
{
	uint8_t ready_constant = 0x00;
	LED1_ON;
	LED2_OFF;
	//READY();
	
/************************************************************************/
/* ОПЕРАЦИЯ ЗАПИСИ ДАННЫХ                                               */
/************************************************************************/	
	
	
	nCS_ON;
	
	SPDR = WRDI;
	while(!(SPSR & (1<<SPIF)));
		
	nCS_OFF;
	
	do
	{
		nCS_ON;
		SPDR = RDSR;                      //RDSR
		while(!(SPSR & (1<<SPIF)));
			
		SPDR = 0x00;				      //RDSR
		while(!(SPSR & (1<<SPIF)));
		
		ready_constant = SPDR;
		nCS_OFF;
	} while (ready_constant != 0x00);
		
	nCS_ON;
	
	SPDR = WREN;
	while(!(SPSR & (1<<SPIF)));
	
	nCS_OFF;
	
	do
	{
		nCS_ON;
		SPDR = RDSR;                      //RDSR
		while(!(SPSR & (1<<SPIF)));
		
		SPDR = 0x00;				      //RDSR
		while(!(SPSR & (1<<SPIF)));
		
		ready_constant = SPDR;
		nCS_OFF;
	} while (ready_constant != 0x02);
	
	nCS_ON;
	
	SPDR = WRITE_CMD;
	while(!(SPSR & (1<<SPIF)));

	SPDR = (uint8_t)((address&0xFF00)>>8);  // Старший полубайт адреса
	while(!(SPSR & (1<<SPIF)));
	
	SPDR = (uint8_t)(address&0x00FF);      //  Младший полубайт адреса
	while(!(SPSR & (1<<SPIF)));

	for(int i=0; i<64;i++)           // ЗАПИСЬ ОДНОЙ СТРАНИЦЫ 64 БАЙТ
	{
		SPDR = mem_buff1[i];
		while(!(SPSR & (1<<SPIF)));
	}
	
	nCS_OFF;
	
	_delay_ms(10);
/************************************************************************/
/* ОПЕРАЦИЯ ЗАВЕРШЕНА, ВСЕМ СПАСИБО                                     */
/************************************************************************/
	do
	{
		nCS_ON;
		SPDR = RDSR;                      //RDSR
		while(!(SPSR & (1<<SPIF)));
			
		SPDR = 0x00;				      //RDSR
		while(!(SPSR & (1<<SPIF)));
			
		ready_constant = SPDR;
		nCS_OFF;
		//_delay_ms(10);
	} while (ready_constant != 0x00);
		
	LED1_OFF;
	LED2_OFF;
}

void READ_BYTE(uint16_t address)
{
	LED1_OFF;
	LED2_ON;
	READY();

/************************************************************************/
/* ОПЕРАЦИЯ ЧТЕНИЯ                                                      */
/************************************************************************/
	nCS_ON;
	
	SPDR = READ_CMD;
	while(!(SPSR & (1<<SPIF)));
	
	SPDR = (uint8_t)((address&0xFF00)>>8);                // Старший полубайт адреса
	while(!(SPSR & (1<<SPIF)));
	
	SPDR = (uint8_t)(address&0x00FF);                    //  Младший полубайт адреса
	while(!(SPSR & (1<<SPIF)));
	
//	SPDR = 0x00;
//	while(!(SPSR & (1<<SPIF)));
	
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
		nCS_ON;
		
		SPDR = RDSR;                      //RDSR
		while(!(SPSR & (1<<SPIF)));
		
		SPDR = 0x00;				      //RDSR
		while(!(SPSR & (1<<SPIF)));
		
		ready_constant = SPDR;
		nCS_OFF;
	} while (ready_constant != 0x00);  

/************************************************************************/
/* ОПЕРАЦИЯ ЧТЕНИЯ СТРАНИЦЫ НАЧАЛАСЬ                                    */
/************************************************************************/	
	
	nCS_ON;
	
	SPDR = READ_CMD;                           // Read Sequence READ Opcode 03h
	while(!(SPSR & (1<<SPIF)));

	SPDR = (uint8_t)address&0xFF00;                 // Address first byte
	while(!(SPSR & (1<<SPIF)));
	
	SPDR = (uint8_t)(address&0x00FF)>>8;            // Address second byte
	while(!(SPSR & (1<<SPIF)));
	
	for(int i = 0; i<64; i++)           // ЗАПИСЫВАЕМ В СВОЙ БУФФФЕР ДАННЫЕ С МК
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
	LED3_OFF;
	uint8_t status = 0;

	READY();
	MemBuf_WRITEFF();
	WRITE_PAGE(address);
	READ_PAGE(address);
	
	for (int i=0; i<64; i++)
	{
		if (mem_buff1[i] == mem_buff2[i]) status = 1;
		else 
		{
			status = 0;
			break;
		}
	}

	MemBuf_WRITE00();
	WRITE_PAGE(address);
	READ_PAGE(address);
	
	for (int i=0; i<64; i++)
	{
		if (mem_buff1[i] == mem_buff2[i]) status = 1;
		else
		{
			status = 0;
			break;
		}
	}
	
	MemBuf_WRITEAA();
	WRITE_PAGE(address);
	READ_PAGE(address);
	
	for (int i=0; i<64; i++)
	{
		if (mem_buff1[i] == mem_buff2[i]) status = 1;
		else
		{
			status = 0;
			break;
		}
	}
	
	MemBuf_WRITE55();
	WRITE_PAGE(address);
	READ_PAGE(address);
	
	for (int i=0; i<64; i++)
	{
		if (mem_buff1[i] == mem_buff2[i]) status = 1;
		else
		{
			status = 0;
			break;
		}
	}
	LED1_OFF;
	LED2_OFF;
	return(status);
	
}