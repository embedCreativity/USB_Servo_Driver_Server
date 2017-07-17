
#define CMDBUFLEN 256
#define SQL_TRIM "delete from power where id < ( select min(id) from ( select id from power order by id desc limit 100));"
#define SQL_INSERT "insert into power ( voltage, current ) values ( %.3f, %.3f ); %s"


/* ------------------------------------------------------------ */
/***    SqlLogging
**
**  Synopsis:
**      void    SqlLogging(void *threadid);
**
**  Parameters:
**      void *threadid
**
**  Return Values:
**      none
**
**  Errors:
**      none
**
**  Description:
**
*/
void* SqlLogging(void *arg) {
    sqlite3 *connDB;
    int sqlReturn;
    char *zErrMsg;
    char pathToSqlDb[] = "/var/www/html/sql/stat.sl3";
    char cmd[CMDBUFLEN];

#if defined(DEBUG)
    fprintf(stdout, "SqlLogging Started.\n");#else
    syslog(LOG_INFO, "SqlLogging Started.");
#endif


    while (fRunning) {
        sleep(1);
        sqlReturn = sqlite3_open(pathToSqlDb, &connDB);
        if (sqlReturn) {
            printf("ERROR: Can not open database: %s\n", sqlite3_errmsg(connDB) );
            continue;
        }
        // TODO: protect voltage/current with mutex
        snprintf((char*)cmd, CMDBUFLEN, SQL_INSERT, voltage, current, SQL_TRIM );
        sqlReturn  = sqlite3_exec(connDB, (char*)cmd, NULL, 0, &zErrMsg);
        if( sqlReturn != SQLITE_OK ){
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(connDB);
    }

    return NULL;
} //end Webcam
