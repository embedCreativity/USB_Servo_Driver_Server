#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "socketInterface.h"
#include "commManager.h"
#include "watchDog.h"
#include "batteryMonitor.h"
#include "sqlUpdater.h"

#define SOCKET_PORT 49000

using namespace std;

bool fRunning = true;

void signal_handler(int sig)
{
    switch(sig)
    {
        case SIGHUP:
        case SIGTERM:
        case SIGINT:
        case SIGQUIT:
            // Wait for threads to return
            fRunning = false; // this kills the main thread
            break;
        default:
            //syslog(LOG_WARNING, "Unhandled signal (%d) %s", sig, strsignal(sig));
            break;
    }
}

int main(void)
{
    SocketInterface socket;
    CommManager commManager;
    Watchdog watchdog;
    BatteryMonitor batteryMonitor;
    SqlUpdater sqlUpdater;

    // signal handler
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

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

    while ( fRunning == true ) {
        usleep(500000);
    }

    commManager.Stop();
    socket.Stop();
    watchdog.Stop();
    batteryMonitor.Stop();
    sqlUpdater.Stop();

    return 0;
}



