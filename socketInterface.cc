#include "socketInterface.h"

void SocketInterface::Start()
{
    t = new thread(&SocketInterface::StartServer, this);
}

void SocketInterface::Stop()
{
    socketIntf.Close();
    socketIntf.Close();
    running = false; t->join();
}

void SocketInterface::StartServer()
{
    cout << "socket -> Opening port " << port << endl;
    if ( socketIntf.OpenAndConnect(port) ) {
        //syslog(LOG_INFO, "Connected!");
        cout << "socket-> socket opened" << endl;
    } else {
        //syslog(LOG_INFO, "Failed to connect to socket - exit");
        cout << "socket -> failed to connect to socket - thread exitting" << endl;
        return;
    }

    while(running)
    {
        //syslog(LOG_INFO, "Client connected.");
        cout << "socket -> Client connected." << endl;
        HandleClient();
        //syslog(LOG_INFO, "Client disconnected.");
        cout << "socket -> Client disconnected." << endl;

        // Re-open port and wait for client to connect
        //syslog(LOG_INFO,"Attempting to connect on port %d...\n", portNum);
        cout << "socket -> attempting to reconnect on port " << port << endl;
        if ( socketIntf.OpenAndConnect(port) ) {
            cout << "socket -> connected!" << endl;
            //syslog(LOG_INFO, "Connected!");
        } else {
            //syslog(LOG_INFO, "Failed to connect to socket - exit");
            cout << "socket -> failed to connect to socket" << endl;
            return 0;
        }

    }

}

void SocketInterface::HandleClient( void )
{
    uint8_t socketBuf[SOCK_BUF_SIZE];
    int32_t cntSocketRx;

    while(true) {
        cntSocketRx = socketIntf.Read(socketBuf, SOCK_BUF_SIZE);
        if ( cntSocketRx > 0 ) {
            // TODO: do I need a bool return type?
            if ( InterpretSocketCommand(socketBuf, cntSocketRx) ) {
                pubCommManager.notify(&controlData);
                pubWatchdog.notify(); // reset watchdog
            }
            // send response back to client
            if ( false == status.commFault ) {
                snprintf((char*)socketBuf, SOCK_BUF_SIZE, "0x%x,%.3f,%.3f",
                  status.status, status.voltage, status.current);
            } else {
                snprintf((char*)socketBuf, SOCK_BUF_SIZE, "Board Fault");
            }
            socketIntf.Write(socketBuf, strlen((char*)socketBuf));
        } else {
            //syslog(LOG_INFO, "Client disconnected - resetting board\n");
            cout << "socket -> client disconnected" << endl;
            // TODO: publish a watchdog reset type message immediately
            //  to remove power and reset to safe values

            socketIntf.Close(); // close client connection
            socketIntf.Close(); // close socket // may not be the best idea, ok for now
            break;
        }
    } // end while
}

bool SocketInterface::InterpretSocketCommand(uint8_t *data, uint32_t length)
{
    char *strInput;
    char *ptr;

    // sanity check
    if ( data == NULL || length > MAX_SOCKET_MSG_LEN ) {
        //syslog(LOG_WARNING, "InterpretSocketCommand: failed input sanity check");
        cout << "socket -> InterpretSocketCommand: failed input sanity check" << endl;
        if (data == NULL) {
            //syslog(LOG_WARNING, "ISC()-->data = NULL, length: %d", length);
            cout << "socket -> InterpretSocketCommand() -> data = NULL, length = " <<
                length << endl;
        } else {
            //syslog(LOG_WARNING, "ISC()-->length = %d (max is %d)", length,\
              MAX_SOCKET_MSG_LEN);
            cout << "socket -> InterpretSocketCommand() -> length = " <<
                length << endl;
        }
        // bail
        return false;
    }
    // preserve original string and null terminate
    strInput = (char*)malloc(length+1);
    strncpy(strInput, (char*)data, length);
    strInput[length] = 0;

    // start string parsing from here:
    // candidate command strings:
    //  setservo [1,2,3,4,5,6,7,8] ####
    //  setmotor [1,2,3,4] ####
    //  setled ####
    //  mon, moff, con, coff
    // TODO: chain multiple commands together in a single message separated by
    //  ';', e.g., "mon; setservo 1 200; setservo2 750"
    //  Do this by placing this whole thing in a loop, strtok by ';' and replace
    //  return statements with continue statements

    if ( strcasestr(strInput, "setServo") != NULL ) {
        int servo;
        int position;
        int ret;

        ptr = strcasestr(strInput, "setServo") + strlen("setServo");

        // Grab arguments
        ret = sscanf(ptr, "%d %d", &servo, &position);
        if ( (ret != EOF) && (ret != 2) ) {
            // Error
            return false;
        } else {
            // Good - now sanity check
            if ( (API_SERVO_MIN > servo) || (servo > API_SERVO_MAX) ||
                 (API_SERVOPOS_MIN > position) || (position > API_SERVOPOS_MAX) )
            {
                return false;
            }
            switch (servo)
            {
                case 1:
                    controlData.servo1 = position;
                    break;
                case 2:
                    controlData.servo2 = position;
                    break;
                case 3:
                    controlData.servo3 = position;
                    break;
                case 4:
                    controlData.servo4 = position;
                    break;
                case 5:
                    controlData.servo5 = position;
                    break;
                case 6:
                    controlData.servo6 = position;
                    break;
                case 7:
                    controlData.servo7 = position;
                    break;
                case 8:
                    controlData.servo8 = position;
                    break;
                default:
                    return false;
            }
        }
    } else if ( strcasestr(strInput, "setMotor") != NULL ) {
        int motor;
        int power;
        int ret;

        ptr = strcasestr(strInput, "setMotor") + strlen("setMotor");

        // Grab arguments
        ret = sscanf(ptr, "%d %d", &motor, &power);
        if ( (ret != EOF) && (ret != 2) ) {
            // Error
            return false;
        } else {
            // Good - now sanity check
            if ( (API_MOTOR_MIN > motor) || (motor > API_MOTOR_MAX) ||
                (API_MOTORPOWER_MIN > power) || (power > API_MOTORPOWER_MAX) )
            {
                return false;
            }
            switch (motor)
            {
                case 1:
                    controlData.motorA = power;
                    break;
                case 2:
                    controlData.motorB = power;
                    break;
                case 3:
                    controlData.motorC = power;
                    break;
                case 4:
                    controlData.motorD = power;
                    break;
                default;
                    return false;
            }
        }
    } else if ( strcasestr(strInput, "setLED") != NULL ) {
        int power;
        int ret;

        ptr = strcasestr(strInput, "setLED") + strlen("setLED");

        // Grab arguments
        ret = sscanf(ptr, "%d", &power);
        if ( (ret != EOF) && (ret != 1) ) {
            return false;
        } else {
            // Good - now sanity check
            if ( (API_LEDPOWER_MIN > power) || (power > API_LEDPOWER_MAX) )
            {
                return false;
            }
            controlData.extLed = power;
        }
    } else if ( strcasestr(strInput, "CON") != NULL ) {
        controlData.hostPower = true;
    } else if ( strcasestr(strInput, "COFF") != NULL ) {
        controlData.hostPower = false;
    } else if ( strcasestr(strInput, "MON") != NULL ) {
        controlData.motorPower = true;
    } else if ( strcasestr(strInput, "MOFF") != NULL ) {
        controlData.motorPower = false;
    } else {
        return false;
    }
    return true;
}

