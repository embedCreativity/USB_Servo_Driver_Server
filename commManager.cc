#include "commManager.h"

typedef struct _tlvPosition_T {
    uint8_t type;
    uint8_t length;
    uint8_t motorA[3];
    uint8_t motorB[3];
    uint8_t motorC[3];
    uint8_t motorD[3];
    uint8_t servo1[3];
    uint8_t servo2[3];
    uint8_t servo3[3];
    uint8_t servo4[3];
    uint8_t servo5[3];
    uint8_t servo6[3];
    uint8_t servo7[3];
    uint8_t servo8[3];
    uint8_t extLed[3];
    uint8_t checksum;
} __attribute__ ((__packed__)) tlvPosition_T;

typedef struct _tlvPowerManagement_T {
    uint8_t type;
    uint8_t length;
    uint8_t config;
    uint8_t checksum;
} __attribute__ ((__packed__)) tlvPowerManagement_T;

void CommManager::Start()
{
    t = new thread(&CommManager::StartCommManager, this);
}

void CommManager::StartCommManager()
{
    uint8_t *data;
    uint8_t length;

    cout << "Starting CommManager" << endl;

    while ( running )
    {
        /* Look at stored command data. If it is different than
            public command data, determine which kind of TLV is required
            to address it :
                tlvPosition_T cfgPosition;
                tlvPowerManagement_T cfgPower;

                // struct for holding data,len returned from SerialRead
                serialRx_T serialRxData;
                tlvAck_T tlvAck = { .type = TYPE_ACK, .length = LENGTH_ACK, .status = 0,
                  .adcVoltage= 0, .adcCurrent = 0, .checksum = 0 };
                  uint16_t shortADCVoltage;
                  uint16_t shortADCCurrent;
                  uint8_t bBoardStatus;
        */
        if ( lastData != controlData ) {
            if (!lastData.diffTypePosition(&controlData) ) {
                // Send Position TLV
                tlvPosition_T cfgPosition;
                cfgPosition.type = TYPE_LOC_UPDATE;
                cfgPosition.length = LENGTH_LOC_UPDATE;
                MapMotorValue(controlData.motorA, &cfgPosition.motorA);
                MapMotorValue(controlData.motorB, &cfgPosition.motorB);
                MapMotorValue(controlData.motorC, &cfgPosition.motorC);
                MapMotorValue(controlData.motorD, &cfgPosition.motorD);
                MapServoValue(controlData.servo1, &cfgPosition.servo1);
                MapServoValue(controlData.servo2, &cfgPosition.servo2);
                MapServoValue(controlData.servo3, &cfgPosition.servo3);
                MapServoValue(controlData.servo4, &cfgPosition.servo4);
                MapServoValue(controlData.servo5, &cfgPosition.servo5);
                MapServoValue(controlData.servo6, &cfgPosition.servo6);
                MapServoValue(controlData.servo7, &cfgPosition.servo7);
                MapServoValue(controlData.servo8, &cfgPosition.servo8);
                MapLedValue(controlData.extLed, &cfgPosition.extLed);
                cfgPosition.checksum = ComputeChecksum((uint8_t*)(&cfgPosition.motorA), cfgPosition.length);
                data = &cfgPosition;
                length = cfgPosition.length;
            }
            if (!lastData.diffTypePower(&controlData) ) {
                // Send Power TLV
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
                cfgPower.checksum = ComputeChecksum((uint8_t*)(&cfgPower.config), cfgPower.length);
                data = &cfgPower;
                length = cfgPower.length;
            }
        } else { // No updates to our data. Send heartbeat?

        }

        // Translate command values to board TLV type data

        // Send command data




        // Get response

        /* If response bad, send command again. Do no more than 3 times before
            declaring fault
        */

        // Publish


        usleep(500000);
        cout << "commMgr V before " << status.voltage << endl;
        status.voltage = status.voltage - 1.0;
        cout << "commMgr V after " << status.voltage << endl;
        cout << "commManager[motorA]: " <<
            unsigned(controlData.motorA) << endl;
        pubBoardStatus.notify(&status);
    }
}

