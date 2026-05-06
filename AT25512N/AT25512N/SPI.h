#ifndef SPI_H_
#define SPI_H_

#include "Preference.h"

#define WREN  0x06  // SET WRITE ENABLE LATCH
#define WRDI  0x04  // RESET WRITE ENABLE LATCH
#define RDSR  0x05  // READ STATUS REGISTER
#define WRSR  0x01  // WRITE STATUS REGISTER

#define READ_CMD  0x03  // READ FROM MEMORY ARRAY
#define WRITE_CMD 0x02  // WRITE TO MEMORY ARRAY

#define MO_1         PORTB|=0b00100000;     // PB5 Лог. 1
#define MO_0         PORTB&=0b11011111;     // PB5 Лог. 0

#define MI_1         PORTB|=0b01000000;     //  PB6 Лог. 1
#define MI_0         PORTB&=0b10111111;     // PB6 Лог. 0

#define SCK_1        PORTB|=0b10000000;     // PB7 Лог. 1
#define SCK_0        PORTB&=0b01111111;     // PB7 Лог. 0

#define CS_1         PORTB|=0b00010000;     // PB4 Лог. 1
#define CS_0         PORTB&=0b11101111;     // PB4 Лог. 0

#define SPI_time  4                         // Задержка


void MemBuf_WRITE(void);

void MemBuf_WRITE0(void);

void MemBuf_WRITE1(void);

void SPI_INI(void);

void SPI_Off(void);

void READY(void);

void WRITE_PAGE(uint16_t address);

void READ_BYTE(uint16_t address);

void READ_PAGE(uint16_t address);

uint8_t CHECK_PAGE(uint16_t address);


#endif /* SPI_H_ */