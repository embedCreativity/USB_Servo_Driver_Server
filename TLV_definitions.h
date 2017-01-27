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
#define TYPE_LOC_UPDATE     0xAA // position: 0
#define LENGTH_LOC_UPDATE   39   // 1
#define POS_EN_A            2    // 2,3,4    -> count 1,2,3
#define POS_EN_B            5    // 5,6,7    -> 4,5,6
#define POS_EN_C            8    // 8-10     -> 7,8,9
#define POS_EN_D            11   // 11-13    -> 10
#define POS_CHAN1           14   // 14-16    -> 13
#define POS_CHAN2           17   // 17       -> 16
#define POS_CHAN3           20   // 20       -> 19
#define POS_CHAN4           23   // 23       -> 22
#define POS_CHAN5           26   // 26       -> 25
#define POS_CHAN6           29   // 29       -> 28
#define POS_CHAN7           32   // 32       -> 31
#define POS_CHAN8           35   // 35,36,37 -> 34
#define POS_EXT_LED         38   // 38,39,40 -> 37,38,39
//#define POS_CHECKSUM        41   // 41       -> 40

// Update position data Ack
#define TYPE_UPDATE_ACK      0xBB
#define LENGTH_UPDATE_ACK    0

#endif

