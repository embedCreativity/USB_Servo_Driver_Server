/********************************************************************/
/*                                                                  */
/********************************************************************/

#ifndef __SOCKET_INTERFACE_H__
#define __SOCKET_INTERFACE_H__

#include <iostream>
#include <iomanip>
#include <thread>
#include <unistd.h>
#include "types.h"
#include "pubsub.h"

using namespace std;

class StatusSubscriber: public Subscriber
{
public:
    StatusSubscriber(palmettoStatus_T *status)
    {
        pStatus = status;
    };

    void update(Publisher* who, void* what = 0)
    {
        *pStatus = *((palmettoStatus_T*)what);
        cout << "socket-> publisher called me!" << endl;
    };

    palmettoStatus_T *pStatus;
};

class SocketInterface {

public:

    // class functions
    SocketInterface()
    {
        running = true;
        subCommManager = new StatusSubscriber(&status);
    };

    ~SocketInterface() {};

    void SetPort(uint16_t listenPort) { port = listenPort; };

    // primary socketInterface thread - doesn't return
    void Start();
    void Stop() { running = false; t->join(); };

    // subscribe to commManager's device status data updates
    StatusSubscriber *subCommManager;

    // updated through the subCommManager's publish messages
    palmettoStatus_T status;

    Publisher pubWatchdog;

private:

    void StartServer();

    uint16_t port;

    // main tells us if we're still running
    bool running;

    thread *t;
};

#endif
