
#define I2C_START		    0
#define I2C_RESTART         1
#define I2C_STOP            2
#define I2C_TRANSMIT        3
#define I2C_RECEIVE_ACK     4
#define I2C_RECEIVE_NACK    5

#define WRITE_FIRST_BLOCK   0xA0
#define WRITE_SECOND_BLOCK  0xA2
#define WRITE_THIRD_BLOCK   0xA4
#define WRITE_FOURTH_BLOCK  0xA6
#define WRITE_FIFTH_BLOCK   0xA8
#define WRITE_SIXTH_BLOCK   0xAA
#define WRITE_SEVENTH_BLOCK 0xAC
#define WRITE_EIGHTH_BLOCK  0xAE

#define READ_FIRST_BLOCK    0xA1
#define READ_SECOND_BLOCK   0xA3
#define READ_THIRD_BLOCK    0xA5
#define READ_FOURTH_BLOCK   0xA7
#define READ_FIFTH_BLOCK    0xA9
#define READ_SIXTH_BLOCK    0xAB
#define READ_SEVENTH_BLOCK  0xAD
#define READ_EIGHTH_BLOCK   0xAF


void I2C_INIT(void);

uint8_t I2C_COMAND(uint8_t action);

uint8_t READ_ONE_SEQUENTIAL(uint8_t CONTROL_BYTE, uint8_t data);

void WRITE_PAGE(uint8_t CONTROL_BYTE, uint8_t DATA);

uint8_t CHECK_SCHEME(uint8_t CONTROL_BYTE_WRITE, uint8_t CONTROL_BYTE_READ, uint8_t data);
