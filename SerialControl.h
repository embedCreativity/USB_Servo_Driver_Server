/************************************************************************/
/*                                                                      */
/*  SerialControl.h --  Serial Comm helper routines header file         */
/*                                                                      */
/************************************************************************/
/*  Author:     Mark Taylor                                             */
/*  Copyright 2009, Digilent, Inc.                                      */
/************************************************************************/
/*  File Description:                                                   */
/*                                                                      */
/*  This header file contains declarations the functions contained in   */
/*  SerialControl.c                                                     */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  02/02/2009(MarkT): created                                          */
/*                                                                      */
/************************************************************************/

#if !defined(_SERIALCONTROL_H)
#define _SERIALCONTOL_H

#include <stdint.h>  /* for unsigned values*/
#include <stdlib.h>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <stdbool.h>

/* ------------------------------------------------------------ */
/*                  General Type Declarations                   */
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*                  Object Class Declarations                   */
/* ------------------------------------------------------------ */



/* ------------------------------------------------------------ */
/*                  Variable Declarations                       */
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*                  Function Prototypes                         */
/* ------------------------------------------------------------ */

int     SerialWriteNBytes(uint8_t *rgbChars, int n);
int     SerialWriteByte(uint8_t *pByte);
int     SerialRead(uint8_t *result);
int     SerialGetBaud(void);
bool    SerialInit(char *szDevice);
void    SerialClose(void);

/* ------------------------------------------------------------ */

#endif

/**********************************  EOF  **************************************/





