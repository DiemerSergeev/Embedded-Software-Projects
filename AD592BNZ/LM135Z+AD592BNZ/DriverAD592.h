#ifndef DRIVERLM135Z_H_
#define DRIVERLM135Z_H_

#define nCS0	PORTB &= 0b11101111;     // установка линии nCS в 0 (PB4)
#define nCS1	PORTB |= 0b00010000;     // установка линии nCS в 1 (PB4)

void SPI_ini(void);

float expRunningAverageAdaptive(float newVal);

float simpleKalman(float newVal); 

float AD592_read(void);

#endif /* DRIVERLM135Z_H_ */