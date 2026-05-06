#ifndef PCA9544A_TDRIVER_H_
#define PCA9544A_TDRIVER_H_
/////////////////////////////////////////////////////////////////

#define I2C_START		    0
#define I2C_RESTART         1
#define I2C_STOP            2
#define I2C_TRANSMIT        3
#define I2C_RECEIVE_ACK     4
#define I2C_RECEIVE_NACK    5

/////////////////////////////////////////////////////////////////
// | ADDR7 | ADDR6 | ADDR5 | ADDR4 | ADDR3 | ADDR2 | ADDR1 | R/W |
//     7       6       5       4       3       2       1      0
//     1       1       1       0       0       0       0      0
#define  WRITE_CMD  0xE0

/////////////////////////////////////////////////////////////////
// | ADDR7 | ADDR6 | ADDR5 | ADDR4 | ADDR3 | ADDR2 | ADDR1 | R/W |
//     7       6       5       4       3       2       1      0
//     1       1       1       0       0       0       0      1
#define  READ_CMD   0xE1

/////////////////////////////////////////////////////////////////
// ENABLE REGISTERS
#define NOT_ENABLE 0x00
#define CH0_ENABLE 0x04
#define CH1_ENABLE 0x05
#define CH2_ENABLE 0x06
#define CH3_ENABLE 0x07
/////////////////////////////////////////////////////////////////
void I2C_INIT(void);
/////////////////////////////////////////////////////////////////
uint8_t I2C_COMAND(uint8_t action);
/////////////////////////////////////////////////////////////////
uint8_t READ_ONE_BYTE(void);
/////////////////////////////////////////////////////////////////
void WRITE_ONE_BYTE(uint8_t data);
/////////////////////////////////////////////////////////////////
void PCA9544A_CHECK(void);
/////////////////////////////////////////////////////////////////
void Problem(uint8_t problem, uint8_t data);

#endif