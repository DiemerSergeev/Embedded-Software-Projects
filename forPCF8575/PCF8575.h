
#ifndef PCF8575_H_
#define PCF8575_H_

#define I2C_START		    0
#define I2C_RESTART         1
#define I2C_STOP            2
#define I2C_TRANSMIT        3
#define I2C_RECEIVE_ACK     4
#define I2C_RECEIVE_NACK    5

void I2C_INIT(void);
uint8_t I2C_COMAND(uint8_t action);
uint8_t FindChip(void);
uint16_t READ_REGISTER(void);
void WRITE_REGISTER(uint16_t data);
//float READ_TEMP_LOC(void);
//float READ_TEMP_REM(uint8_t remote_num);




#endif /* PCF8575_H_ */