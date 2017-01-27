/************************************************************************/
/*                                                                      */
/*  socketToSerial.c    --  Main program module                         */
/*                                                                      */
/************************************************************************/
/*  Author:     Mark Taylor                                             */
/*  Copyright 2016, Mark Taylor                                         */
/************************************************************************/
/*  Module Description:                                                 */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  09/16/2009 (MarkT): created                                         */
/*  03/09/2010 (MarkT): Resumed development on project, moved official  */
/*                      copy to server                                  */
/*  10/10/2016 (MarkT): Finally actually working on this                */
/*                                                                      */
/************************************************************************/

//Debug switch
#define  DEBUG

//compilation directives
#define VERBOSE
//#define WEBCAM

#define _GNU_SOURCE

/* ------------------------------------------------------------ */
/*              Include File Definitions                        */
/* ------------------------------------------------------------ */
//#include <sys/types.h>
#include <sys/stat.h> // umask()
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <socket.h> // My installed library

#include <pthread.h> //peripheral thread library; must compile with -lpthread option

//seperate thread that handles incoming serial communications
#include "SerialControl.h" /* my Serial comm functions */
#include "socketToSerial.h"

/* ------------------------------------------------------------ */
/*              Local Type Definitions                          */
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*              Global Variables                                */
/* ------------------------------------------------------------ */

//--- TODO: protect all instances with pthread mutexes
// This is what we send to the board
tlvLocUpdates_T locUpdates;

// structure of saved default values
savedDefaults_T locDefaults;

// struct for holding data,len returned from SerialRead
serialRx_T serialRxData;

// handles to the threads
pthread_t threadWebcam;

char serialPort[LEN_SERIAL_PORT];
int portNum;
int baudRate;
extern SocketInterface_T socketIntf;

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
/***    signal_handler
**
**  Synopsis:
**      void signal_handler(int sig)
**
**  Parameters:
**      optional command line arguments
**
**  Return Values:
**      void
**
**  Errors:
**      none
**
**  Description:
**      This function handles select signals that the daemon may
**      receive.  This gives the daemon a chance to properly shut
**      down in emergency situations.  This function is installed
**      as a signal handler in the 'main()' function.
*/
void signal_handler(int sig)
{
    switch(sig)
    {
        case SIGHUP:
        case SIGTERM:
        case SIGINT:
        case SIGQUIT:
            // Wait for threads to return
        #if defined(WEBCAM)
            pthread_join(threadWebcam, NULL);
        #endif

            syslog(LOG_WARNING, "Received SIGHUP signal.");
            socketIntf.Close(); // close connection to client
            socketIntf.Close(); // close socket
            exit(1);
            break;
        default:
            syslog(LOG_WARNING, "Unhandled signal (%d) %s", sig, strsignal(sig));
            break;
    }
}

uint8_t ComputeChecksum(uint8_t *input, uint32_t length)
{
    int i;
    uint8_t checksum;
    checksum = 0;

    for ( i = 0; i < length; i++ ) {
        checksum = checksum ^ input[i];
    }

    return checksum;
}

