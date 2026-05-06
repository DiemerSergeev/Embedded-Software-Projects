#ifndef DRIVERLM57_H_
#define DRIVERLM57_H_


#define nCS0    PORTB&=0b11101111;     // установка линии nCS в 0
#define nCS1    PORTB|=0b00010000;     // установка линии nCS в 1

#define TRIP_T_OFF    PORTC&=0b11111011;  // установка линии TRIP_TEST в 0
#define TRIP_T_ON    PORTA|=0b00000100;  // установка линии TRIP_TEST в 1

void SPI_ini(void);
float LM57_read(void);


#endif /* DRIVERLM57_H_ */