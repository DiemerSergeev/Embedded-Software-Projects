#ifndef PCA950X_TDRIVER_H_
#define PCA950X_TDRIVER_H_
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
//     0       1       0       0       0       0       0      0
#define  WRITE_CMD  0x40

/////////////////////////////////////////////////////////////////
// | ADDR7 | ADDR6 | ADDR5 | ADDR4 | ADDR3 | ADDR2 | ADDR1 | R/W |
//     7       6       5       4       3       2       1      0
//     0       1       0       0       0       0       0      1
#define  READ_CMD   0x41

/////////////////////////////////////////////////////////////////
// INPUT PORT REGISTERS (READ ONLY)
#define	IP0 0x00  // bank0
#define IP1 0x01  // bank1
#define IP2 0x02  // bank2
#define IP3 0x03  // bank3
#define IP4 0x04  // bank4
/////////////////////////////////////////////////////////////////
// OUTPUT PORT REGISTERS (R/W)
#define OP0 0x08  // bank0
#define OP1 0x09  // bank1
#define OP2 0x0A  // bank2
#define OP3 0x0B  // bank3
#define OP4 0x0C  // bank4
/////////////////////////////////////////////////////////////////
// POLARITY INVERSION REGISTERS (R/W)
#define PI0 0x10  // bank0
#define PI1 0x11  // bank1
#define PI2 0x12  // bank2
#define PI3 0x13  // bank3
#define PI4 0x14  // bank4
/////////////////////////////////////////////////////////////////
// I/O CONFIGURATION REGISTERS (R/W)
#define IOC0 0x18  // bank0
#define IOC1 0x19  // bank1
#define IOC2 0x1A  // bank2
#define IOC3 0x1B  // bank3
#define IOC4 0x1C  // bank4
/////////////////////////////////////////////////////////////////
// MASK INTERRUPT REGISTERS (R/W)
#define MSK0 0x20  // bank0
#define MSK1 0x21  // bank1
#define MSK2 0x22  // bank2
#define MSK3 0x23  // bank3
#define MSK4 0x24  // bank4
/////////////////////////////////////////////////////////////////
// AUTOINCREMENT
#define INC 0x80
/////////////////////////////////////////////////////////////////
void I2C_INIT(void);
/////////////////////////////////////////////////////////////////
uint8_t I2C_COMAND(uint8_t action);
/////////////////////////////////////////////////////////////////
uint8_t READ_ONE_BYTE(uint8_t COMMAND);
/////////////////////////////////////////////////////////////////
uint64_t READ_ALL_BYTES(uint8_t COMMAND);
/////////////////////////////////////////////////////////////////
void WRITE_ONE_BYTE(uint8_t COMMAND, uint8_t data);
/////////////////////////////////////////////////////////////////
void WRITE_ALL_BYTES(uint8_t COMMAND, uint8_t data);
/////////////////////////////////////////////////////////////////
void PCA950X_CHECK(void);
/////////////////////////////////////////////////////////////////
void Problem(uint8_t problem, uint8_t data);

#endif