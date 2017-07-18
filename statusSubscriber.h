/********************************************************************/
/*                                                                  */
/********************************************************************/

#ifndef __STATUSSUBSCRIBER_H__
#define __STATUSSUBSCRIBER_H__

#include <iostream>
#include "pubsub.h"
#include "types.h"

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
    };

    palmettoStatus_T *pStatus;
};

#endif
