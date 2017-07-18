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
} controlData_T;

#endif
