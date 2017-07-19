#include "watchDog.h"

void Watchdog::SetControlSafeValues(void)
{
    uint32_t safeMotor = SAFE_MOTOR;
    uint32_t safeServo = SAFE_SERVO;
    uint32_t safeLED = SAFE_LED;

    memcpy(&controlData.motorA, &safeMotor, 3);
    memcpy(&controlData.motorB, &safeMotor, 3);
    memcpy(&controlData.motorC, &safeMotor, 3);
    memcpy(&controlData.motorD, &safeMotor, 3);
    memcpy(&controlData.servo1, &safeServo, 3);
    memcpy(&controlData.servo2, &safeServo, 3);
    memcpy(&controlData.servo3, &safeServo, 3);
    memcpy(&controlData.servo4, &safeServo, 3);
    memcpy(&controlData.servo5, &safeServo, 3);
    memcpy(&controlData.servo6, &safeServo, 3);
    memcpy(&controlData.servo7, &safeServo, 3);
    memcpy(&controlData.servo8, &safeServo, 3);
    memcpy(&controlData.extLed, &safeLED, 3);
}

void Watchdog::Start(void)
{
    t = new thread(&Watchdog::StartWatchdog, this);
}

void Watchdog::StartWatchdog(void)
{
    cout << "Starting WatchDog" << endl;

    while(running)
    {
        // do something
        usleep(1000); // sleep for 1 ms
        if ( watchdogCount > WATCHDOG_TIMEOUT ) {
            pubCommManager.notify(&controlData);
            cout << "Timeout!" << endl;
            cout << "watchDog: " << watchdogCount << endl;
            running = false;
        } else {
            watchdogCount += 1;
        }
    }
}

#ifdef DEAD
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
    uint8_t socketBuf[SOCK_BUF_SIZE];
    int32_t cntSocketRx;
    TLV_TYPE type;

    #if defined(SQL_POWER)
    // Start periodic update of power logging
    if (pthread_create(&threadUpdateSql, NULL, &SqlLogging, NULL)) {
        syslog(LOG_PERROR, "ERROR;  pthread_create(&threadUpdateSql... \n");
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
        cntSocketRx = socketIntf.Read(socketBuf, SOCK_BUF_SIZE);
        if ( cntSocketRx > 0 ) {
            type = InterpretSocketCommand(socketBuf, cntSocketRx);
            // transmit the packet to the board
            if ( BoardComm(type) ) {
                snprintf((char*)socketBuf, SOCK_BUF_SIZE, "0x%x,0x%x,0x%x",
                  bBoardStatus, shortADCVoltage, shortADCCurrent);

            #if defined(SQL_POWER)
                // Put ADC Reading into the Sqlite database for the webpage
                // Must convert from the Palmetto Board's ADC result to a voltage.
                // ADC has a sample range of 4096 with 19.8 representing the maximum voltage
                // Current has a maximum of 55.0Amps represented by a linear voltage from the
                // current sense amplifier.
                voltage = (shortADCVoltage / 4096.0) * 19.8;
                current = shortADCCurrent * (55.0/4096.0);
            #endif
            } else {
                snprintf((char*)socketBuf, SOCK_BUF_SIZE, "BoardComm Error");
            }
            // Push serial response back to client over the socket
            socketIntf.Write(socketBuf, strlen((char*)socketBuf));
        } else {
            // restore defaults
            SetDefaults();
            syslog(LOG_INFO, "Client disconnected - resetting board\n");
            // reset position data
            BoardComm(TLV_POSITION);
            usleep(RESET_DELAY);
            // remove power
            BoardComm(TLV_POWER);
            socketIntf.Close(); // close client connection
            socketIntf.Close(); // close socket // may not be the best idea, ok for now
            SerialClose();
            break;
        }
    }// end while
    /********************************************************************/
} //end HandleClient()



