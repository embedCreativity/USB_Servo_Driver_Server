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
/*  07/10/2017 (MarkT): Adding SQL logging of voltage/power for web     */
/*                                                                      */
/************************************************************************/

#define _GNU_SOURCE

//Debug switch
#define  DEBUG

//compilation directives
#define VERBOSE
#define SQL_POWER


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
#include <ec_serial.h> // My installed library

#if defined(SQL_POWER)
#include <sqlite3.h> // for webpage display of voltage and current readings
#endif

#include <pthread.h> //peripheral thread library; must compile with -lpthread option

//seperate thread that handles incoming serial communications
#include "socketToSerial.h"
#include "TLV_definitions.h"

/* ------------------------------------------------------------ */
/*              Local Type Definitions                          */
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*              Global Variables                                */
/* ------------------------------------------------------------ */

//--- TODO: protect all instances with pthread mutexes
// This is what we send to the board
tlvPosition_T cfgPosition;
tlvPowerManagement_T cfgPower;

// structure of saved default values
savedDefaults_T cfgDefaultPosition;

// struct for holding data,len returned from SerialRead
serialRx_T serialRxData;
tlvAck_T tlvAck = { .type = TYPE_ACK, .length = LENGTH_ACK, .status = 0,
  .adcVoltage= 0, .adcCurrent = 0, .checksum = 0 };
uint16_t shortADCVoltage;
uint16_t shortADCCurrent;
uint8_t bBoardStatus;

// handles to the threads
bool fRunning = true;

#if defined(SQL_POWER)
pthread_t threadUpdateSql;
#endif

char serialPort[LEN_SERIAL_PORT];
int portNum;
int baudRate;
extern SocketInterface_T socketIntf;

/* ------------------------------------------------------------ */
/*              Local Variables                                 */
/* ------------------------------------------------------------ */

#if defined(SQL_POWER)
float voltage;
float current;
#endif

/* ------------------------------------------------------------ */
/*              Forward Declarations                            */
/* ------------------------------------------------------------ */
#if defined(SQL_POWER)
void* SqlLogging(void *arg);
#endif

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
            fRunning = false; // signal threads to quit
            // Wait for threads to return
        #if defined(SQL_POWER)
            pthread_join(threadUpdateSql, NULL);
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

    cfgPosition.type = TYPE_LOC_UPDATE;
    cfgPosition.length = LENGTH_LOC_UPDATE;
    cfgPower.type = TYPE_PWR_UPDATE;
    cfgPower.length = LENGTH_PWR_UPDATE;

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

void SetDefaults ( void )
{
    memcpy(cfgPosition.motorA, cfgDefaultPosition.motorADefault, 3);
    memcpy(cfgPosition.motorB, cfgDefaultPosition.motorBDefault, 3);
    memcpy(cfgPosition.motorC, cfgDefaultPosition.motorCDefault, 3);
    memcpy(cfgPosition.motorD, cfgDefaultPosition.motorDDefault, 3);
    memcpy(cfgPosition.servo1, cfgDefaultPosition.servo1Default, 3);
    memcpy(cfgPosition.servo2, cfgDefaultPosition.servo2Default, 3);
    memcpy(cfgPosition.servo3, cfgDefaultPosition.servo3Default, 3);
    memcpy(cfgPosition.servo4, cfgDefaultPosition.servo4Default, 3);
    memcpy(cfgPosition.servo5, cfgDefaultPosition.servo5Default, 3);
    memcpy(cfgPosition.servo6, cfgDefaultPosition.servo6Default, 3);
    memcpy(cfgPosition.servo7, cfgDefaultPosition.servo7Default, 3);
    memcpy(cfgPosition.servo8, cfgDefaultPosition.servo8Default, 3);
    memcpy(cfgPosition.extLed, cfgDefaultPosition.extLedDefault, 3);
    cfgPosition.checksum = ComputeChecksum((uint8_t*)(&cfgPosition.motorA),
        cfgPosition.length);

    cfgPower.config = (1 << BS_PWR_C); // Turn on controller, keep motors off
    cfgPower.checksum = ComputeChecksum((uint8_t*)(&cfgPower.config),
        cfgPower.length);
}

bool LoadDefaults ( void )
{
    int num;
    FILE *fd;
    if ( (fd = fopen(DEFAULT_FILE, "r") ) == NULL ) return false;
    num = fread(&cfgDefaultPosition, 1, sizeof(cfgDefaultPosition), fd);
    fclose(fd);
    if(num != sizeof(cfgDefaultPosition)) return false;

    return true;
}

bool SaveDefaults ( void )
{
    int num;
    FILE *fd;
    if ( (fd = fopen(DEFAULT_FILE, "w") ) == NULL ) return false;
    num = fwrite(&cfgDefaultPosition, 1, sizeof(cfgDefaultPosition), fd);
    fclose(fd);
    if(num != sizeof(cfgDefaultPosition)) return false;

    return true;
}

/****************************************************************************/


