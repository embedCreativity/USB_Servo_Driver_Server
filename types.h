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

#endif
