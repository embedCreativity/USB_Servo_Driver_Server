#include "commManager.h"
#include "commManagerTypes.h"

void CommManager::Start()
{
    /* lastData is not initialized, and therefore will cause commManager to send controlData to the board,
       which will ensure that the board gets whatever the Init() data is. At that time, lastData is updated */
    controlData.Init();
    t = new thread(&CommManager::StartCommManager, this);
}

void CommManager::StartCommManager()
{
    cout << "Starting CommManager" << endl;

    while ( running )
    {
        // If there are updates to our position data, send them out
        if ( lastData != controlData ) {
            if (!lastData.diffTypePosition(&controlData) ) {
                if ((status.commFault = !( SendPositionData() ) )) {
                    cout << "commManager -> position data: commFault" << endl;
                } else {
                    pubBoardStatus.notify(&status);
                }
            }
            if (!lastData.diffTypePower(&controlData) ) {
                if ((status.commFault = !( SendPowerData() ) )) {
                    cout << "commManager -> powerdata: commFault" << endl;
                } else {
                    pubBoardStatus.notify(&status);
                }
            }
        } else { // No updates to our data. Send heartbeat?
            if ((status.commFault = !( SendHeartBeat() ) )) {
                cout << "commManager -> heartbeat: commFault" << endl;
            } else {
                pubBoardStatus.notify(&status);
            }
        }
        lastData = controlData; // update
        usleep(INTERCOMMAND_REST); // rest before sending traffic again (throttle)
    }

    SerialClose(); // close serial port before leaving
}

bool CommManager::SendPositionData(void)
{
    tlvPosition_T cfgPosition;

    cfgPosition.type = TYPE_LOC_UPDATE;
    cfgPosition.length = LENGTH_LOC_UPDATE;
    MapMotorValue(controlData.motorA, (uint8_t*)(&cfgPosition.motorA));
    MapMotorValue(controlData.motorB, (uint8_t*)(&cfgPosition.motorB));
    MapMotorValue(controlData.motorC, (uint8_t*)(&cfgPosition.motorC));
    MapMotorValue(controlData.motorD, (uint8_t*)(&cfgPosition.motorD));
    MapServoValue(controlData.servo1, (uint8_t*)(&cfgPosition.servo1));
    MapServoValue(controlData.servo2, (uint8_t*)(&cfgPosition.servo2));
    MapServoValue(controlData.servo3, (uint8_t*)(&cfgPosition.servo3));
    MapServoValue(controlData.servo4, (uint8_t*)(&cfgPosition.servo4));
    MapServoValue(controlData.servo5, (uint8_t*)(&cfgPosition.servo5));
    MapServoValue(controlData.servo6, (uint8_t*)(&cfgPosition.servo6));
    MapServoValue(controlData.servo7, (uint8_t*)(&cfgPosition.servo7));
    MapServoValue(controlData.servo8, (uint8_t*)(&cfgPosition.servo8));
    MapLedValue(controlData.extLed, (uint8_t*)(&cfgPosition.extLed));
    cfgPosition.checksum = ComputeChecksum((uint8_t*)(&cfgPosition.motorA), cfgPosition.length);

    return SendCommand((uint8_t*)(&cfgPosition), cfgPosition.length);
}

bool CommManager::SendPowerData(void)
{
    tlvPowerManagement_T cfgPower;

    cfgPower.type = TYPE_PWR_UPDATE;
    cfgPower.length = LENGTH_PWR_UPDATE;
    cfgPower.config = 0; // init
    if ( controlData.motorPower ) {
        cfgPower.config |= (1 << BS_PWR_M);
    }
    if ( controlData.hostPower ) {
        cfgPower.config |= (1 << BS_PWR_C);
    }
    cfgPower.checksum = ComputeChecksum((uint8_t*)(&cfgPower.config),
        cfgPower.length);

    return SendCommand((uint8_t*)(&cfgPower), cfgPower.length);
}

// TODO: create a heartbeat TLV between board and host
bool CommManager::SendHeartBeat(void)
{
    tlvPowerManagement_T cfgPower;

    cfgPower.type = TYPE_PWR_UPDATE;
    cfgPower.length = LENGTH_PWR_UPDATE;
    cfgPower.config = 0; // init
    if ( controlData.motorPower ) {
        cfgPower.config |= (1 << BS_PWR_M);
    }
    if ( controlData.hostPower ) {
        cfgPower.config |= (1 << BS_PWR_C);
    }
    cfgPower.checksum = ComputeChecksum((uint8_t*)(&cfgPower.config),
        cfgPower.length);

    return SendCommand((uint8_t*)(&cfgPower), cfgPower.length);
}

bool CommManager::SendCommand( uint8_t *data, uint8_t length )
{
    uint8_t attempt;
    bool fault;

    for ( attempt = 0, fault = true; attempt < MAX_SERIAL_ATTEMPTS; attempt++ ) {
        if ( ! SerialWriteNBytes(data, length) ) {
            usleep(SERIAL_RETRY_DELAY);
            continue;
        }

        // Give board a moment to respond
        usleep(SERIAL_READ_DELAY);

        if ( SerialGetResponse(SERIAL_TIMEOUT) ) {
            fault = false;
            break;
        }
    }

    if ( fault ) {
        status.commFault = true;
    } else {
        status.commFault = false;
    }

    return !status.commFault;
}

