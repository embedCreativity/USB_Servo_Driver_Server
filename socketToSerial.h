
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

void*   Webcam(void *arg);
void*   BoardComms(void *arg);
void    HandleClient( void );
void    InterpretSocketCommand(uint8_t *data, uint32_t length);
uint8_t ComputeChecksum(uint8_t *input, uint32_t length);

#endif

