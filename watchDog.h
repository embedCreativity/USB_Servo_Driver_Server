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

#define WATCHDOG_RATE 10000 // check every 10ms
#define WATCHDOG_TIMEOUT 50 // 10ms * 50 = 500milliseconds

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
    };

    uint32_t *pCount;
};

class ClientSubscriber: public Subscriber
{
public:
    ClientSubscriber(bool *clockRunning)
    {
        pClockRunning = clockRunning;
    };

    void update(Publisher* who, void* what = 0)
    {
        *pClockRunning = *((bool*)what);
    };

    bool *pClockRunning;
};

class Watchdog {

public:

    // class functions
    Watchdog()
    {
        running = true;
        clockRunning = false;
        watchdogCount = 0;
        subWatchdog = new WatchdogSubscriber(&watchdogCount);
        subClient = new ClientSubscriber(&clockRunning);
    };

    ~Watchdog() {};

    // primary thread
    void Start();
    void Stop() { running = false; t->join(); };

    // socket's control of the running clock
    void StartClock();
    void StopClock();

    // pub/sub classes
    WatchdogSubscriber *subWatchdog;
    ClientSubscriber *subClient;

    // pub/sub data
    uint32_t watchdogCount;
    bool clockRunning;

    // watchDog's safe copy of control data
    ControlData controlData;

    Publisher pubCommManager;

private:

    void StartWatchdog(void);

    // main tells us if we're still running
    bool running;

    thread *t;
};

#endif