bool CommManager::SerialGetResponse( uint32_t timeout )
{
    int serialRxLen;
    uint8_t serialRxData[64];

    tlvAck_T tlvAck;

    uint8_t offset; // offset into compiled buffer
    uint8_t i; // pointer in read data
    uint8_t *ptr;
    uint16_t shortADCVoltage;
    uint16_t shortADCCurrent;

    // init locals
    offset = POS_TLV_TYPE;

    // Compile tlv message
    while ( true ) {
        i = 0; // reset read buffer pointer
        // read enough data to fill tlv struct
        serialRxLen = SerialRead((uint8_t*)(&(serialRxData)),
            (uint32_t)(sizeof(tlvAck_T) - offset), timeout);
        if ( serialRxLen == SERIAL_ERROR_CODE ) {
            cout << "commManager-> serial error" << endl;
            return false;
        } else if ( serialRxLen == SERIAL_TIMEOUT_CODE ) {
            cout << "commManager-> serial timeout" << endl;
            return false;
        } else if ( serialRxLen == 0 ) { // just start over...
            continue;
        }

        if ( POS_TLV_TYPE == offset ) { // look for type code
            // look for type code throwing away garbage before it
            for ( ; (i < serialRxLen); i++ ) {
                if ( serialRxData[i] == TYPE_ACK ) {
                    i += 1;
                    offset = POS_TLV_LENGTH; // mark next thing to look for
                    break;
                }
            }
            if ( POS_TLV_TYPE == offset ) { // didn't find type code - fail
                continue;
            }
        }
        // Verify length
        if ( POS_TLV_LENGTH == offset ) {
            if ( i < serialRxLen ) {
                if ( LENGTH_ACK == serialRxData[i] ) {
                    i += 1; // increment pointer in read buffer
                    offset = TLV_OVERHEAD - 1; // mark next thing to look for
                } else { // fail - corrupt data
                    offset = 0;
                    continue;
                }
            } else { // no more data, go around
                continue;
            }
        }

        // Data bytes
         if ( ((TLV_OVERHEAD - 1) <= offset ) && (offset < (LENGTH_ACK + 2)) )
        {
            ptr = (uint8_t*)&tlvAck;
            for ( ; (i < serialRxLen) && (offset < LENGTH_ACK + 2); i++ ) {
                ptr[offset] = serialRxData[i]; // copy byte into struct
                offset += 1; // increment pointer into struct
            }
        }

        // checksum byte
        if ( (LENGTH_ACK + 2 ) == offset ) {
            if ( i < serialRxLen ) {
                if ( serialRxData[i] != ComputeChecksum(&tlvAck.status,
                  tlvAck.length) )
                { // fail - corrupt data
                    offset = 0;
                    continue;
                }
                else { // Checksum verified!
                    status.status = tlvAck.status;
                    shortADCVoltage = tlvAck.adcVoltage;
                    shortADCCurrent = tlvAck.adcCurrent;
                    status.voltage = (shortADCVoltage / 4096.0) * 19.8;
                    status.current = shortADCCurrent * (55.0/4096.0);
                    return true;
                }
            } else { // no more data; go around
                continue;
            }
        }
    }
}


uint8_t CommManager::ComputeChecksum(uint8_t *input, uint32_t length)
{
    uint8_t i;
    uint8_t checksum;
    checksum = 0;

    for ( i = 0; i < length; i++ ) {
        checksum = checksum ^ input[i];
    }

    return checksum;
}

void CommManager::MapMotorValue(int32_t power, uint8_t *ptr)
{
    bool negative = false;

    if ( power < 0 ) {
       negative = true;
       power = abs(power);
    } else {
        power = MOTOR_REFRESH_PERIOD - MOTOR_MIN_SPIN - (power * API_MOTORPOWER_SCALAR);
    }
    if ( negative ) {
        // we set the most significant bit to indicate a backward direction
        power |= (1 << 23);
    }
    memcpy(ptr, &power, 3);
}

void CommManager::MapServoValue(uint32_t position, uint8_t *ptr)
{
    // map API range to board's expected range
    position = SERVO_MIN_PERIOD + (position * API_SERVOPOS_SCALAR);
    // save into structure
    memcpy(ptr, &position, 3);
}

void CommManager::MapLedValue(uint32_t power, uint8_t *ptr)
{
    // TODO: I think this is FUBAR
    power = power * API_LEDPOWER_SCALAR;
    if ( power == EXTLED_REFRESH_PERIOD ) {
        power -= 1;
    }
    memcpy(ptr, &power, 3);
}

bool CommManager::InitComms(char *port, int baudRate)
{
    return SerialInit(port, baudRate);
}



