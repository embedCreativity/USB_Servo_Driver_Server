#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include "socketInterface.h"
#include "commManager.h"
#include "watchDog.h"
#include "batteryMonitor.h"

#define SOCKET_PORT 49000

using namespace std;

void feederCallback (void) {
    cout << "Feeder callback called!" << endl;
    return;
}

int main(void)
{
    SocketInterface socket;
    CommManager commManager;
    Watchdog watchdog;
    BatteryMonitor batteryMonitor;

    // init classes
    socket.SetPort(SOCKET_PORT);
    watchdog.SetControlSafeValues();

    // Set up publisher/subscribers
    commManager.pubBoardStatus.subscribe(socket.subCommManager);
    commManager.pubBoardStatus.subscribe(batteryMonitor.subCommManager);
    socket.pubWatchdog.subscribe(watchdog.subWatchdog);
    // socket and watchdog both can set the control data being sent out commManager
    socket.pubCommManager.subscribe(commManager.subControlData);
    watchdog.pubCommManager.subscribe(commManager.subControlData);
    batteryMonitor.pubCommMgrAlert.subscribe(commManager.subMonitorAlert);

    // launch threads
    socket.Start();
    watchdog.Start();
    commManager.Start();
    batteryMonitor.Start();

    usleep(3000000); // sleep for 3 seconds then kill socket to cause timeout
    socket.Stop();

    // sleep for 10 seconds
    usleep(10000000);

    // now kill watchdog and exit
    watchdog.Stop();
    commManager.Stop();
    batteryMonitor.Stop();

    return 0;
}
