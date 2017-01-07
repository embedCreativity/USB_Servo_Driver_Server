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
    uint32_t temp;
    temp = 0;

    printf("DisplaySettings...\n");

    memcpy(&temp, &locDefaults.motorADefault, 3);
    printf("motorADefault: %d\n", temp);
    memcpy(&temp, &locDefaults.motorBDefault, 3);
    printf("motorBDefault: %d\n", temp);
    memcpy(&temp, &locDefaults.motorCDefault, 3);
    printf("motorCDefault: %d\n", temp);
    memcpy(&temp, &locDefaults.motorDDefault, 3);
    printf("motorDDefault: %d\n", temp);

    memcpy(&temp, &locDefaults.servo1Default, 3);
    printf("servo1Default: %d\n", temp);
    memcpy(&temp, &locDefaults.servo2Default, 3);
    printf("servo2Default: %d\n", temp);
    memcpy(&temp, &locDefaults.servo3Default, 3);
    printf("servo3Default: %d\n", temp);
    memcpy(&temp, &locDefaults.servo4Default, 3);
    printf("servo4Default: %d\n", temp);
    memcpy(&temp, &locDefaults.servo5Default, 3);
    printf("servo5Default: %d\n", temp);
    memcpy(&temp, &locDefaults.servo6Default, 3);
    printf("servo6Default: %d\n", temp);
    memcpy(&temp, &locDefaults.servo7Default, 3);
    printf("servo7Default: %d\n", temp);
    memcpy(&temp, &locDefaults.servo8Default, 3);
    printf("servo8Default: %d\n", temp);

    memcpy(&temp, &locDefaults.extLedDefault, 3);
    printf("extLedDefault: %d\n", temp);

    memcpy(&temp, &locDefaults.motorAGo, 3);
    printf("motorAGo: %d\n", temp);
    memcpy(&temp, &locDefaults.motorBGo, 3);
    printf("motorBGo: %d\n", temp);
    memcpy(&temp, &locDefaults.motorCGo, 3);
    printf("motorCGo: %d\n", temp);
    memcpy(&temp, &locDefaults.motorDGo, 3);
    printf("motorDGo: %d\n", temp);

    memcpy(&temp, &locDefaults.motorABack, 3);
    printf("motorABack: %d\n", temp);
    memcpy(&temp, &locDefaults.motorBBack, 3);
    printf("motorBBack: %d\n", temp);
    memcpy(&temp, &locDefaults.motorCBack, 3);
    printf("motorCBack: %d\n", temp);
    memcpy(&temp, &locDefaults.motorDBack, 3);
    printf("motorDBack: %d\n", temp);

    memcpy(&temp, &locDefaults.motorABack, 3);
    printf("motorABack: %d\n", temp);
    memcpy(&temp, &locDefaults.motorBBack, 3);
    printf("motorBBack: %d\n", temp);
    memcpy(&temp, &locDefaults.motorCBack, 3);
    printf("motorCBack: %d\n", temp);
    memcpy(&temp, &locDefaults.motorDBack, 3);
    printf("motorDBack: %d\n", temp);

    memcpy(&temp, &locDefaults.motorAStop, 3);
    printf("motorAStop: %d\n", temp);
    memcpy(&temp, &locDefaults.motorBStop, 3);
    printf("motorBStop: %d\n", temp);
    memcpy(&temp, &locDefaults.motorCStop, 3);
    printf("motorCStop: %d\n", temp);
    memcpy(&temp, &locDefaults.motorDStop, 3);
    printf("motorDStop: %d\n", temp);

    memcpy(&temp, &locDefaults.motorAPivotRight, 3);
    printf("motorAPivotRight: %d\n", temp);
    memcpy(&temp, &locDefaults.motorBPivotRight, 3);
    printf("motorBPivotRight: %d\n", temp);
    memcpy(&temp, &locDefaults.motorCPivotRight, 3);
    printf("motorCPivotRight: %d\n", temp);
    memcpy(&temp, &locDefaults.motorDPivotRight, 3);
    printf("motorDPivotRight: %d\n", temp);

    memcpy(&temp, &locDefaults.motorAPivotLeft, 3);
    printf("motorAPivotLeft: %d\n", temp);
    memcpy(&temp, &locDefaults.motorBPivotLeft, 3);
    printf("motorBPivotLeft: %d\n", temp);
    memcpy(&temp, &locDefaults.motorCPivotLeft, 3);
    printf("motorCPivotLeft: %d\n", temp);
    memcpy(&temp, &locDefaults.motorDPivotLeft, 3);
    printf("motorDPivotLeft: %d\n", temp);

    memcpy(&temp, &locDefaults.motorATurnRight, 3);
    printf("motorATurnRight: %d\n", temp);
    memcpy(&temp, &locDefaults.motorBTurnRight, 3);
    printf("motorBTurnRight: %d\n", temp);
    memcpy(&temp, &locDefaults.motorCTurnRight, 3);
    printf("motorCTurnRight: %d\n", temp);
    memcpy(&temp, &locDefaults.motorDTurnRight, 3);
    printf("motorDTurnRight: %d\n", temp);

    memcpy(&temp, &locDefaults.motorATurnLeft, 3);
    printf("motorATurnLeft: %d\n", temp);
    memcpy(&temp, &locDefaults.motorBTurnLeft, 3);
    printf("motorBTurnLeft: %d\n", temp);
    memcpy(&temp, &locDefaults.motorCTurnLeft, 3);
    printf("motorCTurnLeft: %d\n", temp);
    memcpy(&temp, &locDefaults.motorDTurnLeft, 3);
    printf("motorDTurnLeft: %d\n", temp);
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
    uint32_t input;
    int ret;

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

    /************************************************/
    /* TODO: make this suck less                    */
    /************************************************/
    printf("motorADefault: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorADefault, &input, 3);

    printf("motorBDefault: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorBDefault, &input, 3);

    printf("motorCDefault: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorCDefault, &input, 3);

    printf("motorDDefault: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorDDefault, &input, 3);

    printf("servo1Default: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.servo1Default, &input, 3);

    printf("servo2Default: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.servo2Default, &input, 3);

    printf("servo3Default: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.servo3Default, &input, 3);

    printf("servo4Default: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.servo4Default, &input, 3);

    printf("servo5Default: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.servo5Default, &input, 3);

    printf("servo6Default: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.servo6Default, &input, 3);

    printf("servo7Default: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.servo7Default, &input, 3);

    printf("servo8Default: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.servo8Default, &input, 3);

    printf("extLedDefault: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.extLedDefault, &input, 3);

    printf("motorAGo: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorAGo, &input, 3);

    printf("motorBGo: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorBGo, &input, 3);

    printf("motorCGo: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorCGo, &input, 3);

    printf("motorDGo: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorDGo, &input, 3);

    printf("motorABack: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorABack, &input, 3);

    printf("motorBBack: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorBBack, &input, 3);

    printf("motorCBack: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorCBack, &input, 3);

    printf("motorDBack: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorDBack, &input, 3);

    printf("motorAStop: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorAStop, &input, 3);

    printf("motorBStop: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorBStop, &input, 3);

    printf("motorCStop: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorCStop, &input, 3);

    printf("motorDStop: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorDStop, &input, 3);

    printf("motorAPivotRight: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorAPivotRight, &input, 3);

    printf("motorBPivotRight: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorBPivotRight, &input, 3);

    printf("motorCPivotRight: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorCPivotRight, &input, 3);

    printf("motorDPivotRight: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorDPivotRight, &input, 3);

    printf("motorAPivotLeft: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorAPivotLeft, &input, 3);

    printf("motorBPivotLeft: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorBPivotLeft, &input, 3);

    printf("motorCPivotLeft: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorCPivotLeft, &input, 3);

    printf("motorDPivotLeft: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorDPivotLeft, &input, 3);

    printf("motorATurnRight: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorATurnRight, &input, 3);

    printf("motorBTurnRight: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorBTurnRight, &input, 3);

    printf("motorCTurnRight: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorCTurnRight, &input, 3);

    printf("motorDTurnRight: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorDTurnRight, &input, 3);

    printf("motorATurnLeft: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorATurnLeft, &input, 3);

    printf("motorBTurnLeft: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorBTurnLeft, &input, 3);

    printf("motorCTurnLeft: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorCTurnLeft, &input, 3);

    printf("motorDTurnLeft: ");
    ret = scanf("%d", &input);
    // range check
    if ( (input > 0xFFFFFF) || (ret == EOF) || (ret == 0) ) {
        printf("ERROR: invalid input\n");
        return -1;
    }
    memcpy(&locDefaults.motorDTurnLeft, &input, 3);

    /************************************************/

    if ( false == SaveDefaults()) {
        printf("ERROR: Couldn't save default file\n");
    } else {
        printf("Config file saved!\n");
    }


    return 0;
}


