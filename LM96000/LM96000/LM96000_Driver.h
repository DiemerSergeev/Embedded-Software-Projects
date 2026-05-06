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
// | ADDR7 | ADDR6 | ADDR5 | ADDR4 | ADDR3 | ADDR2 | ADDR1 | R/W |
//     7       6       5       4       3       2       1      0
//     0       1       0       1       1       0       0      0
#define  WRITE_CMD  0x58
// | ADDR7 | ADDR6 | ADDR5 | ADDR4 | ADDR3 | ADDR2 | ADDR1 | R/W |
//     7       6       5       4       3       2       1      0
//     0       1       0       1       1       0       0      1
#define  READ_CMD   0x59
/////////////////////////////////////////////////////////////////
// VOLTAGE READING (READ ONLY)
#define	REG_25V 0x20   
#define REG_VCCP_IN 0x21
#define REG_33V 0x22
#define REG_5V 0x23
#define REG_12V 0x24
/////////////////////////////////////////////////////////////////
// TEMPERATURE READING (READ ONLY)
#define REG_PROCESS_ZONE1_TEMP 0x25 // REMOTE1
#define REG_INTERNAL_ZONE2_TEMP 0x26 // INTERNAL
#define REG_REMOTE_ZONE3 0x27 // REMOTE2
/////////////////////////////////////////////////////////////////
// FAN TACHOMETER READING (READ ONLY)
#define REG_TACH1_LSB 0x28
#define REG_TACH1_MSB 0x29
#define REG_TACH2_LSB 0x2A
#define REG_TACH2_MSB 0x2B
#define REG_TACH3_LSB 0x2C
#define REG_TACH3_MSB 0x2D
#define REG_TACH4_LSB 0x2E
#define REG_TACH4_MSB 0x2F
/////////////////////////////////////////////////////////////////
// CURRENT PWM DUTY
#define REG_PWM1 0x30
#define REG_PWM2 0x31
#define REG_PWM3 0x32
/////////////////////////////////////////////////////////////////
// COMPANY ID (0x01) (READ ONLY)
#define REG_ID 0x3E
/////////////////////////////////////////////////////////////////
// VERSION, STEPPING (0x68) (READ ONLY)
#define REG_VERSION 0x3F
/////////////////////////////////////////////////////////////////
// | RES | RES | RES | RES | OVRID | READY | LOCK | START |
//    7     6     5     4      3       2       1      0
#define REG_READY_LOCK_START_OVERRIDE 0x40
/////////////////////////////////////////////////////////////////
// INTERRUPT STATUS REGISTER 1 (READ ONLY)
// | ERROR STATUS REG 2 | ZONE3 LIMIT EXCEED | ZONE2 LIMIT EXCEED | ZONE1 LIMIT EXCEED | 5V ERROR | 3.3V ERROR | VCCP_IN ERROR | 2.5V ERROR
//          7                       6                   5                   4               3            2             1             0
#define REG_INT1 0x41
/////////////////////////////////////////////////////////////////
// INTERRUPT STATUS REGISTER 1 (READ ONLY)
// | REMOTE DIODE 2 FAULT | REMOTE DIODE 1 FAULT | FAN4 STALLED | FAN3 STALLED | FAN2 STALLED | FAN1 STALLED | RES | +12V ERROR |
//             7                      6                  5              4              3              2         1         0
#define REG_INT2 0x42
/////////////////////////////////////////////////////////////////
// VID (READ ONLY)
// | RES | RES | RES | VID4 | VID3 | VID2 | VID1 | VID0 |
//    7     6     5      4      3       2       1      0
#define REG_VID 0x43
/////////////////////////////////////////////////////////////////
// VOLTAGE LIMIT REGISTERS
#define REG_25V_LOW_LIMIT 0x44
#define REG_25V_HIGH_LIMIT 0x45
#define REG_VCCP_LOW_LIMIT 0x46
#define REG_VCCP_HIGH_LIMIT 0x47
#define REG_33V_LOW_LIMIT 0x48
#define REG_33V_HIGH_LIMIT 0x49
#define REG_5V_LOW_LIMIT 0x4A
#define REG_5V_HIGH_LIMIT 0x4B
#define REG_12V_LOW_LIMIT 0x4C
#define REG_12V_HIGH_LIMIT 0x4D
/////////////////////////////////////////////////////////////////
// TEMPERATURE LIMIT REGISTERS
#define REG_PROCESS_ZONE1_LOW_TEMP 0x4E
#define REG_PROCESS_ZONE1_HIGH_TEMP 0x4F
#define REG_INTERNAL_ZONE2_LOW_TEMP 0x50
#define REG_INTERNAL_ZONE2_HIGH_TEMP 0x51
#define REG_REMOTE_ZONE3_LOW_TEMP 0x52
#define REG_REMOTE_ZONE3_HIGH_TEMP 0x53
/////////////////////////////////////////////////////////////////
// FAN TACHOMETER LOW LIMIT REGISTERS
#define REG_TACH1_MINIMUM_LSB 0x54
#define REG_TACH1_MINIMNUM_MSB 0x55
#define REG_TACH2_MINIMUM_LSB 0x56
#define REG_TACH2_MINIMUM_MSB 0x57
#define REG_TACH3_MINIMUM_LSB 0x58
#define REG_TACH3_MINIMUM_MSB 0x59
#define REG_TACH4_MINIMUM_LSB 0x5A
#define REG_TACH4_MINIMUM_MSB 0x5B
/////////////////////////////////////////////////////////////////
// FAN TACHOMETER CONFIGURATION REGISTERS
// | ZON2 | ZON1 | ZON0 | INV | RES | SPIN2 | SPIN1 | SPIN0 |
//     7      6      5     4     3      2       1       0
#define REG_FAN1_CONF 0x5C
#define REG_FAN2_CONF 0x5D
#define REG_FAN3_CONF 0x5E
/////////////////////////////////////////////////////////////////
// AUTO FAN SPEED RANGE, PWM FREQUENCY REGISTERS
// FAN TACHOMETER CONFIGURATION REGISTERS
// | RAN3 | RAN2 | RAN1 | RAN0 | HLFRQ | FRQ2 | FRQ1 | FRQ0 |
//     7      6      5      4      3       2      1      0
#define REG_FAN1_RANGE_FREQ 0x5F
#define REG_FAN2_RANGE_FREQ 0x60
#define REG_FAN3_RANGE_FREQ 0x61
/////////////////////////////////////////////////////////////////
// MIN/OFF, SPIKE SMOOTHING REGISTERS
// | OFF3 | OFF2 | OFF1 | RES | ZN1E | ZN1-2 | ZN1-1 | ZN1-0 |
//     7      6      5     4      3      2       1       0
#define REG_MIN_OFF_ZONE1_SPIKE_SMOOTH 0x62
#define REG_ZONE2_ZONE3_SPIKE_SMOOTH 0x63
/////////////////////////////////////////////////////////////////
// MINIMUM PWM DUTY CYCLE REGISTERS
#define REG_FAN1_PWM_MIN 0x64
#define REG_FAN2_PWM_MIN 0x65
#define REG_FAN3_PWM_MIN 0x66
/////////////////////////////////////////////////////////////////
// TEMPERATURE LIMIT REGISTERS
#define REG_ZONE1_FAN_TEMP_LIMIT 0x67
#define REG_ZONE2_FAN_TEMP_LIMIT 0x68
#define REG_ZONE3_FAN_TEMP_LIMIT 0x69
/////////////////////////////////////////////////////////////////
// ABSOLUTE TEMPERATURE LIMIT REGISTERS
#define REG_ZONE1_TEMP_ABSOLUTE_LIMIT 0x6A
#define REG_ZONE2_TEMP_ABSOLUTE_LIMIT 0x6B
#define REG_ZONE3_TEMP_ABSOLUTE_LIMIT 0x6C
/////////////////////////////////////////////////////////////////
// ZONE HYSTERESIS REGISTERS
// | H1-3 | H1-2 | H1-1 | H1-0 | H2-3 | H2-2 | H2-1 | H2-0 |
//     7      6      5      4      3      2      1       0
//-------------------------------------------------------------//
// | H3-3 | H3-2 | H3-1 | H3-0 | RES | RES | RES | RES |
//     7      6      5      4     3     2     1     0
#define REG_ZONE1_ZONE2_HYSTERESIS 0x6D
#define REG_ZONE3_HYSTERESIS 0x6E
/////////////////////////////////////////////////////////////////
// TEST REGISTERS
// | RES | RES | RES | RES | RES | RES | RES | XEN |
//    7     6     5     4     3     2     1     0
#define REG_XOR_TEST_TREE_ENABLE 0x6F
/////////////////////////////////////////////////////////////////
// TACHOMETER MONITOR MODE REGISTERS
// | RES | RES | T3/4-1 | T3/4-0 | T2-1 | T2-0 | T1-1 | T1-0 |
//    7     6       5        4       3      2      1     0
#define REG_TACH_MONITOR_MODE 0x74
/////////////////////////////////////////////////////////////////
// TACHOMETER MONITOR MODE REGISTERS
#define REG_FAN_SPINUP_MODE 0x75
/////////////////////////////////////////////////////////////////
void I2C_INIT(void);
/////////////////////////////////////////////////////////////////
uint8_t I2C_COMAND(uint8_t action);
/////////////////////////////////////////////////////////////////
uint8_t READ_BYTE(uint8_t COMMAND);
/////////////////////////////////////////////////////////////////
void WRITE_BYTE(uint8_t COMMAND, uint8_t data);
/////////////////////////////////////////////////////////////////
void LM96000_CHECK(void);
/////////////////////////////////////////////////////////////////
int Set_THIGH(int temp);
/////////////////////////////////////////////////////////////////
int Set_TLOW(int temp);
/////////////////////////////////////////////////////////////////
int TEMP_CONVERSION(int temp);
/////////////////////////////////////////////////////////////////
int TEMP_RECONVERSION(int temp);
/////////////////////////////////////////////////////////////////
void Problem(uint8_t problem, uint8_t data);

#endif