/* ------------------------------------------------------------ */
/***    main
**
**  Synopsis:
**      int main(int argc, char *argv[])
**
**  Parameters:
**      optional command line arguments
**
**  Return Values:
**      standard return values
**
**  Errors:
**      none
**
**  Description:
**      Main program module.
*/
int main(int argc, char *argv[])
{

    if (argc != 4) {
        fprintf(stderr, "USAGE: %s <path to serial port> <baud Rate> <socket port number>\n", argv[0]);
        exit(1);
    }
    strncpy(serialPort, (char*)argv[1], LEN_SERIAL_PORT);
    baudRate = atoi(argv[2]);
    portNum = atoi(argv[3]);

#if defined(DEBUG)
    int daemonize = 0;
#else
    int daemonize = 1;
#endif

    // Setup signal handling
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    // Setup syslog logging - see SETLOGMASK(3)
#if defined(DEBUG)
    setlogmask(LOG_UPTO(LOG_DEBUG));
    openlog(DAEMON_NAME, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);
#else
    setlogmask(LOG_UPTO(LOG_INFO));
    openlog(DAEMON_NAME, LOG_CONS, LOG_USER);
#endif

    syslog(LOG_INFO, "%s daemon starting up", DAEMON_NAME);

    /* Our process ID and Session ID */
    pid_t pid, sid;

    if (daemonize) {
        syslog(LOG_INFO, "starting the daemonizing process");

        /* Fork off the parent process */
        pid = fork();
        if (pid < 0) {
            exit(EXIT_FAILURE);
        }
        /* If we got a good PID, then
        we can exit the parent process. */
        if (pid > 0) {
            exit(EXIT_SUCCESS);
        }

        /* Change the file mode mask */
        umask(0);

        /* Create a new SID for the child process */
        sid = setsid();
        if (sid < 0) {
            /* Log the failure */
            exit(EXIT_FAILURE);
        }

        /* Change the current working directory */
        if ((chdir("/")) < 0) {
            /* Log the failure */
            exit(EXIT_FAILURE);
        }

        /* Close out the standard file descriptors */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }

    // Open port and wait for client to connect
    syslog(LOG_INFO,"Attempting to connect on port %d...\n", portNum);
    if ( socketIntf.OpenAndConnect(portNum) ) {
        syslog(LOG_INFO, "Connected!");
    } else {
        syslog(LOG_INFO, "Failed to connect to socket - exit");
        return 0;
    }

    locUpdates.type = TYPE_LOC_UPDATE;
    locUpdates.length = LENGTH_LOC_UPDATE;

    /* Run until cancelled */
    while (true) {
        syslog(LOG_INFO, "Client connected.");
        HandleClient();
        syslog(LOG_INFO, "Client disconnected.");

        // Re-open port and wait for client to connect
        syslog(LOG_INFO,"Attempting to connect on port %d...\n", portNum);
        if ( socketIntf.OpenAndConnect(portNum) ) {
            syslog(LOG_INFO, "Connected!");
        } else {
            syslog(LOG_INFO, "Failed to connect to socket - exit");
            return 0;
        }
    }

    /* Prepare to exit the daemon process.  Free any resources before exit. */
    syslog(LOG_INFO, "%s daemon exiting", DAEMON_NAME);
    return 0;
} //end main()

/* ------------------------------------------------------------ */
/***    HandleClient
**
**  Synopsis:
**      void    HandleClient()
**
**  Parameters:
**      none
**
**  Return Values:
**      none
**
**  Errors:
**      none
**
**  Description:
**      Directs control of application based on TCP traffic content.
**
*/
void HandleClient( void )
{
    uint8_t socketRx[BUFFSIZE];
    int32_t cntSocketRx;

    #if defined(WEBCAM)
    // Start webcam thread
    if (pthread_create(&threadWebcam, NULL, &Webcam, NULL)) {
        syslog(LOG_PERROR, "ERROR;  pthread_create(&threadWebcam... \n");
    } // end if
    #endif

    /****************************************/
    /* Prep Serial Handling                 */
    if ( false == LoadDefaults() ) {
        syslog(LOG_PERROR, "ERROR: Could not load defaults from: %s\n", DEFAULT_FILE);
        return;
    }
    SetDefaults();

    // Bring up the serial port
    if ( ! SerialInit(serialPort, baudRate) ) {
#if defined(DEBUG)
        fprintf(stdout, "ERROR: Couldn't open %s.\n", serialPort);
#else
        syslog(LOG_PERROR, "ERROR: Couldn't open %s.\n", serialPort);
#endif
        return;
    }
    /****************************************/

    /**************** Main TCP linked section **************************/
    while(true) {
        cntSocketRx = socketIntf.Read(socketRx, BUFFSIZE);
        if ( cntSocketRx > 0 ) {
            InterpretSocketCommand(socketRx, cntSocketRx);
            BoardComm(); // transmit the packet to the board
            // Push serial response back to client over the socket
            socketIntf.Write(serialRxData.data, serialRxData.len);
        } else {
            // restore defaults
            SetDefaults();
            syslog(LOG_INFO, "Client disconnected - resetting board\n");
            BoardComm(); // restore back to defaults
            socketIntf.Close(); // close client connection
            socketIntf.Close(); // close socket // may not be the best idea, ok for now
            SerialClose();
            break;
        }
    }// end while
    /********************************************************************/
} //end HandleClient()

