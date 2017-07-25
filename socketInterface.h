/********************************************************************/
/*                                                                  */
/********************************************************************/

#ifndef __SOCKET_INTERFACE_H__
#define __SOCKET_INTERFACE_H__

#include <iostream>
#include <iomanip>
#include <thread>
#include <unistd.h>
#include <socket.h> // my installed library
#include "pubsub.h"
#include "types.h"
#include "statusSubscriber.h"
#include "watchDog.h"

using namespace std;

extern SocketInterface_T socketIntf;

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
    void Stop();

    // subscribe to commManager's device status data updates
    StatusSubscriber *subCommManager;

    // updated through the subCommManager's publish messages
    palmettoStatus_T status;

    // socket's copy of the current control data
    ControlData controlData;

    Publisher pubWatchdog;
    Publisher pubCommManager;
    Publisher pubClient;

private:

    void StartServer();
    void HandleClient();
    bool InterpretSocketCommand(uint8_t *data, uint32_t length);
    uint16_t port;

    // main tells us if we're still running
    bool running;

    thread *t;
};

#endif
