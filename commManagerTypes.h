/********************************************************************/
/*                                                                  */
/********************************************************************/

#ifndef __COMM_MANAGER_TYPES_H__
#define __COMM_MANAGER_TYPES_H__


/****************************************************
*   Types                                           *
****************************************************/

#define MAX_SERIAL_ATTEMPTS     3
#define SERIAL_RETRY_DELAY      1000 // 1 ms (in microseconds)
#define SERIAL_READ_DELAY       1000 // 1 ms (in microseconds)
#define SERIAL_TIMEOUT          100 // 100 ms
#define INTERCOMMAND_REST       25000 // 25 ms

#define SHUTDOWN_DELAY          30 // Palmetto will wait 30 seconds before pulling power on us

// TLV-related defs
#define TLV_OVERHEAD    3 // type, length, checksum fields
#define POS_TLV_TYPE    0
#define POS_TLV_LENGTH  1

#define TYPE_LOC_UPDATE     0xAA
#define LENGTH_LOC_UPDATE   39

#define TYPE_PWR_UPDATE     0xBB
#define LENGTH_PWR_UPDATE   1

#define TYPE_HEARTBEAT      0xDD
#define LENGTH_HEARTBEAT    0

#define TYPE_SHUTDOWN       0xEE
#define LENGTH_SHUTDOWN     1

#define TYPE_ACK            0xCC
#define LENGTH_ACK          5

// Board API-related fields
#define MOTOR_REFRESH_PERIOD    4000
#define MOTOR_MIN_SPIN          2000
#define API_MOTORPOWER_SCALAR   2
#define API_SERVOPOS_SCALAR     80
#define SERVO_MIN_PERIOD        60000
#define EXTLED_REFRESH_PERIOD   400000
#define API_LEDPOWER_MAX        1000
#define API_LEDPOWER_SCALAR     (EXTLED_REFRESH_PERIOD/API_LEDPOWER_MAX)

// bit-field defs
#define BS_PWR_M        0 // lsb
#define BS_PWR_C        1 // bits 2-7 are unused

typedef struct _tlvPosition_T {
    uint8_t type;
    uint8_t length;
    uint8_t motorA[3];
    uint8_t motorB[3];
    uint8_t motorC[3];
    uint8_t motorD[3];
    uint8_t servo1[3];
    uint8_t servo2[3];
    uint8_t servo3[3];
    uint8_t servo4[3];
    uint8_t servo5[3];
    uint8_t servo6[3];
    uint8_t servo7[3];
    uint8_t servo8[3];
    uint8_t extLed[3];
    uint8_t checksum;
} __attribute__ ((__packed__)) tlvPosition_T;

typedef struct _tlvPowerManagement_T {
    uint8_t type;
    uint8_t length;
    uint8_t config;
    uint8_t checksum;
} __attribute__ ((__packed__)) tlvPowerManagement_T;

typedef struct _tlvHeartBeat_T {
    uint8_t type;
    uint8_t length;
    uint8_t checksum;
} __attribute__ ((__packed__)) tlvHeartBeat_T;

typedef struct _tlvShutDown_T {
    uint8_t type;
    uint8_t length;
    uint8_t delay;
    uint8_t checksum;
} __attribute__ ((__packed__)) tlvShutDown_T;

typedef struct _tlvAck_T {
    uint8_t     type;
    uint8_t     length;
    uint8_t     status; // bit field (TBD)
    uint16_t    adcVoltage; // 12-bit ADC result
    uint16_t    adcCurrent; // 12-bit ADC result
    uint8_t     checksum;
} __attribute__ ((__packed__)) tlvAck_T;

#endif