void BoardComm ( void )
{
    // Send the board the data that we've been updating with interpreted socket data
    SerialWriteNBytes((uint8_t*)&locUpdates, sizeof(tlvLocUpdates_T));

    // Give board a moment to respond
    usleep(SERIAL_READ_DELAY);

    // Get response from serial ( should be ADC battery voltage reading )
    serialRxData.len = SerialRead((uint8_t*)(&(serialRxData.data)));
}

void InterpretSocketCommand(uint8_t *data, uint32_t length)
{
    char *strInput;

    // sanity check
    if ( data == NULL || length > 64 ) {
        syslog(LOG_WARNING, "InterpretSocketCommand: failed input sanity check");
        if (data == NULL) {
            syslog(LOG_WARNING, "ISC()-->data = NULL, length: %d", length);
        } else {
            syslog(LOG_WARNING, "ISC()-->length = %d", length);
        }
        return;
    }
    // preserve original string and null terminate
    strInput = (char*)malloc(length+1);
    strncpy(strInput, (char*)data, length);
    strInput[length] = 0;


    syslog(LOG_INFO, "SocketRx: %s", strInput);

    // MSB first to LSB as we increment pointer in buffer. 24-bits each value
    //tlvLocUpdate[POS_EN_A] = foo;

    // start string parsing from here:
    // candidate command strings:
    //  Go, stop, back, pivot right, pivot left, turn right, turn left
    //  set servo[1,2,3,4,5,6,7,8] ####
    //  set motor[A,B,C,D] ####
    // It'd be cool to chain these together so they arrive all at the same time
    //  set motorA 500; set motorB 500;

    // look for 'set'
    if ( strcasestr(strInput, "setServo") != NULL ) {

    } else if ( strcasestr(strInput, "setMotor") != NULL ) {

    } else if ( strcasestr(strInput, "Go") != NULL ) {
        syslog(LOG_INFO, "Go!");
        memcpy(locUpdates.motorA, locDefaults.motorAGo, 3);
        memcpy(locUpdates.motorB, locDefaults.motorBGo, 3);
        memcpy(locUpdates.motorC, locDefaults.motorCGo, 3);
        memcpy(locUpdates.motorD, locDefaults.motorDGo, 3);
    } else if ( strcasestr(strInput, "Stop") != NULL ) {
        memcpy(locUpdates.motorA, locDefaults.motorAStop, 3);
        memcpy(locUpdates.motorB, locDefaults.motorBStop, 3);
        memcpy(locUpdates.motorC, locDefaults.motorCStop, 3);
        memcpy(locUpdates.motorD, locDefaults.motorDStop, 3);
    } else if ( strcasestr(strInput, "Back") != NULL ) {
        memcpy(locUpdates.motorA, locDefaults.motorABack, 3);
        memcpy(locUpdates.motorB, locDefaults.motorBBack, 3);
        memcpy(locUpdates.motorC, locDefaults.motorCBack, 3);
        memcpy(locUpdates.motorD, locDefaults.motorDBack, 3);
    } else if ( strcasestr(strInput, "PivotRight") != NULL ) {
        memcpy(locUpdates.motorA, locDefaults.motorAPivotRight, 3);
        memcpy(locUpdates.motorB, locDefaults.motorBPivotRight, 3);
        memcpy(locUpdates.motorC, locDefaults.motorCPivotRight, 3);
        memcpy(locUpdates.motorD, locDefaults.motorDPivotRight, 3);
    } else if ( strcasestr(strInput, "PivotLeft") != NULL ) {
        memcpy(locUpdates.motorA, locDefaults.motorAPivotLeft, 3);
        memcpy(locUpdates.motorB, locDefaults.motorBPivotLeft, 3);
        memcpy(locUpdates.motorC, locDefaults.motorCPivotLeft, 3);
        memcpy(locUpdates.motorD, locDefaults.motorDPivotLeft, 3);
    } else if ( strcasestr(strInput, "TurnRight") != NULL ) {
        memcpy(locUpdates.motorA, locDefaults.motorATurnRight, 3);
        memcpy(locUpdates.motorB, locDefaults.motorBTurnRight, 3);
        memcpy(locUpdates.motorC, locDefaults.motorCTurnRight, 3);
        memcpy(locUpdates.motorD, locDefaults.motorDTurnRight, 3);
    } else if ( strcasestr(strInput, "TurnLeft") != NULL ) {
        memcpy(locUpdates.motorA, locDefaults.motorATurnLeft, 3);
        memcpy(locUpdates.motorB, locDefaults.motorBTurnLeft, 3);
        memcpy(locUpdates.motorC, locDefaults.motorCTurnLeft, 3);
        memcpy(locUpdates.motorD, locDefaults.motorDTurnLeft, 3);
    } else {

    }

    // Update checksum
    locUpdates.checksum = ComputeChecksum((uint8_t*)(&locUpdates.motorA),
        locUpdates.length);
}