TLV_TYPE InterpretSocketCommand(uint8_t *data, uint32_t length)
{
    char *strInput;
    char *ptr;
    TLV_TYPE type;

    // init to invalid
    type = TLV_ERROR;

    // sanity check
    if ( data == NULL || length > MAX_SOCKET_MSG_LEN ) {
        syslog(LOG_WARNING, "InterpretSocketCommand: failed input sanity check");
        if (data == NULL) {
            syslog(LOG_WARNING, "ISC()-->data = NULL, length: %d", length);
        } else {
            syslog(LOG_WARNING, "ISC()-->length = %d (max is %d)", length,\
              MAX_SOCKET_MSG_LEN);
        }
        return TLV_ERROR;
    }
    // preserve original string and null terminate
    strInput = (char*)malloc(length+1);
    strncpy(strInput, (char*)data, length);
    strInput[length] = 0;

    // start string parsing from here:
    // candidate command strings:
    //  Go, stop, back, pivot right, pivot left, turn right, turn left
    //  set servo[1,2,3,4,5,6,7,8] ####
    //  set motor[A,B,C,D] ####
    // It'd be cool to chain these together so they arrive all at the same time
    //  set motorA 500; set motorB 500;

    if ( strcasestr(strInput, "setServo") != NULL ) {
        int servo;
        int position;
        int ret;

        ptr = strcasestr(strInput, "setServo") + strlen("setServo");

        // Grab arguments
        ret = sscanf(ptr, "%d %d", &servo, &position);
        if ( (ret != EOF) && (ret != 2) ) {
            // Error
            syslog(LOG_PERROR, "FORMAT ERROR [setServo] \
              --> usage: setServo [servo] [position]. Rec'd: \"%s\"", ptr);
            return TLV_ERROR;
        } else {
            bool pass = true;
            // Good - now sanity check
            if ( (API_SERVO_MIN > servo) || (servo > API_SERVO_MAX) )
            {
                syslog(LOG_PERROR, "FORMAT ERROR [setServo] --> servo range \
                  [%d to %d]. Rec'd: %d", API_SERVO_MIN, API_SERVO_MAX,
                  servo);
                pass = false;
                return TLV_ERROR;
            }
            if ( (API_SERVOPOS_MIN > position) || (position > API_SERVOPOS_MAX) )
            {
                syslog(LOG_PERROR, "FORMAT ERROR [setServo] --> position range \
                  [%d to %d]. Rec'd: %d", API_SERVOPOS_MIN, API_SERVOPOS_MAX,
                  position);
                pass = false;
                return TLV_ERROR;
            }
            if ( pass )
            { // Sanity check - passed
                SetServo (servo, position);
            }
        }
        type = TLV_POSITION;
    } else if ( strcasestr(strInput, "setMotor") != NULL ) {
        int motor;
        int power;
        int ret;

        ptr = strcasestr(strInput, "setMotor") + strlen("setMotor");

        // Grab arguments
        ret = sscanf(ptr, "%d %d", &motor, &power);
        if ( (ret != EOF) && (ret != 2) ) {
            // Error
            syslog(LOG_PERROR, "FORMAT ERROR [setMotor] \
              --> usage: setMotor [motor] [power]. Rec'd: \"%s\"", ptr);
            return TLV_ERROR;
        } else {
            bool pass = true;
            // Good - now sanity check
            if ( (API_MOTOR_MIN > motor) || (motor > API_MOTOR_MAX) )
            {
                syslog(LOG_PERROR, "FORMAT ERROR [setMotor] --> motor range \
                  [%d to %d]. Rec'd: %d", API_MOTOR_MIN, API_MOTOR_MAX,
                  motor);
                pass = false;
                return TLV_ERROR;
            }
            if ( (API_MOTORPOWER_MIN > power) || (power > API_MOTORPOWER_MAX) )
            {
                syslog(LOG_PERROR, "FORMAT ERROR [setMotor] --> power range \
                  [%d to %d]. Rec'd: %d", API_MOTORPOWER_MIN, API_MOTORPOWER_MAX,
                  power);
                pass = false;
                return TLV_ERROR;
            }
            if ( pass )
            { // Sanity check - passed
                SetMotor(motor, power);
            }
        }
        type = TLV_POSITION;
    } else if ( strcasestr(strInput, "setLED") != NULL ) {
        int power;
        int ret;

        ptr = strcasestr(strInput, "setLED") + strlen("setLED");

        // Grab arguments
        ret = sscanf(ptr, "%d", &power);
        if ( (ret != EOF) && (ret != 1) ) {
            // Error
            syslog(LOG_PERROR, "FORMAT ERROR [setLED] \
              --> usage: setLED [power]. Rec'd: \"%s\"", ptr);
            return TLV_ERROR;
        } else {
            bool pass = true;
            // Good - now sanity check
            if ( (API_LEDPOWER_MIN > power) || (power > API_LEDPOWER_MAX) )
            {
                syslog(LOG_PERROR, "FORMAT ERROR [setLED] --> power range \
                  [%d to %d]. Rec'd: %d", API_LEDPOWER_MIN, API_LEDPOWER_MAX,
                  power);
                pass = false;
                return TLV_ERROR;
            }
            if ( pass )
            { // Sanity check - passed
                SetLED(power);
            }
        }
        type = TLV_POSITION;
    } else if ( strcasestr(strInput, "Go") != NULL ) {
        syslog(LOG_INFO, "Go!");
        memcpy(cfgPosition.motorA, cfgDefaultPosition.motorAGo, 3);
        memcpy(cfgPosition.motorB, cfgDefaultPosition.motorBGo, 3);
        memcpy(cfgPosition.motorC, cfgDefaultPosition.motorCGo, 3);
        memcpy(cfgPosition.motorD, cfgDefaultPosition.motorDGo, 3);
        type = TLV_POSITION;
    } else if ( strcasestr(strInput, "Stop") != NULL ) {
        memcpy(cfgPosition.motorA, cfgDefaultPosition.motorAStop, 3);
        memcpy(cfgPosition.motorB, cfgDefaultPosition.motorBStop, 3);
        memcpy(cfgPosition.motorC, cfgDefaultPosition.motorCStop, 3);
        memcpy(cfgPosition.motorD, cfgDefaultPosition.motorDStop, 3);
        type = TLV_POSITION;
    } else if ( strcasestr(strInput, "Back") != NULL ) {
        memcpy(cfgPosition.motorA, cfgDefaultPosition.motorABack, 3);
        memcpy(cfgPosition.motorB, cfgDefaultPosition.motorBBack, 3);
        memcpy(cfgPosition.motorC, cfgDefaultPosition.motorCBack, 3);
        memcpy(cfgPosition.motorD, cfgDefaultPosition.motorDBack, 3);
        type = TLV_POSITION;
    } else if ( strcasestr(strInput, "PivotRight") != NULL ) {
        memcpy(cfgPosition.motorA, cfgDefaultPosition.motorAPivotRight, 3);
        memcpy(cfgPosition.motorB, cfgDefaultPosition.motorBPivotRight, 3);
        memcpy(cfgPosition.motorC, cfgDefaultPosition.motorCPivotRight, 3);
        memcpy(cfgPosition.motorD, cfgDefaultPosition.motorDPivotRight, 3);
        type = TLV_POSITION;
    } else if ( strcasestr(strInput, "PivotLeft") != NULL ) {
        memcpy(cfgPosition.motorA, cfgDefaultPosition.motorAPivotLeft, 3);
        memcpy(cfgPosition.motorB, cfgDefaultPosition.motorBPivotLeft, 3);
        memcpy(cfgPosition.motorC, cfgDefaultPosition.motorCPivotLeft, 3);
        memcpy(cfgPosition.motorD, cfgDefaultPosition.motorDPivotLeft, 3);
        type = TLV_POSITION;
    } else if ( strcasestr(strInput, "TurnRight") != NULL ) {
        memcpy(cfgPosition.motorA, cfgDefaultPosition.motorATurnRight, 3);
        memcpy(cfgPosition.motorB, cfgDefaultPosition.motorBTurnRight, 3);
        memcpy(cfgPosition.motorC, cfgDefaultPosition.motorCTurnRight, 3);
        memcpy(cfgPosition.motorD, cfgDefaultPosition.motorDTurnRight, 3);
        type = TLV_POSITION;
    } else if ( strcasestr(strInput, "TurnLeft") != NULL ) {
        memcpy(cfgPosition.motorA, cfgDefaultPosition.motorATurnLeft, 3);
        memcpy(cfgPosition.motorB, cfgDefaultPosition.motorBTurnLeft, 3);
        memcpy(cfgPosition.motorC, cfgDefaultPosition.motorCTurnLeft, 3);
        memcpy(cfgPosition.motorD, cfgDefaultPosition.motorDTurnLeft, 3);
        type = TLV_POSITION;
    } else if ( strcasestr(strInput, "CON") != NULL ) {
        cfgPower.config |= (1 << BS_PWR_C);
        type = TLV_POWER;
    } else if ( strcasestr(strInput, "COFF") != NULL ) {
        cfgPower.config &= ~(1 << BS_PWR_C);
        type = TLV_POWER;
    } else if ( strcasestr(strInput, "MON") != NULL ) {
        cfgPower.config |= (1 << BS_PWR_M);
        type = TLV_POWER;
    } else if ( strcasestr(strInput, "MOFF") != NULL ) {
        cfgPower.config &= ~(1 << BS_PWR_M);
        type = TLV_POWER;
    } else {
        type = TLV_ERROR;
    }

    // Update checksum
    if ( type == TLV_POSITION ) {
        cfgPosition.checksum = ComputeChecksum((uint8_t*)(&cfgPosition.motorA),
            cfgPosition.length);
    } else if ( type == TLV_POWER ) {
        cfgPower.checksum = ComputeChecksum((uint8_t*)(&cfgPower.config),
            cfgPower.length);
    }
    return type;
}

