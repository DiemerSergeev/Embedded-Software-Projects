#ifndef DS1682_TDRIVER_H_
#define DS1682_TDRIVER_H_

#define I2C_START		    0
#define I2C_RESTART         1
#define I2C_STOP            2
#define I2C_TRANSMIT        3
#define I2C_RECEIVE_ACK     4
#define I2C_RECEIVE_NACK    5

#define  WRITE_CMD  0xD6	// јдресс ћ» –ќ—’≈ћџ+0, будем писать (1  1  0  1  0  1  1  R/W) 11010110 (0xD6);
#define  READ_CMD   0xD7	// јдресс ћ» –ќ—’≈ћџ+1, будем читать (1  0  0  1  0  1  1  R/W) 11010111 (0xD7);

/************************************************************************/
/*                          –егистры                                    */
/************************************************************************/
// ALARM
#define ALARM_LOW_REG				0x01
#define ALARM_LOW_MIDDLE_REG		0x02
#define ALARM_HIGH_MIDDLE_REG		0x03
#define ALARM_HIGH_REG				0x04
// ELAPSED TIME COUNTER
#define ETC_LOW_REG					0x05
#define ETC_LOW_MIDDLE_REG			0x06
#define ETC_HIGH_MIDDLE_REG			0x07
#define ETC_HIGH_REG				0x08
// EVENT COUNTER
#define EVENT_LOW_REG				0x09
#define EVENT_HIGH_REG				0x0A
// USER MEMORY
#define USER_BYTE1					0x0B
#define USER_BYTE2					0x0C
#define USER_BYTE3					0x0D	
#define USER_BYTE4					0x0E
#define USER_BYTE5					0x0F
#define USER_BYTE6					0x10
#define USER_BYTE7					0x11
#define USER_BYTE8					0x12
#define USER_BYTE9					0x13
#define USER_BYTE10					0x14
// NOT USED
#define ZERO_READ					0x15
// RESET COMMAND
#define RESET_REG					0x1D
// WRITE DISABLE
#define WRITE_DISABLE_REG			0x1E
// WRITE MEMORY DISABLE
#define WRITE_MEMORY_DISABLE_REG	0x1F
// RESET COMMAND
#define  RESET_MC					0x55
// WRITE DISABLE COMMAND
#define WRITE_DISABLE_MC			0xAA
// WRITE MEMORY DISABLE COMMAND
#define WRITE_MEMORY_DISABLE_MC		0xF0

// CONFIGURATION REG
#define CONF_REG					0x00

void I2C_INIT(void);

uint8_t I2C_COMAND(uint8_t action);

uint8_t READ_ONE_REG(uint8_t REG_CMD);

void WRITE_ONE_REG(uint8_t REG_CMD, uint8_t data);

uint8_t DS1682_CHECK(uint8_t data);

#endif