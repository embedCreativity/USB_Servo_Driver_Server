/********************************************************************/
/*                                                                  */
/********************************************************************/

#ifndef __COMM_MANAGER_H__
#define __COMM_MANAGER_H__

#include <iostream>
#include <algorithm> // STL remove
#include <vector> // STL vector
#include <thread>
#include <cstring> // memcpy
#include <stdint.h>
#include <unistd.h>
extern "C" {
    #include <ec_serial.h> // my serial port library
}
#include "pubsub.h"
#include "types.h"

using namespace std;

/****************************************************
*   Types                                           *
****************************************************/

/****************************************************
*   Class Declaractions                             *
****************************************************/

// Classes can publish updated control data here
class ControlDataSubscriber: public Subscriber
{
public:
    ControlDataSubscriber(ControlData *data)
    {
        pControlData = data;
    };

    void update(Publisher* who, void* what = 0)
    {
        *pControlData = *((ControlData*)what);
        cout << "commManager-> controlData Updated" << endl;
    };

    ControlData *pControlData;
};

// This fires when the voltage drops too low
class MonitorAlertSubscriber: public Subscriber
{
public:
    MonitorAlertSubscriber(bool *alert)
    {
        pAlert= alert;
    };

    void update(Publisher* who, void* what = 0)
    {
        *pAlert = true;
        cout << "commManager-> low voltage!" << endl;
    };

    bool *pAlert;
};

class CommManager
{

public:

    // class functions
    CommManager()
    {
        running = true;
        bLowVoltage = false;
        status.status = 1;
        status.voltage = 14.123;
        status.current = 1.234;
        status.commFault = false;

        subControlData = new ControlDataSubscriber(&controlData);
        subMonitorAlert = new MonitorAlertSubscriber(&bLowVoltage);
    };

    ~CommManager() {};

    // primary commManager thread
    void Start();
    void Stop() { running = false; t->join(); };

    // Call to add subscriber
    void AddSubscriber(void (*callback)(palmettoStatus_T));

    // Call to remove subscriber
    void RemoveSubscriber(void (*callback)(palmettoStatus_T));

    // Initialize serial port
    bool InitComms(char *port, int baudRate);

    // publishers (socket&watchdog) set this
    ControlData controlData;
    bool bLowVoltage;

    // subscribe to socket and watchdog
    ControlDataSubscriber *subControlData;
    MonitorAlertSubscriber *subMonitorAlert;

    Publisher pubBoardStatus;

private:

    void StartCommManager();
    uint8_t ComputeChecksum(uint8_t *input, uint32_t length);
    void MapMotorValue(int32_t power, uint8_t *ptr);
    void MapServoValue(uint32_t position, uint8_t *ptr);
    void MapLedValue(uint32_t power, uint8_t *ptr);
    bool SendPositionData(void);
    bool SendPowerData(void);
    bool SendHeartBeat(void);
    bool SendCommand(uint8_t *data, uint8_t length);
    bool SerialGetResponse( uint32_t timeout );

    // published board status
    palmettoStatus_T status;

    // subscribers
    std::vector<void (*)(palmettoStatus_T)> subscribers;

    // copy of last transmitted control data
    ControlData lastData;

    bool running;

    thread *t;
};

#endif