void SetServo ( uint8_t servo, uint32_t position )
{
    uint32_t * ptrMotor;
    uint32_t pos;

    switch (servo)
    {
        case 1:
            ptrMotor = (uint32_t*)&cfgPosition.servo1;
            break;
        case 2:
            ptrMotor = (uint32_t*)&cfgPosition.servo2;
            break;
        case 3:
            ptrMotor = (uint32_t*)&cfgPosition.servo3;
            break;
        case 4:
            ptrMotor = (uint32_t*)&cfgPosition.servo4;
            break;
        case 5:
            ptrMotor = (uint32_t*)&cfgPosition.servo5;
            break;
        case 6:
            ptrMotor = (uint32_t*)&cfgPosition.servo6;
            break;
        case 7:
            ptrMotor = (uint32_t*)&cfgPosition.servo7;
            break;
        case 8:
            ptrMotor = (uint32_t*)&cfgPosition.servo8;
            break;
        default: // won't happen
            break;
    }
    // map API range to board's expected range
    pos = SERVO_MIN_PERIOD + (position * API_SERVOPOS_SCALAR);
    // save into structure
    memcpy(ptrMotor, &pos, 3);
}

// motor: 1->4, power: -1000->+1000
void SetMotor ( uint8_t motor, int32_t power )
{
    uint32_t * ptrMotor;
    bool negative = false;

    switch (motor)
    {
        case 1:
            ptrMotor = (uint32_t*)&cfgPosition.motorA;
            break;
        case 2:
            ptrMotor = (uint32_t*)&cfgPosition.motorB;
            break;
        case 3:
            ptrMotor = (uint32_t*)&cfgPosition.motorC;
            break;
        case 4:
            ptrMotor = (uint32_t*)&cfgPosition.motorD;
            break;
        default: // won't happen
            break;
    }
    // map API range to board's expected range
    //printf("motor[%d], orig[%d]\t", motor, power);
    if ( power < 0 ) {
        // We cheat by keeping the value positive for the board's power setting
        // But we mark the GPIO direction via the MSb.
        power = abs(power);
        negative = true;
        //printf("negative!\t");
    }
    if ( power == 0 ) {
        power = MOTOR_REFRESH_PERIOD - 1; // off
    } else {
        power = MOTOR_REFRESH_PERIOD - MOTOR_MIN_SPIN - (power * API_MOTORPOWER_SCALAR);
    }
    if ( negative ) {
        // we set the most significant bit to indicate a backward direction
        power |= (1 << 23);
    }
    //printf("motor[%d], adj[0x%x]\n", motor, power);
    memcpy(ptrMotor, &power, 3);
}

void SetLED ( uint32_t power )
{
    power = power * API_LEDPOWER_SCALAR;
    if ( power == EXTLED_REFRESH_PERIOD ) {
        power -= 1;
    }
    memcpy(cfgPosition.extLed, &power, 3);
}
#endif
