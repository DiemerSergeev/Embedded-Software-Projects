#ifndef XRA1201DRIVER_H_
#define XRA1201DRIVER_H_

#define I2C_START		    0
#define I2C_RESTART         1
#define I2C_STOP            2
#define I2C_TRANSMIT        3
#define I2C_RECEIVE_ACK     4
#define I2C_RECEIVE_NACK    5




//Aдрес диджипода
#define  WRITE_CMD  0x40			 // јдресс ћ» –ќ—’≈ћџ+0, будем писать (0  1  0  0  0  0  0  R/W) 01000000 (0x40);
#define  READ_CMD   0x41			 // јдресс ћ» –ќ—’≈ћџ+1, будем читать (0  1  0  0  0  0  0  R/W) 01000001 (0x41);

//–егистры
#define GSR1_XRA    0x00
#define GSR2_XRA    0x01
#define OCR1_XRA    0x02
#define OCR2_XRA    0x03
#define PIR1_XRA    0x04
#define PIR2_XRA    0x05
#define GCR1_XRA    0x06
#define GCR2_XRA    0x07
#define PUR1_XRA    0x08
#define PUR2_XRA    0x09
#define IER1_XRA    0x0A
#define IER2_XRA    0x0B
#define TSCR1_XRA   0x0C
#define TSCR2_XRA   0x0D
#define ISR1_XRA    0x0E
#define ISR2_XRA    0x0F
#define REIR1_XRA   0x10
#define REIR2_XRA   0x11
#define FEIR1_XRA   0x12
#define FEIR2_XRA   0x13
#define IFR1_XRA    0x14
#define IFR2_XRA    0x15


void I2C_INIT(void);
uint8_t I2C_COMAND(uint8_t action);
uint8_t READ_REGISTER(uint8_t REG_CMD);
void WRITE_REGISTER(uint8_t REG_CMD,uint8_t data);
uint8_t XRA1201_Chip(void);
void XRA1201_Reset(void);
void XRA1201_SET_ONE(void);
void XRA1201_SET_ZERO(void);
void XRA1201_SET_INPUT(void);





#endif /* XRA1201DRIVER_H_ */