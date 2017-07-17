/********************************************************************/
/*                                                                  */
/********************************************************************/

#ifndef __SOCKET_INTERFACE_H__
#define __SOCKET_INTERFACE_H__

#include <iostream>
#include "types.h"
#include "commManager.h"

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
    SocketInterface() {};
    ~SocketInterface() {};

    // primary socketInterface thread - doesn't return
    void StartServer(uint16_t port);

    // Provide the watchdog's callback here
    void SetWatchdogFeeder(void (*callback)(void)) { feedWatchDog = callback; }

    // data members
    StatusSubscriber *subCommManager;

    // updated through the UpdateStatus callback
    palmettoStatus_T status;

private:

    // Watchdog Feed callback
    void (*feedWatchDog)(void);


    // main tells us if we're still running
    bool running;
};

#endif
