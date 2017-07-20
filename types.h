/********************************************************************/
/*                                                                  */
/********************************************************************/

#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

/****************************************************
*   Types                                           *
****************************************************/
typedef struct _palmettoStatus_T {
    uint8_t status; //
    float voltage;
    float current;
    bool commFault;
} palmettoStatus_T;

typedef struct _controlData_T {
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
} controlData_T;

#endif
