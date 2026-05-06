#ifndef DS7505U_TDRIVER_H_
#define DS7505U_TDRIVER_H_

#define I2C_START		    0
#define I2C_RESTART         1
#define I2C_STOP            2
#define I2C_TRANSMIT        3
#define I2C_RECEIVE_ACK     4
#define I2C_RECEIVE_NACK    5

//Aдрес диджипода
#define  WRITE_CMD  0x90			 // јдресс ћ» –ќ—’≈ћџ+0, будем писать (1  0  0  1  A2  A1  A0  R/W) 10010000 (0x90);
#define  READ_CMD   0x91			 // јдресс ћ» –ќ—’≈ћџ+1, будем читать (1  0  0  1  A2  A1  A0  R/W) 10010001 (0x91);

//–егистры
#define Temp_Reg    0x00
#define Conf_Reg    0x01
#define THyst_Reg   0x02
#define TOS_Reg     0x03

#define Device_address 0b1001000

void I2C_INIT(void);
uint8_t I2C_COMAND(uint8_t action);
uint8_t READ_ONE_BYTE_REG(uint8_t REG_CMD);
uint16_t READ_TWO_BYTE_REG(uint8_t REG_CMD);
void WRITE_ONE_BYTE_REG(uint8_t REG_CMD, uint8_t data);
void WRITE_TWO_BYTE_REG(uint8_t REG_CMD, uint16_t data);
uint8_t LM75_Init(void);
uint8_t LM75_ReadConf(void);
float LM75_Read_Temp(void);
void LM75_Set_TOS(float temp_os);
void LM75_Set_THYST(float temp_HYST);
uint16_t TempConverter(uint16_t temp);

#endif /* DS7505U_TDRIVER_H_ */