/************************************************************************/
/*                                                                      */
/*  serialthread.h  --  Header file for serial thread routines          */
/*                                                                      */
/************************************************************************/
/*  Author:     Mark Taylor                                             */
/*  Copyright 2009, Mark Taylor                                         */
/************************************************************************/
/*  Module Description:                                                 */
/*                                                                      */
/*  The serialthread routines are executed as part of a seperate thread */
/*  that executes concurrently along side the main thread that is part  */
/*  of AutomatedSecurity.c.  This thread waits for incoming data on the */
/*  serial port and notifies the main process when data has arrived.    */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  04/17/2009 (MarkT): created                                         */
/*                                                                      */
/************************************************************************/

#ifndef _SERIALTHREAD_H //multiple inclusion protection
#define _SERIALTHREAD_H

/* ------------------------------------------------------------ */
/*              Include File Definitions                        */
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdint.h> /* for unsigned values */
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h> /* POSIX terminal control definitions */
#include <unistd.h>
#include <time.h>
#include <pthread.h> //peripheral thread library; must compile with -lpthread option

/* ------------------------------------------------------------ */
/*              Local Type Definitions                          */
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*              Global Variables                                */
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*              Local Variables                                 */
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*              Forward Declarations                            */
/* ------------------------------------------------------------ */

uint8_t     SpawnSerialThread(void);
void    *SerialThread(void *threadid);

#endif //end multiple inclusion protection
/**************************** EOF *******************************/













