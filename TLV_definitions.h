/***************************************************************
* This is a common file, used in the TI project as well projects
*   communicating with the USB_Servo_Driver board
****************************************************************/

#ifndef __TLV_DEFINITIONS_H__
#define __TLV_DEFINITIONS_H__

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
#define POS_CHAN5           26
#define POS_CHAN6           29
#define POS_CHAN7           32
#define POS_CHAN8           35
#define POS_EXT_LED         38
//#define POS_CHECKSUM        41 -> the 40th byte after type and length fields (data bytes)

#define TYPE_PWR_UPDATE     0xBB
#define LENGTH_PWR_UPDATE   1
#define POS_BF_PWR          2 // uint8_t bit field
#define BS_PWR_M            0 // lsb
#define BS_PWR_C            1 // bits 2-7 are unused

// Update position data Ack
#define TYPE_ACK        0xCC
#define LENGTH_ACK      5

/************************************/
/*   typedefs                       */
/************************************/

typedef struct _tlvAck_T {
    uint8_t     type;
    uint8_t     length;
    uint8_t     status; // bit field (TBD)
    uint16_t    adcVoltage; // 12-bit ADC result
    uint16_t    adcCurrent; // 12-bit ADC result
    uint8_t     checksum;
} __attribute__ ((__packed__)) tlvAck_T;

#endif

