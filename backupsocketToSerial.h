
#ifndef __SOCKET_TO_SERIAL_H__
#define __SOCKET_TO_SERIAL_H__

/************************************/
/*   macros                         */
/************************************/

/************************************/
/*   typedefs                       */
/************************************/

/************************************/
/*   definitions                    */
/************************************/

/************************************ TLVs ***************************************************
 *
 *      ________                                     ____________
 *     |   PC  |  < - - (USB) - - - - - - - >      | This board |
 *      --------                                     ------------
 *      (foo) ----------------------->
 *                        <------------------------- (bar)
 *
 *
 **********************************************************************************************/

// Socket MSG API definitions
#define API_SERVO_MIN 1
#define API_SERVO_MAX 8
#define API_SERVOPOS_MIN    0
#define API_SERVOPOS_MAX    1500
#define API_MOTOR_MIN       1
#define API_MOTOR_MAX       4
#define API_MOTORPOWER_MIN  -1000
#define API_MOTORPOWER_MAX  1000
#define API_LEDPOWER_MIN    0
#define API_LEDPOWER_MAX    1000
#define MAX_SOCKET_MSG_LEN  64

// Socket definitions
#define MAXPENDING      1    /* Max connection requests */
#define SOCK_BUF_SIZE   1500

#define LEN_SERIAL_PORT 32

// Daemon definitions
#define DAEMON_NAME "serialServer"
#define DEFAULT_FILE "/usr/local/etc/defaults.bin"

// From TivaPWM.h
// 20ms (50Hz) = SERVO_REFRESH_PERIOD clocks at a system clock rate of 80MHz
#define SERVO_REFRESH_PERIOD        1600000
// PERIOD = fractional second * 80M clocks per second
// min = 750uS * 80M
#define SERVO_MIN_PERIOD            60000
// mid = 1.5ms * 80M
#define SERVO_MID_PERIOD            120000
// max = 2.25ms * 80M
#define SERVO_MAX_PERIOD            180000

// SERVO_MAX_PERIOD - SERVO_MIN_PERIOD) / API_SERVOPOS_MAX = 80
#define API_SERVOPOS_SCALAR         80

// 20KHz (outside audible?)
#define MOTOR_REFRESH_PERIOD        4000
// Trial-and-error testing shows this is the minimum to
//  get the motor to spin: 1720 -> for easy math, we're
//  going to make this 2000
#define MOTOR_MIN_SPIN              2000
// MOTOR_REFRESH_PERIOD - MOTOR_MIN_SPIN = 2000
#define MOTOR_POWER_RANGE           2000

// MOTOR_POWER_RANGE / API_MOTORPOWER_MAX = 2
#define API_MOTORPOWER_SCALAR       2

// 50Hz flicker
// #define EXTLED_REFRESH_PERIOD       1600000
// 200Hz flicker
#define EXTLED_REFRESH_PERIOD       400000

// EXTLED_REFRESH_PERIOD / API_LEDPOWER_MAX = 1600000/1000 = 1600
#define API_LEDPOWER_SCALAR         (EXTLED_REFRESH_PERIOD/API_LEDPOWER_MAX)

// Delay between writes             100ms
#define SERIAL_SLEEP_PERIOD         100000

// Delay between writing to serial port and subsequently calling read
#define SERIAL_READ_DELAY           1000

// Delay between resetting position and turning off power
#define RESET_DELAY                 2000000

#define RESPONSE_LENGTH             2 // TODO: fix

// Serial comm timeout
#define SERIAL_TIMEOUT              100 // milliseconds

// Default tlv values
#define DFLT_MOTOR          (MOTOR_REFRESH_PERIOD-1)
#define DFLT_SERVO          (SERVO_REFRESH_PERIOD - SERVO_MID_PERIOD)
#define DFLT_EXT_LED    1

typedef enum TLV_TYPE {
    TLV_POSITION = 0,
    TLV_POWER,
    TLV_ERROR
} TLV_TYPE;

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

// This data structure gets written to a file and read upon starting
// It holds all the default values for the board
typedef struct _savedDefaults_T {
    uint8_t motorADefault[3];
    uint8_t motorBDefault[3];
    uint8_t motorCDefault[3];
    uint8_t motorDDefault[3];
    uint8_t servo1Default[3];
    uint8_t servo2Default[3];
    uint8_t servo3Default[3];
    uint8_t servo4Default[3];
    uint8_t servo5Default[3];
    uint8_t servo6Default[3];
    uint8_t servo7Default[3];
    uint8_t servo8Default[3];
    uint8_t extLedDefault[3];
    uint8_t motorAGo[3];
    uint8_t motorBGo[3];
    uint8_t motorCGo[3];
    uint8_t motorDGo[3];
    uint8_t motorABack[3];
    uint8_t motorBBack[3];
    uint8_t motorCBack[3];
    uint8_t motorDBack[3];
    uint8_t motorAStop[3];
    uint8_t motorBStop[3];
    uint8_t motorCStop[3];
    uint8_t motorDStop[3];
    uint8_t motorAPivotRight[3];
    uint8_t motorBPivotRight[3];
    uint8_t motorCPivotRight[3];
    uint8_t motorDPivotRight[3];
    uint8_t motorAPivotLeft[3];
    uint8_t motorBPivotLeft[3];
    uint8_t motorCPivotLeft[3];
    uint8_t motorDPivotLeft[3];
    uint8_t motorATurnRight[3];
    uint8_t motorBTurnRight[3];
    uint8_t motorCTurnRight[3];
    uint8_t motorDTurnRight[3];
    uint8_t motorATurnLeft[3];
    uint8_t motorBTurnLeft[3];
    uint8_t motorCTurnLeft[3];
    uint8_t motorDTurnLeft[3];
} __attribute__ ((__packed__)) savedDefaults_T;

typedef struct _serialRx_T {
    int     len; // must be able to support error codes
    uint8_t data[64];
} serialRx_T;

// Function prototypes
void        SetDefaults (void);
bool        LoadDefaults (void);
bool        SaveDefaults (void);
void*       Webcam(void *arg);
bool        BoardComm( TLV_TYPE type );
void        HandleClient( void );
TLV_TYPE    InterpretSocketCommand(uint8_t *data, uint32_t length);
bool        SerialGetResponse( uint32_t timeout );
void        SetServo (uint8_t servo, uint32_t position);
void        SetMotor (uint8_t motor, int32_t power);
void        SetLED (uint32_t power);
uint8_t     ComputeChecksum(uint8_t *input, uint32_t length);

#endif

