/********************************************************************/
/*                                                                  */
/********************************************************************/

#ifndef __COMM_MANAGER_H__
#define __COMM_MANAGER_H__

#include <iostream>
#include <algorithm> // STL remove
#include <vector> // STL vector
#include <thread>
#include <stdint.h>
#include <unistd.h>
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
    ControlDataSubscriber(controlData_T *data)
    {
        pControlData = data;
    };

    void update(Publisher* who, void* what = 0)
    {
        *pControlData = *((controlData_T*)what);
        cout << "commManager-> controlData Updated" << endl;
    };

    controlData_T *pControlData;
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

class CommManager {

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

    // debug
    void ModStatus(void);

    // publishers (socket&watchdog) set this
    controlData_T controlData;
    bool bLowVoltage;

    // subscribe to socket and watchdog
    ControlDataSubscriber *subControlData;
    MonitorAlertSubscriber *subMonitorAlert;

    Publisher pubBoardStatus;

private:

    void StartCommManager();
    // data members
    palmettoStatus_T status;

    // subscribers
    std::vector<void (*)(palmettoStatus_T)> subscribers;

    bool running;

    thread *t;
};

#endif

