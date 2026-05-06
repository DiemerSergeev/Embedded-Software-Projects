#ifndef MAX6581_TDRIVER_H_
#define MAX6581_TDRIVER_H_

#define I2C_START		    0
#define I2C_RESTART         1
#define I2C_STOP            2
#define I2C_TRANSMIT        3
#define I2C_RECEIVE_ACK     4
#define I2C_RECEIVE_NACK    5

#define WRITE_CMD 0x9A
#define READ_CMD 0x9B
//Регистры
#define Conf_Reg    0x41             // Configurator register

#define ALERT_Mask  0x42
#define OVERT_Mask  0x43

////////////////////////////////////////////////////////////////////////////
#define LOCAL_ALERT_HIGH_LIMIT 0x17

////////////////////////////////////////////////////////////////////////////
#define LOCAL_OVERT_HIGH_LIMIT 0x20

////////////////////////////////////////////////////////////////////////////
#define LOCAL_TEMPERATURE 0x07
#define LOCAL_EXTENDED 0x57

////////////////////////////////////////////////////////////////////////////
#define REMOTE1_TEMPERATURE 0x01
#define REMOTE1_EXTENDED 0x51

#define REMOTE2_TEMPERATURE 0x02
#define REMOTE2_EXTENDED 0x52

#define REMOTE3_TEMPERATURE 0x03
#define REMOTE3_EXTENDED 0x53

#define REMOTE4_TEMPERATURE 0x04
#define REMOTE4_EXTENDED 0x54

#define REMOTE5_TEMPERATURE 0x05
#define REMOTE5_EXTENDED 0x55

#define REMOTE6_TEMPERATURE 0x06
#define REMOTE6_EXTENDED 0x56

#define REMOTE7_TEMPERATURE 0x08
#define REMOTE7_EXTENDED 0x58

////////////////////////////////////////////////////////////////////////////
#define ALERT_LOW_LIMITS 0x30

////////////////////////////////////////////////////////////////////////////
#define ALERT_HIGH_STATUS_READ 0x44

////////////////////////////////////////////////////////////////////////////
#define OVERT_STATUS_READ 0x45

////////////////////////////////////////////////////////////////////////////
#define ALERT_LOW_STATUS_READ 0x47

////////////////////////////////////////////////////////////////////////////
#define ALERT_LOW_DISABLE 0x48

////////////////////////////////////////////////////////////////////////////
#define MANUFACTURER_ID 0x0A
#define REVISION_ID 0x0F

////////////////////////////////////////////////////////////////////////////
#define OFFSET_T_REGISTER 0x4D
#define OFFSET_SELECT_REGISTER 0x4E

void I2C_INIT(void);

uint8_t I2C_COMAND(uint8_t action);

uint8_t READ_ONE_BYTE_REG(uint8_t REG_CMD);

uint16_t READ_TWO_BYTE_REG(uint8_t REG_CMD1, uint8_t REG_CMD2);

void WRITE_ONE_BYTE_REG(uint8_t REG_CMD, uint8_t data);

uint8_t MAX6581_Init(void);

float MAX6581_Read_Temp(uint8_t REG_CMD1,uint8_t REG_CMD2);


#endif /* MAX6581_TDRIVER_H_ */