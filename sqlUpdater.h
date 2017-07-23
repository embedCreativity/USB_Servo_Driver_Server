/********************************************************************/
/*                                                                  */
/********************************************************************/

#ifndef __SQLUPDATER_H__
#define __SQLUPDATER_H__

#include <iostream>
#include <cstring> // memcpy
#include <iomanip>
#include <thread>
#include <unistd.h>
#include <sqlite3.h>
#include "pubsub.h"
#include "types.h"
#include "statusSubscriber.h"

#define CELL_COUNT          3
#define CELL_LOW_VOLTAGE    3.0
#define SQL_MONITOR_RATE    1000000 // check once per second
#define SQL_PATH            "/var/www/html/sql/stat.sl3"

using namespace std;

class SqlUpdater {

public:

    // class functions
    SqlUpdater()
    {
        running = true;
        subCommManager = new StatusSubscriber(&status);
        status.current = 0;
        status.voltage = (CELL_COUNT * CELL_LOW_VOLTAGE); // init to a good level
        snprintf(pathToSqlDb, 128, SQL_PATH);
    };

    ~SqlUpdater() {};

    // primary thread
    void Start();
    void Stop() { running = false; t->join(); };

    // subscribe to commManager's device status data updates
    StatusSubscriber *subCommManager;

    palmettoStatus_T status;

private:

    void StartSqlUpdater(void);

    // private members
    char pathToSqlDb[128];

    bool running;
    thread *t;
};

#endif
