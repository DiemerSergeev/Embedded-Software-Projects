#ifndef SPI_H_
#define SPI_H_

#include "Preference.h"
/*
#define WREN  0x06  // SET WRITE ENABLE LATCH
#define WRDI  0x04  // RESET WRITE ENABLE LATCH
#define RDSR  0x05  // READ STATUS REGISTER
#define WRSR  0x01  // WRITE STATUS REGISTER
*/
#define READ_CMD  0xD5  // READ FROM MEMORY ARRAY
#define WRITE_CMD 0xD4  // WRITE TO MEMORY ARRAY

#define ID_CMD 0x06     // ID DATA

#define SCL_1        PORTD|=0b00000001;     // PD0 Лог. 1
#define SCL_0        PORTD&=0b11111110;     // PD0 Лог. 0

#define SDA_1        PORTD|=0b00000010;     // PD1 Лог. 1
#define SDA_0        PORTD&=0b11111101;     // PD1 Лог. 0

//Реле 1 - PD4(Управляет SDA и SCL) по умолчанию на линии контроллера
//Реле 2 - PC7(Управляет PCI-EX0) по умолчанию на линии контроллера
//Реле 3 - PA6(Управляет PCI-EX1) по умолчанию на линии контроллера
//Реле 4 - PA4(управляет PCI-EX2) по умолчанию на линии контроллера
//Реле 5 - PB1(Управляет PCI-EX3) по умолчанию на линии контроллера
//Реле 6 - PC3(Управляет PCI-EX4) по умолчанию на линии контроллера
//Реле 7 - PD7(управляет PCI-EX5) по умолчанию на линии контроллера
//Реле 8 - PA2(Управляет PCI-O) по умолчанию на линии контроллера
//Реле 9 - PC5(Управляет CLK-IN) по умолчанию на линии контроллера


void MemBuf_WRITE(void);

void MemBuf_WRITE0(void);

void MemBuf_WRITE1(void);

void SPI_INI(void);

void SPI_Off(void);

void WRITE_BYTE(uint8_t Byte_Location, uint8_t Byte_count);

void READ_BYTE(uint8_t Byte_Location, uint8_t Byte_count);

uint8_t CHECK_ID();


#endif /* SPI_H_ */