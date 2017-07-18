#include "sqlUpdater.h"

void SqlUpdater::Start(void)
{
    t = new thread(&SqlUpdater::StartSqlUpdater, this);
}

void SqlUpdater::StartSqlUpdater(void)
{
    cout << "Starting SqlUpdater" << endl;

    while(running)
    {
        // do something
        usleep(SQL_MONITOR_RATE);
        // Insert data into database
    }
}

