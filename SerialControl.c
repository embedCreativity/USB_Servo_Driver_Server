/************************************************************************/
/*                                                                      */
/*  SerialControl.c --  Helper functions that control serial comm.      */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  Copyright 2009, Digilent Inc.                                       */
/*                                                                      */
/************************************************************************/
/*  Module Description:                                                 */
/*                                                                      */
/*  This module was written to facilitate the use of the PC's serial    */
/*  comm port for interfacing with embedded devices.                    */
/*                                                                      */
/*  This file was written specifically for Ubuntu 8.10 Linux            */
/*  installations and as such, may not work on other distrubutions of   */
/*  Linux.                                                              */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  02/02/2009 (MarkT): created                                         */
/*  10/11/2016 (MarkT): updated to remove dependence on ascii strings   */
/*                                                                      */
/************************************************************************/

/* ------------------------------------------------------------ */
/*              Include File Definitions                        */
/* ------------------------------------------------------------ */

#include <stdint.h>  /* for unsigned values*/
#include <stdlib.h>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#include "SerialControl.h"

/* ------------------------------------------------------------ */
/*              Local Type Definitions                          */
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*              Global Variables                                */
/* ------------------------------------------------------------ */

int fd;

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
/***    SerialWriteNBytes
**
**  Synopsis:
**      int SerialWriteNBytes(uint8_t *rgbChars, int n)
**
**  Parameters:
**      *rgbChars       pointer to string
**      n               number of characters to be written to serial port
**
**  Return Values:
**      0               failure
**      1               success
**
**  Errors:
**      none
**
**  Description:
**      Uses the a globally defined integer "file descriptor" fd to write
**      to the serial port.  The serial port must be previously opened and
**      the fd must be the assigned file descriptor for the serial port.
**      Please use the SerialInit(char *szDevice) prior to using this function.
*/
int SerialWriteNBytes(uint8_t *rgbChars, int n) {

    int i;

    for(i = 0; i < n; i++) {

        int n = write(fd, rgbChars, 1);
        if (n < 0) {
            printf("Write Error : %d,  %s\n", errno, strerror(errno));
            printf("0x%x ", *rgbChars);
            return 0;
        }
//debug     printf("0x%x ", *rgbChars);
        rgbChars++; //increment pointer
    }
    return 1;
}

/* ------------------------------------------------------------ */
/***    SerialWriteByte
**
**  Synopsis:
**      int SerialWriteByte(uint8_t *pByte)
**
**  Parameters:
**      *pByte          pointer to a char
**
**  Return Values:
**      0               failure
**      1               success
**
**  Errors:
**      none
**
**  Description:
**      Uses the a globally defined integer "file descriptor" fd to write
**      to the serial port.  The serial port must be previously opened and
**      the fd must be the assigned file descriptor for the serial port.
**      Please use the SerialInit(char *szDevice) prior to using this function.
*/
int SerialWriteByte(uint8_t *pByte) {

    int n = write(fd, pByte, 1);
    if (n < 0) {
        printf("Write Error : %d,  %s\n", errno, strerror(errno));
        printf("0x%x ", *pByte);
        return 0;
    }

//debug printf("0x%x ", *pByte);

    return 1;
}

/* ------------------------------------------------------------ */
/***    SerialRead
**
**  Synopsis:
**      int SerialRead(char *result)
**
**  Parameters:
**      *result         pointer to string
**
**  Return Values:
**      number of characters read on success
**      -1 on error
**
**  Errors:
**      none
**
**  Description:
**      Uses the a globally defined integer "file descriptor" fd to write
**      to the serial port.  The serial port must be previously opened and
**      the fd must be the assigned file descriptor for the serial port.
**      Please use the SerialInit(char *szDevice) prior to using this function.
**      Places the read NULL terminated string into *result.
*/
int SerialRead(char *result) {
    int bytesRead;

    bytesRead = read(fd, result, 256);

    if (bytesRead < 0) {
        if (errno == EAGAIN) {
            printf("SERIAL EAGAIN ERROR\n");
            return -1;
        } else {
            printf("SERIAL read error %d %s\n", errno, strerror(errno));
            return -1;
        }
    }

    return bytesRead;
}

