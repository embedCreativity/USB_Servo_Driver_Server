/********************************************************************/
/*                                                                  */
/********************************************************************/

#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#include <iostream>
#include <cstring> // memcpy
#include <iomanip>
#include <thread>
#include <unistd.h>
#include "pubsub.h"
#include "types.h"

#define WATCHDOG_TIMEOUT 2000 // milliseconds

// TODO: need to come up with easy language translation stuff
#define SAFE_MOTOR 3999
#define SAFE_SERVO 120000
#define SAFE_LED 1

using namespace std;

class WatchdogSubscriber: public Subscriber
{
public:
    WatchdogSubscriber(uint32_t *count)
    {
        pCount = count;
    };

    void update(Publisher* who, void* what = 0)
    {
        *pCount = 0; // reset
        cout << "watchDog-> reset!" << endl;
    };

    uint32_t *pCount;
};

class Watchdog {

public:

    // class functions
    Watchdog()
    {
        running = true;
        watchdogCount = 0;
        subWatchdog = new WatchdogSubscriber(&watchdogCount);
    };

    ~Watchdog() {};

    void SetControlSafeValues(void);

    // primary thread
    void Start();
    void Stop() { running = false; t->join(); };

    // data members
    WatchdogSubscriber *subWatchdog;

    uint32_t watchdogCount;

    // watchDog's safe copy of control data
    controlData_T controlData;

    Publisher pubCommManager;

private:

    void StartWatchdog(void);

    // main tells us if we're still running
    bool running;

    thread *t;
};

#endif