uint8_t CommManager::ComputeChecksum(uint8_t *input, uint32_t length)
{
    int i;
    uint8_t checksum;
    checksum = 0;

    for ( i = 0; i < length; i++ ) {
        checksum = checksum ^ input[i];
    }

    return checksum;
}

void CommManager:MapMotorValue(int32_t power, uint8_t *ptr)
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

void CommManager:MapServoValue(uint32_t position, uint8_t *ptr)
{
    // map API range to board's expected range
    position = SERVO_MIN_PERIOD + (position * API_SERVOPOS_SCALAR);
    // save into structure
    memcpy(ptrMotor, &position, 3);
}

void CommManager:MapLedValue(uint32_t power, uint8_t *ptr)
{
    // TODO: I think this is FUBAR
    power = power * API_LEDPOWER_SCALAR;
    if ( power == EXTLED_REFRESH_PERIOD ) {
        power -= 1;
    }
    memcpy(ptr, &power, 3);
}

#ifdef DEAD
bool BoardComm ( TLV_TYPE type )
{
    uint8_t *data;
    uint8_t length;

    switch (type)
    {
        case TLV_POSITION:
            data = (uint8_t*)&cfgPosition;
            length = (uint8_t)sizeof(tlvPosition_T);
            break;
        case TLV_POWER:
            data = (uint8_t*)&cfgPower;
            length = sizeof(tlvPowerManagement_T);
            break;
        default:
            return false;
    }
    // Send the board the data that we've been updating with interpreted socket data
    if ( false == SerialWriteNBytes(data, length) ) {
        return false;
    }

    // Give board a moment to respond
    usleep(SERIAL_READ_DELAY);

    // Get response from serial ( should be ADC battery voltage reading )
    // TODO: Write a better read algorithm that will keep calling until
    //   everything we want arrives. This is the cause of my hangup. It returns
    //   0 when data hasn't been received yet and fails to send this data to the
    //   connected client
    return SerialGetResponse(SERIAL_TIMEOUT);
}

bool SerialGetResponse( uint32_t timeout )
{
    uint8_t offset; // offset into compiled buffer
    uint8_t i; // pointer in read data
    uint8_t *ptr;

    // init locals
    offset = POS_TLV_TYPE;

    // Compile tlv message
    while ( true ) {
        i = 0; // reset read buffer pointer
        // read enough data to fill tlv struct
        serialRxData.len = SerialRead((uint8_t*)(&(serialRxData.data)),
            (uint32_t)(sizeof(tlvAck_T) - offset), timeout);
        if ( serialRxData.len == SERIAL_ERROR_CODE ) {
            printf("Serial Error\n");
            return false;
        } else if ( serialRxData.len == SERIAL_TIMEOUT_CODE ) {
            printf("Serial Timeout\n");
            return false;
        } else if ( serialRxData.len == 0 ) { // just start over...
            continue;
        }

        if ( POS_TLV_TYPE == offset ) { // look for type code
            // look for type code thowing away garbage before it
            for ( ; (i < serialRxData.len); i++ ) {
                if ( serialRxData.data[i] == TYPE_ACK ) {
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
            if ( i < serialRxData.len ) {
                if ( LENGTH_ACK == serialRxData.data[i] ) {
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
            for ( ; (i < serialRxData.len) && (offset < LENGTH_ACK + 2); i++ ) {
                ptr[offset] = serialRxData.data[i]; // copy byte into struct
                offset += 1; // increment pointer into struct
            }
        }

        // checksum byte
        if ( (LENGTH_ACK + 2 ) == offset ) {
            if ( i < serialRxData.len ) {
                if ( serialRxData.data[i] != ComputeChecksum(&tlvAck.status,
                  tlvAck.length) )
                { // fail - corrupt data
                    offset = 0;
                    continue;
                }
                else { // Checksum verified!
                    bBoardStatus = tlvAck.status;
                    shortADCVoltage = tlvAck.adcVoltage;
                    shortADCCurrent = tlvAck.adcCurrent;
                    return true;
                }
            } else { // no more data; go around
                continue;
            }
        }
    }
}


#endif
