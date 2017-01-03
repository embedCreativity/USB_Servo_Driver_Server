
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

//boolean definitions
#define fFalse          0
#define fTrue           !fFalse

#define MAXPENDING      1    /* Max connection requests */
#define BUFFSIZE        1500

#define LEN_SERIAL_PORT 32

#define DAEMON_NAME "serialServer"

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

// 20KHz (outside audible?)
#define MOTOR_REFRESH_PERIOD        4000
//#define MOTOR_REFRESH_PERIOD        8000

// 50Hz flicker
#define EXTLED_REFRESH_PERIOD       1600000

// Delay between writes             100ms
#define SERIAL_SLEEP_PERIOD         100000

// Delay between writing to serial port and subsequently calling read
#define SERIAL_READ_DELAY           1000

#define RESPONSE_LENGTH             2 // TODO: fix


#define POS_TLV_TYPE  0
#define POS_TLV_LENGTH  1
#define POS_TLV_BEGIN_DATA 2
#define TLV_OVERHEAD 3

// All position definitions start at 2.  0 is type, 1 is length, 2 is start of values thereafter.  Checksum is last
// Position of checksum is always (LENGTH_* + 2)

/****** OUTGOING DATA - FROM DEVICE TO PC ******/
//   Local UPDATE position data
//     There are 13 PWM values that need to be updated, each being 24-bits
#define TYPE_LOC_UPDATE     0xAA
#define LENGTH_LOC_UPDATE   39
#define POS_EN_A            2
#define POS_EN_B            5
#define POS_EN_C            8
#define POS_EN_D            11
#define POS_CHAN1           14
#define POS_CHAN2           17
#define POS_CHAN3           20
#define POS_CHAN4           23
#define POS_CHAN5           25
#define POS_CHAN6           28
#define POS_CHAN7           31
#define POS_CHAN8           34
#define POS_EXT_LED         37

// Update position data Ack
#define TYPE_UPDATE_ACK      0xBB
#define LENGTH_UPDATE_ACK    0

// Default tlv values
#define DFLT_MOTOR          (MOTOR_REFRESH_PERIOD-1)
#define DFLT_SERVO          (SERVO_REFRESH_PERIOD - SERVO_MID_PERIOD)
#define DFLT_EXT_LED    1

typedef struct _tlvLocUpdateDefaults_T {
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
} __attribute__ ((__packed__)) tlvLocUpdateDefaults_T;

// This data structure gets written to a file and read upon starting
// It holds all the default values for the board
typedef struct _savedDefaults_T {
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


void*   Webcam(void *arg);
void*   BoardComms(void *arg);
void    HandleClient( void );
void    InterpretSocketCommand(uint8_t *data, uint32_t length);
uint8_t ComputeChecksum(uint8_t *input, uint32_t length);

#endif

