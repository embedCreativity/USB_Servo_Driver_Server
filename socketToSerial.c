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
#include <sys/types.h>
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

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
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

/* ------------------------------------------------------------ */
/*              Global Variables                                */
/* ------------------------------------------------------------ */

long    lThreadID;
pthread_t threadWebcam; //handle to the thread

char serialPort[LEN_SERIAL_PORT];
int portNum;

/* ------------------------------------------------------------ */
/*              Local Variables                                 */
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*              Forward Declarations                            */
/* ------------------------------------------------------------ */

void    Die(char *mess) { perror(mess); exit(1); }
void    *Webcam(void *threadid);
void    HandleClient(int sock);

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
            syslog(LOG_WARNING, "Received SIGHUP signal.");
            exit(1);
            break;
        case SIGTERM:
            syslog(LOG_WARNING, "Received SIGTERM signal.");
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
int main(int argc, char *argv[]) {

    int serversock, clientsock, flag;
    struct sockaddr_in echoserver, echoclient;

    if (argc != 3) {
        fprintf(stderr, "USAGE: %s <path to serial port> <socket port number>\n", argv[0]);
        exit(1);
    }

    strncpy(serialPort, (char*)argv[1], LEN_SERIAL_PORT);
    portNum = atoi(argv[2]);

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

    /* Create the TCP socket */
    if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        Die("Failed to create socket");
    }

    /* Disable the Nagle (TCP No Delay) algorithm */
    flag = 1;
    if (-1 == setsockopt( serversock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) ) ) {
        printf("Couldn't setsockopt(TCP_NODELAY)\n");
        exit( EXIT_FAILURE );
    }
    /* Set the Keep Alive property */
    flag = 1;
    if (-1 == setsockopt( serversock, SOL_SOCKET, SO_KEEPALIVE, (char *)&flag, sizeof(flag) ) ) {
        printf("Couldn't setsockopt(SO_KEEPALIVE)\n");
        exit( EXIT_FAILURE );
    }
    /* Allow the re-use of port numbers to avoid error */
    flag = 1;
    if (-1 == setsockopt( serversock, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag) ) ) {
        printf("Couldn't setsockopt(SO_REUSEADDR)\n");
        exit( EXIT_FAILURE );
    }

    /* Construct the server sockaddr_in structure */
    memset(&echoserver, 0, sizeof(echoserver));         /* Clear struct */
    echoserver.sin_family = AF_INET;                    /* Internet/IP */
    echoserver.sin_addr.s_addr = htonl(INADDR_ANY);     /* Incoming addr */
    echoserver.sin_port = htons(portNum);               /* server port */

    /* Bind the server socket */
    if (bind(serversock, (struct sockaddr *) &echoserver,
                       sizeof(echoserver)) < 0) {
        Die("Failed to bind the server socket");
    }
    /* Listen on the server socket */
    if (listen(serversock, MAXPENDING) < 0) {
        Die("Failed to listen on server socket");
    }

    /* Run until cancelled */
    while (fTrue) {
        unsigned int clientlen = sizeof(echoclient);
        /* Wait for client connection */
        if ((clientsock = accept(serversock, (struct sockaddr *) &echoclient, &clientlen)) < 0) {
            fprintf(stdout, "clientsock = accept(serversock...    FAILED");
            continue;
        }

        syslog(LOG_INFO, "Client connected: %s", inet_ntoa(echoclient.sin_addr));
        HandleClient(clientsock);
        syslog(LOG_INFO, "Client disconnected.");
    }

    /* Prepare to exit the daemon process.  Free any resources before exit. */
    syslog(LOG_INFO, "%s daemon exiting", DAEMON_NAME);
    exit(0);

} //end main()

/* ------------------------------------------------------------ */
/***    HandleClient
**
**  Synopsis:
**      void    HandleClient(int sock)
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
void HandleClient(int sock) {

    uint8_t buffer[BUFFSIZE];
    int     received;

    #if defined(WEBCAM)
    //Start webcam thread
    if (pthread_create(&threadWebcam, NULL, Webcam, (void *)lThreadID)) {
        printf("ERROR;  pthread_create(&threadWebcam... \n");
    } //end if
    #endif

    // Bring up the serial port
    if ( ! SerialInit(serialPort) ) {
        Die("Failed to open serial port");
    }

    /**************** Main TCP linked section **************************/
    while(fTrue) {

        received = recv(sock, buffer, BUFFSIZE, 0);
        if ( received > 0 ) {
            // Send data over serial port
            SerialWriteNBytes(buffer, received);

            // Get response from serial
            /*received = SerialRead(buffer);

            // Send response back over socket
            if (send(sock, buffer, received, 0) != received) {
                close(sock);
                Die("Failed to send bytes to client");
            }*/
        }

    }// end while
    /********************************************************************/

    close(sock);
} //end HandleClient()

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


