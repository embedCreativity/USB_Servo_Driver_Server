#include "watchDog.h"

void Watchdog::Start(void)
{
    controlData.Init(); // set safe values
    t = new thread(&Watchdog::StartWatchdog, this);
}

void Watchdog::StartWatchdog(void)
{
    bool lastClockRunning;

    lastClockRunning = true;

    cout << "Starting WatchDog" << endl;

    while(running)
    {
        usleep(WATCHDOG_RATE); // throttle
        if ( clockRunning ) {
            if ( watchdogCount > WATCHDOG_TIMEOUT ) {
                pubCommManager.notify(&controlData);
                usleep(500000); // throttle (sleep 500ms)
            } else {
                watchdogCount += 1;
            }
        } else if ( lastClockRunning ) { // it was running, but just stopped
            watchdogCount = 0; // reset
            cout << "watchdog-> client disconnected" << endl;
            pubCommManager.notify(&controlData);
        }
        lastClockRunning = clockRunning; // update last
    }
}

