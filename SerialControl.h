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

/* ------------------------------------------------------------ */
/*                  General Type Declarations                   */
/* ------------------------------------------------------------ */

// indicates we need to call TS specific system calls instead of Unix 
// kernel calls.
#define USE_XUART

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
int     SerialRead(char *result);
int     SerialGetBaud(void);
int     SerialInit(char *szDevice);
void    SerialClose(void);

/* ------------------------------------------------------------ */

#endif

/**********************************  EOF  **************************************/





