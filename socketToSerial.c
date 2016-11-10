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
#include "../Socket/Socket.h"

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
// This is what we send the board
uint8_t tlvLocUpdate[LENGTH_LOC_UPDATE];
// This is what we send the client
uint8_t response[RESPONSE_LENGTH];

uint8_t tlvLocUpdateDefaults[] = {
    TYPE_LOC_UPDATE,
    LENGTH_LOC_UPDATE,
    (0xFF & (DFLT_MOTOR >> 16), // Motor A - MSB first
    (0xFF & (DFLT_MOTOR >> 8),  // Motor A
    (0xFF & (DFLT_MOTOR),       // Motor A - LSB
    (0xFF & (DFLT_MOTOR >> 16), // Motor B - MSB first
    (0xFF & (DFLT_MOTOR >> 8),  // Motor B
    (0xFF & (DFLT_MOTOR),       // Motor B - LSB
    (0xFF & (DFLT_MOTOR >> 16), // Motor C - MSB first
    (0xFF & (DFLT_MOTOR >> 8),  // Motor C
    (0xFF & (DFLT_MOTOR),       // Motor C - LSB
    (0xFF & (DFLT_MOTOR >> 16), // Motor D - MSB first
    (0xFF & (DFLT_MOTOR >> 8),  // Motor D
    (0xFF & (DFLT_MOTOR),       // Motor D - LSB
    (0xFF & (DFLT_MOTOR >> 16), // Servo Channel 1 - MSB
    (0xFF & (DFLT_MOTOR >> 8),  // Servo Channel 1
    (0xFF & (DFLT_MOTOR),       // Servo Channel 1 - LSB

}

// handles to the threads
pthread_t threadWebcam;
pthread_t threadBoardComms;

char serialPort[LEN_SERIAL_PORT];
int portNum;
int baudRate;
bool running;
extern SocketInterface_T socketIntf;

/* ------------------------------------------------------------ */
/*              Local Variables                                 */
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*              Forward Declarations                            */
/* ------------------------------------------------------------ */

void*   Webcam(void *arg);
void*   BoardComms(void *arg);
void    HandleClient( void );
void    InterpretSocketCommand(uint8_t *data, uint32_t length);

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
void signal_handler(int sig) {

    switch(sig) {
        case SIGHUP:
        case SIGTERM:
        case SIGINT:
        case SIGQUIT:
            // Notify thread that we're closing
            if ( running ) {
                running = false;
                // Wait for threads to return
            #if defined(WEBCAM)
                pthread_join(threadWebcam, NULL);
            #endif
                pthread_join(threadBoardComms, NULL);
            }
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

    // init globals
    running = false;
    tlvLocUpdate[POS_TLV_TYPE] = TYPE_LOC_UPDATE;
    tlvLocUpdate[POS_TLV_LENGTH] = LENGTH_LOC_UPDATE;

    // Setup signal handling
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    syslog(LOG_INFO, "%s daemon starting up", DAEMON_NAME);

    // Setup syslog logging - see SETLOGMASK(3)
#if defined(DEBUG)
    setlogmask(LOG_UPTO(LOG_DEBUG));
    openlog(DAEMON_NAME, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);
#else
    setlogmask(LOG_UPTO(LOG_INFO));
    openlog(DAEMON_NAME, LOG_CONS, LOG_USER);
#endif

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
    printf("Attempting to connect on port %d...\n", portNum);
    if ( socketIntf.OpenAndConnect(portNum) ) {
        printf("Connected!\n");
    } else {
        printf("Failed to connect!\n");
        return 0;
    }

    /* Run until cancelled */
    while (fTrue) {

        syslog(LOG_INFO, "Client connected.");
        HandleClient();
        syslog(LOG_INFO, "Client disconnected.");

        // Re-open port and wait for client to connect
        printf("Attempting to connect on port %d...\n", portNum);
        if ( socketIntf.OpenAndConnect(portNum) ) {
            printf("Connected!\n");
        } else {
            printf("Failed to connect!\n");
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
**      int sock, handle to TCP connection
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
    uint32_t cntSocketRx;

    #if defined(WEBCAM)
    // Start webcam thread
    if (pthread_create(&threadWebcam, NULL, &Webcam, NULL)) {
        printf("ERROR;  pthread_create(&threadWebcam... \n");
    } // end if
    #endif

    /****************************************/
    /* Start BoardComm thread               */
    if (pthread_create(&threadBoardComms, NULL, &BoardComms, NULL)) {
        printf("ERROR: pthread_create(&threadBoardComms...\n");
    }
    /****************************************/

    /**************** Main TCP linked section **************************/
    while(fTrue) {
        cntSocketRx = socketIntf.Read(socketRx, BUFFSIZE);
        if ( cntSocketRx> 0 ) {
            InterpretSocketCommand(socketRx, cntSocketRx);

            // Push serial response back to client over the socket
            socketIntf.Write(response, RESPONSE_LENGTH);
        } else {
            // restore defaults
            memcpy(tlvLocUpdate, tlvLocUpdateDefaults, LENGTH_LOC_UPDATE);
            socketIntf.Close(); // close client connection
            socketIntf.Close(); // close socket // may not be the best idea, ok for now
            break;
        }
    }// end while
    /********************************************************************/
} //end HandleClient()

void* BoardComms(void *arg)
{
    uint8_t serialRx[BUFFSIZE];
    int     cntSerialRx;

    /*************************
     This code should run in a loop as long as a client is connected over socket.
     Client connects
     this loop runs and pulls data from a global struct
     client disconnects
     this loop exits
    **************************/

    // Bring up the serial port
    if ( ! SerialInit(serialPort, baudRate) ) {
#if defined(DEBUG)
        fprintf(stdout, "ERROR: Couldn't open %s.\n", serialPort);
#else
        syslog(LOG_PERROR, "ERROR: Couldn't open %s.\n", serialPort);
#endif
        return NULL;
    }

    while (running) {
        // Send the board the data that we've been updating with interpretted socket data
        SerialWriteNBytes(tlvLocUpdate, LENGTH_LOC_UPDATE);

        // Give board a moment to respond
        usleep(SERIAL_READ_DELAY);

        // Get response from serial ( should be ADC battery voltage reading )
        cntSerialRx = SerialRead(serialRx);
        cntSerialRx +=1; // Silence warning

        // Interpret TLV contents and reformat for socket client

        // Pack our response message into the buffer used by HandleClient()
        //response[RESPONSE_LENGTH];

        usleep(SERIAL_SLEEP_PERIOD);
    }

    SerialClose();
    return NULL;
}

void InterpretSocketCommand(uint8_t *data, uint32_t length)
{

    // MSB first to LSB as we increment pointer in buffer. 24-bits each value
    //tlvLocUpdate[POS_EN_A] = foo;




#ifdef DEAD
/*********************** Begin copied code ***********************/
// All position definitions start at 2.  0 is type, 1 is length, 2 is start of values thereafter.  Checksum is last
// Position of checksum is always (LENGTH_* + 2)

/****** OUTGOING DATA - FROM DEVICE TO PC ******/
//   Local UPDATE position data
//     There are 13 PWM values that need to be updated, each being 24-bits#define TYPE_LOC_UPDATE     0xAA

        case TYPE_LOC_UPDATE:
            /*

            If code is compiled like this:
                uint32_t number;
                uint8_t *ptr;
                uint8_t i;

                number = 0x12345678;
                ptr = (uint8_t*)(&number);

                for ( i = 0; i < 4; i++ ) {
                    printf("number[%d]: 0x%x\n", i, *ptr);
                    ptr++;
                }

            The result will be this:
                number[0]: 0x78
                number[1]: 0x56
                number[2]: 0x34
                number[3]: 0x12

            Thus, the pointer comes from MSB downto LSB as it increments in memory
             */
            // MOTOR A - wide timer
            value = (uint32_t)((command[POS_EN_A] << 16)|(command[POS_EN_A + 1] << 8)|(command[POS_EN_A + 2]));
            TimerMatchSet(SERVO_8_MOTOR_A_TIMER_BASE, MOTOR_A_TIMER, value);

/************************ END COPIED CODE *******************/
#endif
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


