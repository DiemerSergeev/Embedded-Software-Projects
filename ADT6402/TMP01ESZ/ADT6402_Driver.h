#ifndef ADT6402_TDRIVER_H_
#define ADT6402_TDRIVER_H_
/////////////////////////////////////////////////////////////////
#define I2C_START		    0
#define I2C_RESTART         1
#define I2C_STOP            2
#define I2C_TRANSMIT        3
#define I2C_RECEIVE_ACK     4
#define I2C_RECEIVE_NACK    5

// ADD0 & ADD1 TMP100 - GND
// ADDR = 0'b1001000

// READ COMMAND
#define READ_CMD 0x91

// WRITE COMMAND
#define WRITE_CMD 0x90

// TEMPERATURE REGISTER (READ ONLY)
#define TEMP_REG 0x00

// CONFIGURATION REGISTER (R/W)
#define CONF_REG 0x01

// TLOW REGISTER (R/W)
#define TLOW_REG 0x02

// THIGH REGISTER (R/W)
#define THIGH_REG 0x03

uint8_t I2C_COMAND(uint8_t action);

void I2C_INIT(void);

uint8_t READ_BYTE(uint8_t COMMAND);

uint16_t READ_2BYTE(uint8_t COMMAND);

void WRITE_BYTE(uint8_t COMMAND, uint8_t data);

uint8_t TMP100_Init(void);

float Temp_read(void);

void Problem(uint8_t problem, uint8_t data);

#endif