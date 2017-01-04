/************************************************************************/
/*                                                                      */
/*  setDefaults.c    --  Helper Config module                         */
/*                                                                      */
/************************************************************************/
/*  Author:     Mark Taylor                                             */
/*  Copyright 2017, Mark Taylor                                         */
/************************************************************************/
/*  Module Description:                                                 */
/*                                                                      */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  01/03/2017 (MarkT): created                                         */
/************************************************************************/


/* ------------------------------------------------------------ */
/*              Include File Definitions                        */
/* ------------------------------------------------------------ */
//#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>

#include "socketToSerial.h"

/* ------------------------------------------------------------ */
/*              Local Type Definitions                          */
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*              Global Variables                                */
/* ------------------------------------------------------------ */

// structure of saved default values
savedDefaults_T locDefaults;


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

void DisplaySettings ( void )
{
    //uint32_t temp;

    printf("DisplaySettings...\n");

}

void UserGetSetting ( uint8_t *dest, char *prompt )
{
    printf("Enter motorADefault: ");

}

void Usage ( char *prog )
{
    printf("USAGE: \n");
    printf("  %s \n", prog);
    printf("    Arguments:\n");
    printf("      none: Run through configurator to save a config file\n");
    printf("      \"show\": Display the contents of the existing config file\n");
}

int main ( int argc, char *argv[] )
{
    if ( argc == 2 )
    {
        if ( 0 == strcmp(argv[1], "show") ) {
            if ( false == LoadDefaults() ) {
                printf("ERROR reading config file\n");
                return -1;
            }
            DisplaySettings();
            return 0;
        } else {
            Usage(argv[0]);
            return 0;
        }
    } else if ( argc != 1 ) {
        Usage(argv[0]);
        return 0;
    }

    memset(&locDefaults, 0, sizeof(savedDefaults_T));

    if ( false == SaveDefaults()) {
        printf("ERROR: Couldn't save default file\n");
    } else {
        printf("Config file saved!\n");
    }


    return 0;
}


