/********************************************************************/
/*                                                                  */
/********************************************************************/

#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#include <iostream>
#include <iomanip>
#include <thread>
#include <unistd.h>
#include "pubsub.h"
#include "types.h"

#define WATCHDOG_TIMEOUT 2000 // milliseconds

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

    // primary thread
    void Start();
    void Stop() { running = false; t->join(); };

    // data members
    WatchdogSubscriber *subWatchdog;

    uint32_t watchdogCount;

    Publisher pubCommManager;

private:

    void StartWatchdog(void);

    // main tells us if we're still running
    bool running;

    thread *t;
};

#endif
