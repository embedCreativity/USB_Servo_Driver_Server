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
/*                      removed xuart stuff as I'm not likely going to  */
/*                      to use anything from Technologic Systems ever   */
/*                      again.                                          */
/*                                                                      */
/************************************************************************/

/* ------------------------------------------------------------ */
/*              Include File Definitions                        */
/* ------------------------------------------------------------ */

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
        //printf("SerialWriteNBytes(n=%d)\n", n);
        int n = write(fd, rgbChars, 1);
        if (n < 0) {
            printf("Write Error : %d,  %s\n", errno, strerror(errno));
            printf("0x%x ", *rgbChars);
            return 0;
        }
        //printf("0x%x ", *rgbChars);
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
**      int SerialRead(uint8_t *result)
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
int SerialRead(uint8_t *result) {
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
        case B57600:  inputSpeed = 57600; break;
        case B115200:  inputSpeed = 115200; break;
        case B230400:  inputSpeed = 230400; break;
    }
    return inputSpeed;
}


/* ------------------------------------------------------------ */
/***    IntToBaud
**
**  Synopsis:
**      int IntToBaud( void )
**
**  Parameters:
**      int baud
**
**  Return Values:
**      OS Baud Rate type
**
**  Errors:
**      none
**
**  Description:
*/
speed_t IntToBaud( int baudRate ) {
    speed_t osBaudRate;

    switch (baudRate) {
        case 0:         osBaudRate = B0; break;
        case 50:        osBaudRate = B50; break;
        case 110:       osBaudRate = B110; break;
        case 134:       osBaudRate = B134; break;
        case 150:       osBaudRate = B150; break;
        case 200:       osBaudRate = B200; break;
        case 300:       osBaudRate = B300; break;
        case 600:       osBaudRate = B600; break;
        case 1200:      osBaudRate = B1200; break;
        case 1800:      osBaudRate = B1800; break;
        case 2400:      osBaudRate = B2400; break;
        case 4800:      osBaudRate = B4800; break;
        case 9600:      osBaudRate = B9600; break;
        case 19200:     osBaudRate = B19200; break;
        case 38400:     osBaudRate = B38400; break;
        case 57600:     osBaudRate = B57600; break;
        case 115200:    osBaudRate = B115200; break;
        case 230400:    osBaudRate = B230400; break;
        default: osBaudRate = B0; break;
    }
    return osBaudRate;
}

/* ------------------------------------------------------------ */
/***    SerialInit
**
**  Synopsis:
**      int SerialInit( char *szDevice )
**
**  Parameters:
**      char *szDevice           string indicating location of serial port "file"
**      int baudRate
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
bool SerialInit(char *szDevice, int baudRate) {

    struct termios options;
    memset (&options, 0, sizeof(options)); // clear whatever was in there before

    fd = open(szDevice, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1) {
        printf("Unable to open %s!", szDevice);
        return false;
    } else {
        fcntl(fd, F_SETFL, 0);
    }

    // Get the current options for the port...
    if ( 0 != tcgetattr(fd, &options) ) {
        printf("SerialInit Error - unable to get options!\n");
        return false;
    }

    // Set the baud rates
    cfsetispeed(&options, IntToBaud(baudRate));
    cfsetospeed(&options, IntToBaud(baudRate));
    // Enable the receiver and set local mode...
    options.c_cflag |= (CLOCAL | CREAD);

    options.c_cflag &= ~PARENB; //no parity
    options.c_cflag &= ~CSTOPB; //1 stop bit
    options.c_cflag &= ~CSIZE; //ready value for character size
    options.c_cflag |= CS8; //8-bit character size
    options.c_iflag &= ~IGNBRK; // disable break processing
    options.c_lflag = 0; // no signalling chars, no echo, no canonical processing
    options.c_oflag = 0; // no remapping, no delays
    options.c_cc[VMIN] = 0; // read doesn't block
    options.c_cc[VTIME] = 5; // 0.5 second read timeout
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // no xon/xoff ctrl
    options.c_cflag &= ~CRTSCTS;

    // Set the new options for the port...
    if ( 0 != tcsetattr(fd, TCSANOW, &options) ) { //change attributes NOW
        printf("SerialInit Error - unable to set options!\n");
        return false;
    }


    return true;
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
        printf("SerialClose: Error - %d,  %s\n", errno, strerror(errno));
    }
    else {
        printf("Serial connection closed.\n");
    }
}

/************************************ EOF ********************************/
