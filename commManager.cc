#include "commManager.h"

void CommManager::Start()
{
    t = new thread(&CommManager::StartCommManager, this);
}

void CommManager::StartCommManager()
{
    cout << "Starting CommManager" << endl;

    while ( running )
    {
        usleep(500000);
        cout << "commMgr V before " << status.voltage << endl;
        status.voltage = status.voltage - 1.0;
        cout << "commMgr V after " << status.voltage << endl;
        cout << "commManager[motorA]: " <<
            unsigned(controlData.motorA[0]) << endl;
        pubBoardStatus.notify(&status);
    }
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
