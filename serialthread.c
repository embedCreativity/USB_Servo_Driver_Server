/************************************************************************/
/*                                                                      */
/*  serialthread.c  --  Serial communication thread module              */
/*                                                                      */
/************************************************************************/
/*  Author:     Mark Taylor                                             */
/*  Copyright 2009, Mark Taylor                                         */
/************************************************************************/
/*  Module Description:                                                 */
/*                                                                      */
/*  This module was written to spawn a seperate thread to monitor the   */
/*  serial port allowing the main body of code to continue doing other  */
/*  tasks.  The main body of code must periodically check the global    */
/*  variable fSerialCommDataReady to see if there is any data ready, but*/
/*  other than that, the main code does not need to do anything else.   */
/*                                                                      */
/*  This component relies on the tools in SerialControl.h / c           */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  04/17/2009 (MarkT): created                                         */
/*                                                                      */
/************************************************************************/

/* ------------------------------------------------------------ */
/*              Include File Definitions                        */
/* ------------------------------------------------------------ */

//seperate thread that handles incoming serial communications
#include "serialthread.h"
#include "SerialControl.h" /* my Serial comm functions */

/* ------------------------------------------------------------ */
/*              Local Type Definitions                          */
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*              Global Variables                                */
/* ------------------------------------------------------------ */

pthread_t threadSerialComm; //handle to the thread

long    lThreadID;
char    szCommRx[256]; //Received data buffer, accessible everywhere
char    fSerialCommDataReady; //flag indicating ready data

/* ------------------------------------------------------------ */
/*              Local Variables                                 */
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*              Forward Declarations                            */
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*              Procedure Definitions                           */
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/***    SpawnSerialThread
**
**  Synopsis:
**      bool    SpawnSerialThread(void);
**
**  Parameters:
**      none
**
**  Return Values:
**      boolean success
**
**  Errors:
**      none
**
**  Description:
**      Spawns a seperate process to handle serial communication.  This
**      thread will init the interface and start listening to the port.
*/
uint8_t SpawnSerialThread(void) {

    lThreadID = 0; //init global
    fSerialCommDataReady = 0; //init to false
    
    if (pthread_create(&threadSerialComm, NULL, SerialThread, (void *)lThreadID)) {
        printf("ERROR;  pthread_create(&threadSerialComm... \n");
        return 0;
    } //end if
    lThreadID += 1; //increment thread count
    
    return 1;
}

/* ------------------------------------------------------------ */
/***    SerialThread
**
**  Synopsis:
**      void    SerialThread(void *threadid);
**
**  Parameters:
**      void *threadid
**
**  Return Values:
**      none
**
**  Errors:
**      none
**
**  Description:
**      Initializes serial interface and waits on data to arrive.  Places
**      data in globally accessible buffer and notifies main code of
**      reception.
*/
void *SerialThread(void *threadid) {
    
    /* We will now open our serial connection to the target device. */
    if( ! SerialInit("/dev/ttyS0") ) {
        printf("Error...  Serial Initialization Failed.  Abort now.\n");
        pthread_exit(NULL);
    }
    
    printf("Serial comm opened with baud of %d\n", SerialGetBaud());
    
    
    while(1) {
        printf("Sending 'Hello World' now\n"); //debug
        SerialWriteNBytes("Hello World!", strlen("Hello World!"));
/*      printf("Getting ready to read serial port in SerialThread()...\n"); //debug
        SerialRead(&szCommRx[0]); //go get data
        fSerialCommDataReady = 1; //indicate data is ready
        printf("%s <-- from Serial Connection\n",szCommRx); //debug echo...         */
        usleep(500000); //sleep for 0.5 seconds to return CPU usage to system
    }
}








/**************************** EOF *****************************/
