#include "SPI.h"
#include "Preference.h"

//nCS - PB4
//MO  - PB5 (БШУНД лй)
//MI  - PB6 (БУНД лй)
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
	SPSR = (0<<SPI2X); // ЯЙНПНЯРЭ АЕГ ЛМНФХРЕКЪ

	DDRD &= 0b11111100;
	DDRD |= 0b00000011;	   // DDRB  SPI
	
	PORTB &= 0b11111100;
	PORTB |= 0b00000011;
     
}
void SPI_INI(void)
{
	LED1_ON;
	LED2_ON;

	//   SPI ENABLE|MSB first| Master  |    Mode:0,0       |  Fosc/4=2Mhz
	SPCR = (1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0);
	SPSR = (0<<SPI2X); // ЯЙНПНЯРЭ АЕГ ЛМНФХРЕКЪ
	
	DDRD &= 0b11111100;
	DDRD |= 0b00000011;	   // DDRB  SPI
		
	PORTB &= 0b11111100;
	PORTB |= 0b00000011;
		
	LED1_OFF;
	LED2_OFF;
	
}


void WRITE_BYTE(uint8_t Byte_Location, uint8_t Byte_count)
{
//	uint8_t ready_constant = 0x00;
	LED1_ON;
	LED2_OFF;
	
/************************************************************************/
/* ярюпрнбши ахр дкъ гюохях                                             */
/************************************************************************/	
	SCL_1;
	SDA_1;
	_delay_us(5);
	SDA_0;
	
/************************************************************************/
/* ноепюжхъ гюохях                                                      */
/************************************************************************/	
	
	SPDR = WRITE_CMD;
	while(!(SPSR & (1<<SPIF)));

	SPDR = Byte_Location;                    //  Beginning byte location
	while(!(SPSR & (1<<SPIF)));
	
	SPDR = Byte_count;                    //  Data byte count
	while(!(SPSR & (1<<SPIF)));

	for(int i=0; i<Byte_count;i++)           // Bit write
	{
		SPDR = mem_buff1[i];
		while(!(SPSR & (1<<SPIF)));
	}
/************************************************************************/
/* нропюбйю ярно ахрю                                                   */
/************************************************************************/
	SDA_0;
	SCL_1;
	SDA_1;
	_delay_ms(5);
/************************************************************************/
/* ноепюжхъ гюбепьемю, бяел яоюяхан                                     */
/************************************************************************/	
/*	do 
	{
	
		SPDR = RDSR;                      //RDSR
		while(!(SPSR & (1<<SPIF)));
	
		SPDR = 0x00;				      //RDSR
		while(!(SPSR & (1<<SPIF)));
		
		ready_constant = SPDR;
		
	} while ((ready_constant & 0x01) != 0x00);
*/	
	_delay_ms(1);
	LED1_OFF;
	LED2_OFF;
}

void READ_BYTE(uint8_t Byte_Location, uint8_t Byte_count)
{
	LED1_OFF;
	LED2_ON;
	_delay_us(5);
	
/************************************************************************/
/* ярюпрнбши ахр                                                        */
/************************************************************************/
	SCL_1;
	SDA_1;
	_delay_us(5);
	SDA_0;

/************************************************************************/
/* ноепюжхъ времхъ                                                      */
/************************************************************************/
	
	SPDR = WRITE_CMD;
	while(!(SPSR & (1<<SPIF)));
	
	SPDR = Byte_Location;                    //  аЮИР ЮДПЕЯЮ
	while(!(SPSR & (1<<SPIF)));
	
	SPDR = 0x00;
	while(!(SPSR & (1<<SPIF)));
		
	SPDR = READ_CMD;
	while(!(SPSR & (1<<SPIF)));
	
	SPDR = Byte_count;                    //  аЮИР ЮДПЕЯЮ
	while(!(SPSR & (1<<SPIF)));
		
	buff_check = SPDR;
	
	for (int i=0;i<Byte_count;i++)
	{
		SPDR = 0x00;
		while(!(SPSR & (1<<SPIF)));
		
		mem_buff2[i] = SPDR;
		_delay_us(1);
	}
	
/************************************************************************/
/* нропюбйю ярно ахрю                                                   */
/************************************************************************/
	SDA_0;
	SCL_1;
	SDA_1;
	_delay_ms(5);
	
/************************************************************************/
/*  ноепюжхъ гюбепьемю, бяел яоюяхан                                    */
/************************************************************************/	
	LED1_OFF;
	LED2_OFF;
}

uint8_t CHECK_ID()
{
	uint8_t result = 0;
	
	LED1_OFF;
	LED2_OFF;
//	uint8_t status = 0;

/************************************************************************/
/* ярюпрнбши ахр                                                        */
/************************************************************************/
	SCL_1;
	SDA_1;
	_delay_us(5);
	SDA_0;
	
	SPDR = ID_CMD;                    //  аЮИР ЮДПЕЯЮ
	while(!(SPSR & (1<<SPIF)));
	
	SPDR = 0x00;
	while(!(SPSR & (1<<SPIF)));
	
	 result = SPDR;
	
	_delay_us(1);		

/************************************************************************/
/* нропюбйю ярно ахрю                                                   */
/************************************************************************/
	SDA_0;
	SCL_1;
	SDA_1;
	_delay_ms(5);

/************************************************************************/
/*  ноепюжхъ гюбепьемю, бяел яоюяхан                                    */
/************************************************************************/
	LED1_OFF;
	LED2_OFF;
	return(result);
	
}