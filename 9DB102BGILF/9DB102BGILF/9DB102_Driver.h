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
uint8_t mem_buff[6];

/////////////////////////////////////////////////////////////////
// R/W
#define  WRITE_CMD  0xD4
//#define  WRITE_CMD  0x6A
#define  READ_CMD   0xD5

/////////////////////////////////////////////////////////////////
// REGISTERS

// | SW_EN | RES | RES | RES | RES | RES | PLL_BW_ADJUST | PLL_ENABLE |
// SW = 0 - SMBUS CONTROL
// SW = 1 - PINS CONTROLS
// BW = 0 - LOW BW
// BW = 1 - HIGH BW
// ENABLE = 0 - BYPASSED
// ENABLE = 1 - ENABLED
#define CONTROL_REG_BYTE   0 // R/W

// | RES | RES | RES | RES | RES | RES | RES | RES |
#define OUTPUT_ENABLE_BYTE 1 // R/W

// | RES | RES | RES | RES | RES | RES | RES | RES |
#define FUNCTION_SELECT_BYTE 2 // R/W

// | RID3 | RID2 | RID1 | RID0 | VID3 | VID2 | VID1 | VID0 |
// RID0 = 0
// VID0 = 0
#define REVISION_VENDOR_ID_BYTE 3 // R

// | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0 |
// DEVICE_ID = 0x06
#define DEVICE_ID_BYTE 4 // R

// | BC7 | BC6 | BC5 | BC4 | BC3 | BC2 | BC1 | BC0 |
// DEFAULT 0x06
#define COUNT_BYTE 5

/////////////////////////////////////////////////////////////////
void I2C_INIT(void);

/////////////////////////////////////////////////////////////////
uint8_t I2C_COMAND(uint8_t action);

/////////////////////////////////////////////////////////////////
void READ_BYTES(uint8_t N, uint8_t X);

/////////////////////////////////////////////////////////////////
void WRITE_BYTES(uint8_t data, uint8_t N, uint8_t X);

/////////////////////////////////////////////////////////////////
void ICS9DB102_CHECK(void);

/////////////////////////////////////////////////////////////////
void Problem(uint8_t problem, uint8_t data);

#endif