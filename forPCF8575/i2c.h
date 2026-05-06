#ifndef I2C_H_
#define I2C_H_
#include "main.h" 

void I2C_INIT (void);
uint8_t I2C_Start(void);
void I2C_Stop(void);
void I2C_TransmitByte(unsigned char c);
unsigned char I2C_ReceiveByte(void);
unsigned char I2C_ReceiveLastByte(void);
uint8_t I2C_ErrorMessage(void);


#endif /* I2C_H_ */