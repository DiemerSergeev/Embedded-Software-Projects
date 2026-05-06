#ifndef DS1682_TDRIVER_H_
#define DS1682_TDRIVER_H_
/////////////////////////////////////////////////////////////////
#define I2C_START		    0
#define I2C_RESTART         1
#define I2C_STOP            2
#define I2C_TRANSMIT        3
#define I2C_RECEIVE_ACK     4
#define I2C_RECEIVE_NACK    5
/////////////////////////////////////////////////////////////////
#define  WRITE_CMD  0x30	// јдресс ћ» –ќ—’≈ћџ+0, будем писать (0  0  1  1  0  0  0  R/W) 00110000 (0x30); јдрес команд приведен дл€ ADD0 и ADD1 подт€нутых к GND
#define  READ_CMD   0x31	// јдресс ћ» –ќ—’≈ћџ+1, будем читать (0  0  1  1  0  0  0  R/W) 00110001 (0x31); јдрес команд приведен дл€ ADD0 и ADD1 подт€нутых к GND
/////////////////////////////////////////////////////////////////
#define RLTS	0x00 // Read local temperature: returns latest temperature
#define RRTE	0x01 // Read remote temperature: returns latest temperature
#define RSL		0x02 // Read status byte (flags, busy signal)
#define RCL		0x03 // Read configuration byte
#define RCRA	0x04 // Read conversion rate byte
#define RLHN	0x05 // Read local THIGH limit 
#define RLLI	0x06 // Read local TLOW limit
#define RRHI	0x07 // Read remote THIGH limit
#define RRLS	0x08 // Read remote TLOW limit
#define WCA		0x09 // Write configuration byte
#define WCRW	0x0A // Write conversion rate byte
#define WLHO	0x0B // Write local THIGH limit
#define WLLM	0x0C // Write local TLOW limit
#define WRHA	0x0D // Write remote THIGH limit
#define WRLN	0x0E // Write remote TLOW limit
#define OSHT	0x0F // One-shot command (use send-byte format)
/////////////////////////////////////////////////////////////////
#define SPOR	0xFC // Write software POR
#define MFGID	0xFE // Read manufacturer ID code
#define DEVID	0xFF // Read device ID code
/////////////////////////////////////////////////////////////////
//#define ALERT_RESPONSE 0x19
#define ALERT_RESPONSE 0x0C

void I2C_INIT(void);

void START(void);

uint8_t I2C_COMAND(uint8_t action);

uint8_t READ_BYTE(uint8_t COMMAND);

uint8_t RECEIVE_BYTE(void);

void WRITE_BYTE(uint8_t COMMAND, uint8_t data);

void SEND_BYTE(uint8_t data);

uint8_t MAX1617A_CHECK(uint8_t data);

int Set_THIGH(int temp);

int Set_TLOW(int temp);

int TEMP_CONVERSION(int temp);

int TEMP_RECONVERSION(int temp);

void Problem(void);

uint8_t READ_ALERT_RESPONSE(void);

#endif