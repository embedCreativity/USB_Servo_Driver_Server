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
#include "../Socket/Socket.h"

#include <pthread.h> //peripheral thread library; must compile with -lpthread option

//seperate thread that handles incoming serial communications
#include "SerialControl.h" /* my Serial comm functions */
#include "TLV_definitions.h"


/* ------------------------------------------------------------ */
/*              Local Type Definitions                          */
/* ------------------------------------------------------------ */

//boolean definitions
#define fFalse          0
#define fTrue           !fFalse

#define MAXPENDING      1    /* Max connection requests */
#define BUFFSIZE        1500

#define LEN_SERIAL_PORT 32

#define DAEMON_NAME "serialServer"

// From TivaPWM.h
// 20ms (50Hz) = SERVO_REFRESH_PERIOD clocks at a system clock rate of 80MHz
#define SERVO_REFRESH_PERIOD        1600000
// PERIOD = fractional second * 80M clocks per second
// min = 750uS * 80M
#define SERVO_MIN_PERIOD            60000
// mid = 1.5ms * 80M
#define SERVO_MID_PERIOD            120000
// max = 2.25ms * 80M
#define SERVO_MAX_PERIOD            180000

// 20KHz (outside audible?)
#define MOTOR_REFRESH_PERIOD        4000
//#define MOTOR_REFRESH_PERIOD        8000

// 50Hz flicker
#define EXTLED_REFRESH_PERIOD       1600000




/* ------------------------------------------------------------ */
/*              Global Variables                                */
/* ------------------------------------------------------------ */

long    lThreadID;
pthread_t threadWebcam; //handle to the thread

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

void    Die(char *mess) { perror(mess); exit(1); }
void    *Webcam(void *threadid);
void    *BoardComms(void *threadid);
void    HandleClient( void );

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
            syslog(LOG_WARNING, "Received SIGHUP signal.");
            SerialClose();
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
    exit(0);

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
void HandleClient( void ) {

    uint8_t serialTx[BUFFSIZE];
    uint8_t serialRx[BUFFSIZE];
    int     cntSerialTx;
    int     cntSerialRx;

    #if defined(WEBCAM)
    //Start webcam thread
    if (pthread_create(&threadWebcam, NULL, Webcam, (void *)lThreadID)) {
        printf("ERROR;  pthread_create(&threadWebcam... \n");
    } //end if
    #endif

    /**************************************
    Start BoardComm thread TODO
    pthread_create(&threadBoardComms, NULL, BoardComms, (void *)lBoardThreadID)) {
        printf("ERROR: pthread_create(&threadBoardComms...\n");
    }
    **************************************/
    // Bring up the serial port
    if ( ! SerialInit(serialPort, baudRate) ) {
        Die("Failed to open serial port");
    }

    /**************** Main TCP linked section **************************/
    while(fTrue) {
        cntSerialTx = socketIntf.Read(serialTx, BUFFSIZE);
        serialTx[cntSerialTx] = 0; // null terminate
        if ( cntSerialTx > 0 ) {

    k
            // Send data over serial port
            SerialWriteNBytes(serialTx, cntSerialTx);
            printf("socket rx/tx serial-->%s\n", serialTx);

            usleep(1000);

            // Get response from serial
            cntSerialRx = SerialRead(serialRx);
            serialRx[cntSerialRx] = 0; // null terminate

            // Push serial response back to client over the socket
            socketIntf.Write(serialRx, cntSerialRx);
            printf("serial rx/socket tx-->%s\n", serialRx);
        } else {
            socketIntf.Close(); // close client connection
            socketIntf.Close(); // close socket // may not be the best idea, ok for now
            SerialClose(); // close the serial port
            break;
        }
    }// end while
    /********************************************************************/
} //end HandleClient()


void *BoardComms(void *threadid) {
{

/***********************88
This code should run in a loop as long as a client is connected over socket.
 Client connects
 this loop runs and pulls data from a global struct
 client disconnects
 this loop exits
**************************/


/*********************** Begin copied code ***********************/
#define POS_TLV_TYPE  0
#define POS_TLV_LENGTH  1
#define POS_TLV_BEGIN_DATA 2
#define TLV_OVERHEAD 3

// All position definitions start at 2.  0 is type, 1 is length, 2 is start of values thereafter.  Checksum is last
// Position of checksum is always (LENGTH_* + 2)

/****** OUTGOING DATA - FROM DEVICE TO PC ******/
//   Local UPDATE position data
//     There are 13 PWM values that need to be updated, each being 24-bits#define TYPE_LOC_UPDATE     0xAA
#define LENGTH_LOC_UPDATE   39
#define POS_EN_A            2
#define POS_EN_B            5
#define POS_EN_C            8
#define POS_EN_D            11
#define POS_CHAN1           14
#define POS_CHAN2           17
#define POS_CHAN3           20
#define POS_CHAN4           23
#define POS_CHAN5           25
#define POS_CHAN6           28
#define POS_CHAN7           31
#define POS_CHAN8           34
#define POS_EXT_LED         37

// Update position data Ack
#define TYPE_UPDATE_ACK      0xBB
#define LENGTH_UPDATE_ACK    0

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
void *Webcam(void *threadid) {

#if defined(DEBUG)
    fprintf(stdout, "Webcam broadcasting.\n");
#else
    syslog(LOG_INFO, "Webcam broadcasting.");
#endif

    //start webcam broadcasting
    system("mjpg_streamer -i \"/usr/lib/input_uvc.so -d /dev/video1\" -o \"/usr/lib/output_http.so\"");
} //end Webcam
/****************************************************************************/
#endif


