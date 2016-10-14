/*
*	blah blah blah.  Sockets are great
*
*	History:
*		November 16, 2010:  created
*/

#ifndef SOCKET_H
#define SOCKET_H

#include <stdint.h>
#include "stdtypes.h"

/* 	CreateCryptoHandle()
*  	input params:
*		uint8_t* pIV - 16-byte shared initialization vector
*		uint8_t* pKey - 16-byte secret pre-shared cryptographic key
*		
*	return CryptoHandle_T*
*		NULL if allocation failed
*/
typedef BOOL (*OpenAndConnect_T)(uint16_t port);

/* ConnectToServer
*	input params:
*		char* szIPAddress - ASCII string representation of the target IP address Ex: "127.0.0.1"
*		uint16_t portNum - port number to connect to
*
*	return BOOL
*		TRUE if connected
*		FALSE if failed to connect
*/
typedef BOOL (*ConnectToServer_T)(char* szIPAddress, uint16_t portNum);

/* Read()
*	returns the return value from recv()
*/
typedef uint32_t (*Read_T)( uint8_t* buffer, uint32_t bufferLen );

/* Write()
*	returns the return value from send()
*/
typedef uint32_t (*Write_T)( uint8_t* data, uint32_t len );

/* Close ()
*	closes the open socket connection
*/
typedef void (*Close_T)( void );

typedef struct _SocketInterface_T {
	OpenAndConnect_T OpenAndConnect;
	ConnectToServer_T ConnectToServer;
	Read_T Read;
	Write_T Write;
	Close_T Close;
} __attribute__((__packed__))SocketInterface_T;

#endif // SOCKET_H

