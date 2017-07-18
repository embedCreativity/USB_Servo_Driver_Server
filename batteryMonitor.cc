#include "batteryMonitor.h"

void BatteryMonitor::Start(void)
{
    t = new thread(&BatteryMonitor::StartBatteryMonitor, this);
}

void BatteryMonitor::StartBatteryMonitor(void)
{
    cout << "Starting BatteryMonitor" << endl;

    while(running)
    {
        // do something
        usleep(MONITOR_RATE); // sleep for 1 ms
        if ( (CELL_COUNT * CELL_LOW_VOLTAGE) > status.voltage ) {
            pubCommMgrAlert.notify();
        }
    }
}

