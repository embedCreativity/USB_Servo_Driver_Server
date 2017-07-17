#include <iostream>

#include <stdio.h>
#include "socketInterface.h"
#include "commManager.h"

using namespace std;

void feederCallback (void) {
    cout << "Feeder callback called!" << endl;
    return;
}

int main(void)
{
    SocketInterface socket;
    CommManager commManager;

    socket.subCommManager = new StatusSubscriber(&socket.status);

    // Set up callbacks
    socket.SetWatchdogFeeder(feederCallback);
    commManager.publisher.subscribe(socket.subCommManager);

    // do stuff
    commManager.ModStatus(); // modify status and call notify to subs

    socket.StartServer(49000);

    return 0;

}
