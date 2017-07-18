/********************************************************************/
/*                                                                  */
/********************************************************************/

#ifndef __BATTERYMONITOR_H__
#define __BATTERYMONITOR_H__

#include <iostream>
#include <cstring> // memcpy
#include <iomanip>
#include <thread>
#include <unistd.h>
#include "pubsub.h"
#include "types.h"
#include "statusSubscriber.h"

#define CELL_COUNT 3
#define CELL_LOW_VOLTAGE 3.0
#define MONITOR_RATE 1000000 // check once per second

using namespace std;

class BatteryMonitor {

public:

    // class functions
    BatteryMonitor()
    {
        running = true;
        subCommManager = new StatusSubscriber(&status);
        status.voltage = (CELL_COUNT * CELL_LOW_VOLTAGE); // init to a good level
    };

    ~BatteryMonitor() {};

    // primary thread
    void Start();
    void Stop() { running = false; t->join(); };

    // subscribe to commManager's device status data updates
    StatusSubscriber *subCommManager;
    Publisher pubCommMgrAlert;

    palmettoStatus_T status;

private:

    void StartBatteryMonitor(void);

    // private members
    bool running;
    thread *t;
};

#endif
