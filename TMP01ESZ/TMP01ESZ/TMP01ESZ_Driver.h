#ifndef TMP01ESZ_TDRIVER_H_
#define TMP01ESZ_TDRIVER_H_

#define nCS0    PORTB&=0b11111110;     // установка линии nCS в 0
#define nCS1    PORTB|=0b00000001;     // установка линии nCS в 1

void SPI_ini(void);

float TMP01ESZ_read(void);

uint8_t TMP01ESZ_Init(void);

#endif /* AD7416_TDRIVER_H_ */