/* ------------------------------------------------------------ */
/***    SerialGetBaud
**
**  Synopsis:
**      int SerialGetBaud( void )
**
**  Parameters:
**      none
**
**  Return Values:
**      inputSpeed          integer value relating baud rate
**
**  Errors:
**      none
**
**  Description:
**      Uses the a globally defined integer "file descriptor" fd to write
**      to the serial port.  The serial port must be previously opened and
**      the fd must be the assigned file descriptor for the serial port.
**      Please use the SerialInit(char *szDevice) prior to using this function.
**      Places the defined baud rate value into inputSpeed and returns.
*/
int SerialGetBaud( void ) {
#ifndef USE_XUART
    struct termios termAttr;
    int inputSpeed = -1;
    speed_t baudRate;
    tcgetattr(fd, &termAttr);
    /* Get the input speed. */
    baudRate = cfgetispeed(&termAttr);
    switch (baudRate) {
        case B0:      inputSpeed = 0; break;
        case B50:     inputSpeed = 50; break;
        case B110:    inputSpeed = 110; break;
        case B134:    inputSpeed = 134; break;
        case B150:    inputSpeed = 150; break;
        case B200:    inputSpeed = 200; break;
        case B300:    inputSpeed = 300; break;
        case B600:    inputSpeed = 600; break;
        case B1200:   inputSpeed = 1200; break;
        case B1800:   inputSpeed = 1800; break;
        case B2400:   inputSpeed = 2400; break;
        case B4800:   inputSpeed = 4800; break;
        case B9600:   inputSpeed = 9600; break;
        case B19200:  inputSpeed = 19200; break;
        case B38400:  inputSpeed = 38400; break;
    }
    return inputSpeed;

#else
    return 0;
#endif

}

/* ------------------------------------------------------------ */
/***    SerialInit
**
**  Synopsis:
**      int SerialInit( char *szDevice )
**
**  Parameters:
**      *szDevice           string indicating location of serial port "file"
**
**  Return Values:
**      1                   success
**      0                   failure
**
**  Errors:
**      none
**
**  Description:
**      Uses the a globally defined integer "file descriptor" fd to write
**      to the serial port.  The serial port must be previously opened and
**      the fd must be the assigned file descriptor for the serial port.
**      Please use the SerialInit(char *szDevice) prior to using this function.
**      Opens the serial port with a baud rate of 9600.  No parity or flow control.
**      One stop bit.
*/
int SerialInit(char *szDevice) {

    struct termios options;
    char rgchBuf[32];

#ifdef  USE_XUART
    system("xuartctl --port 0 --server --speed 38400");
    strcpy(rgchBuf, "/dev/pts/1");
    fd = open(rgchBuf, O_RDWR | O_NOCTTY | O_NDELAY);
#else
    strcpy(rgchBuf, szDevice);
    fd = open(szDevice, O_RDWR | O_NOCTTY | O_NDELAY);
#endif

    if (fd == -1) {
        printf("Unable to open %s!", rgchBuf);
        return 0; //indicate failure
    } else {
        fcntl(fd, F_SETFL, 0);
    }

#ifndef USE_XUART
    // Get the current options for the port...
    tcgetattr(fd, &options);
    // Set the baud rates
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    // Enable the receiver and set local mode...
    options.c_cflag |= (CLOCAL | CREAD);

    options.c_cflag &= ~PARENB; //no parity
    options.c_cflag &= ~CSTOPB; //1 stop bit
    options.c_cflag &= ~CSIZE; //ready value for character size
    options.c_cflag |= CS8; //8-bit character size

    // Set the new options for the port...
    tcsetattr(fd, TCSANOW, &options); //change attributes NOW
#endif //USE_XUART

    return 1;
}

/* ------------------------------------------------------------ */
/***    SerialClose
**
**  Synopsis:
**      void SerialClose( void)
**
**  Parameters:
**
**
**  Return Values:
**
**
**  Errors:
**      none
**
**  Description:
**      Uses the a globally defined integer "file descriptor" fd to write
**      to the serial port.  The serial port must be previously opened and
**      the fd must be the assigned file descriptor for the serial port.
**      Please use the SerialInit(char *szDevice) prior to using this function.
**      Closes the fd file descriptor.
*/
void SerialClose(void) {
    if(0 > close(fd)) {
        printf("\nError closing serial connection.\n");
    }
    else {
        printf("Serial connection closed.\n");
    }
}

/************************************ EOF ********************************/
