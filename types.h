/********************************************************************/
/*                                                                  */
/********************************************************************/

#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

#define SOCK_BUF_SIZE   1500
#define MAX_SOCKET_MSG_LEN  SOCK_BUF_SIZE
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

/****************************************************
*   Types                                           *
****************************************************/
typedef struct _palmettoStatus_T {
    uint8_t status; //
    float voltage;
    float current;
    bool commFault;
} palmettoStatus_T;




class ControlData {
public:

    // overload comparison operators
    void operator= (const ControlData &other);
    bool operator== (const ControlData &other);
    bool operator!= (const ControlData &other);

    // What kind of messages are required of a detected change?
    bool diffTypePosition(const ControlData *other);
    bool diffTypePower(const ControlData *other);

    uint32_t motorA;
    uint32_t motorB;
    uint32_t motorC;
    uint32_t motorD;
    uint32_t servo1;
    uint32_t servo2;
    uint32_t servo3;
    uint32_t servo4;
    uint32_t servo5;
    uint32_t servo6;
    uint32_t servo7;
    uint32_t servo8;
    uint32_t extLed;
    bool motorPower;
    bool hostPower;
};

#endif
