#include "sqlUpdater.h"

#define CMDBUFLEN 256
#define SQL_TRIM "delete from power where id < ( select min(id) from ( select id from power order by id desc limit 100));"
#define SQL_INSERT "insert into power ( voltage, current ) values ( %.3f, %.3f ); %s"

void SqlUpdater::Start(void)
{
    t = new thread(&SqlUpdater::StartSqlUpdater, this);
}

void SqlUpdater::StartSqlUpdater(void)
{
    sqlite3 *connDB;
    int sqlReturn;
    char *zErrMsg;
    char cmd[CMDBUFLEN];

    cout << "Starting SqlUpdater" << endl;

    while (running) {
        usleep(SQL_MONITOR_RATE);
        sqlReturn = sqlite3_open(pathToSqlDb, &connDB);
        if (sqlReturn) {
            cout << "sqlUpdater: Cannot open database: " << sqlite3_errmsg(connDB) << endl;
            continue;
        }
        // TODO: protect voltage/current with mutex
        snprintf((char*)cmd, CMDBUFLEN, SQL_INSERT, status.voltage, status.current, SQL_TRIM );
        sqlReturn  = sqlite3_exec(connDB, (char*)cmd, NULL, 0, &zErrMsg);
        if( sqlReturn != SQLITE_OK ){
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(connDB);
    }
}