void SetDefaults ( void )
{
    memcpy(locUpdates.motorA, locDefaults.motorADefault, 3);
    memcpy(locUpdates.motorB, locDefaults.motorBDefault, 3);
    memcpy(locUpdates.motorC, locDefaults.motorCDefault, 3);
    memcpy(locUpdates.motorD, locDefaults.motorDDefault, 3);
    memcpy(locUpdates.servo1, locDefaults.servo1Default, 3);
    memcpy(locUpdates.servo2, locDefaults.servo2Default, 3);
    memcpy(locUpdates.servo3, locDefaults.servo3Default, 3);
    memcpy(locUpdates.servo4, locDefaults.servo4Default, 3);
    memcpy(locUpdates.servo5, locDefaults.servo5Default, 3);
    memcpy(locUpdates.servo6, locDefaults.servo6Default, 3);
    memcpy(locUpdates.servo7, locDefaults.servo7Default, 3);
    memcpy(locUpdates.servo8, locDefaults.servo8Default, 3);
    memcpy(locUpdates.extLed, locDefaults.extLedDefault, 3);
    locUpdates.checksum = ComputeChecksum((uint8_t*)(&locUpdates.motorA),
        locUpdates.length);
}

bool LoadDefaults ( void )
{
    int num;
    FILE *fd;
    if ( (fd = fopen(DEFAULT_FILE, "r") ) == NULL ) return false;
    num = fread(&locDefaults, 1, sizeof(locDefaults), fd);
    fclose(fd);
    if(num != sizeof(locDefaults)) return false;

    return true;
}

bool SaveDefaults ( void )
{
    int num;
    FILE *fd;
    if ( (fd = fopen(DEFAULT_FILE, "w") ) == NULL ) return false;
    num = fwrite(&locDefaults, 1, sizeof(locDefaults), fd);
    fclose(fd);
    if(num != sizeof(locDefaults)) return false;

    return true;
}

#ifdef WEBCAM
/* ------------------------------------------------------------ */
/***    Webcam
**
**  Synopsis:
**      void    Webcam(void *threadid);
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
**
*/
void* Webcam(void *arg) {

#if defined(DEBUG)
    fprintf(stdout, "Webcam broadcasting.\n");
#else
    syslog(LOG_INFO, "Webcam broadcasting.");
#endif

    //start webcam broadcasting
    system("mjpg_streamer -i \"/usr/lib/input_uvc.so -d /dev/video1\" -o \"/usr/lib/output_http.so\"");
    return NULL;
} //end Webcam


/****************************************************************************/
#endif


