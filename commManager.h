/********************************************************************/
/*                                                                  */
/********************************************************************/

#ifndef __COMM_MANAGER_H__
#define __COMM_MANAGER_H__

#include <iostream>
#include <algorithm> // STL remove
#include <vector> // STL vector
#include <stdint.h>
#include "pubsub.h"
#include "types.h"

using namespace std;

/****************************************************
*   Types                                           *
****************************************************/

/****************************************************
*   Class Declaraction                              *
****************************************************/
class CommManager {

public:

    // class functions
    CommManager() {};
    ~CommManager() {};

    // Call to add subscriber
    void AddSubscriber(void (*callback)(palmettoStatus_T));

    // Call to remove subscriber
    void RemoveSubscriber(void (*callback)(palmettoStatus_T));

    // debug
    void ModStatus(void);

    Publisher publisher;

private:

    // This function calls all the subscribers and sends current status
    void PublishStatus(void);

    // data members
    palmettoStatus_T status;

    // subscribers
    std::vector<void (*)(palmettoStatus_T)> subscribers;

};

#endif

