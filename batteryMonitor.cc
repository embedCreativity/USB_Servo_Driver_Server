#include "batteryMonitor.h"

void BatteryMonitor::Start(void)
{
    t = new thread(&BatteryMonitor::StartBatteryMonitor, this);
}

void BatteryMonitor::StartBatteryMonitor(void)
{
    uint8_t i;

    cout << "Starting BatteryMonitor" << endl;

    while(running)
    {
        // do something
        usleep(MONITOR_RATE); // sleep for 1 ms
        if ( (CELL_COUNT * CELL_LOW_VOLTAGE) > status.voltage ) {
            i += 1; // increment
            if ( i >= LOW_SUBSEQUENT_READINGS ) {
                pubCommMgrAlert.notify(); // kill thy self
            }
        } else {
            i = 0; // reset counter
        }
    }
}

