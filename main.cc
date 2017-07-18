#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include "socketInterface.h"
#include "commManager.h"
#include "watchDog.h"
#include "batteryMonitor.h"
#include "sqlUpdater.h"

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
    SqlUpdater sqlUpdater;

    // init classes
    socket.SetPort(SOCKET_PORT);
    watchdog.SetControlSafeValues();

    /*** Set up publisher/subscribers ***/
    // Subscribers to commManager's board status data
    commManager.pubBoardStatus.subscribe(socket.subCommManager);
    commManager.pubBoardStatus.subscribe(batteryMonitor.subCommManager);
    commManager.pubBoardStatus.subscribe(sqlUpdater.subCommManager);
    // Watchdog subscribes to being fed by socket
    socket.pubWatchdog.subscribe(watchdog.subWatchdog);
    // socket and watchdog both can set the control data being sent out commManager
    socket.pubCommManager.subscribe(commManager.subControlData);
    watchdog.pubCommManager.subscribe(commManager.subControlData);
    // commManager subscribes to batteryMonitor alerts
    batteryMonitor.pubCommMgrAlert.subscribe(commManager.subMonitorAlert);

    // launch threads
    socket.Start();
    watchdog.Start();
    commManager.Start();
    batteryMonitor.Start();
    sqlUpdater.Start();

    usleep(3000000); // sleep for 3 seconds then kill socket to cause timeout
    socket.Stop();

    // sleep for 10 seconds
    usleep(5000000);

    // now kill watchdog and exit
    watchdog.Stop();
    commManager.Stop();
    batteryMonitor.Stop();
    sqlUpdater.Stop();

    return 0;
}



