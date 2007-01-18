#include <windows.h>
#include <stdio.h>
#include <sqlite3.h>
#include "assert.h"
#include "database.h"
#include "defines.h"
sqlite3 *dbData, *dbUser;


// when running in debug mode with gdb, do:
#define free
// to turn off free. it appears to be bugged!


// TODO: make all functions threadsafe; do a lock at enter of function, and unlock at leave of function
// TODO: dbSaveSetting and dbLoadSetting must check if dbUser is loaded at all
char* dbLoadSettingReturnValue;
static int dbLoadSettingFunc(void *NotUsed, int argc, char **argv, char **azColName);
void DBInit() {
    int rc;

    if (dbUser) {
        sqlite3_close(dbUser);
        dbUser=NULL;
    }
    rc = sqlite3_open("user.db", &dbUser);
    if( rc != SQLITE_OK ){
        printf("SQLite cannot open user database.\nVerify directory access rights!\n\nError: %s\n", sqlite3_errmsg(dbUser));
        //sqlite3_free((char*)freeme);
        sqlite3_close(dbUser);
        dbUser=NULL;
        goto datadb;
    }
    // try to access table
    if (!dbTableExists(dbUser, "settings")) {
        printf("SQLite could not access table 'settings'. Reason: %s. Trying to create...\n", sqlite3_errmsg(dbUser));
        //sqlite3_free((char*)freeme);
        if (dbExec(dbUser, "create table settings (`field` text, `value` text);", NULL, 0, NULL) != SQLITE_OK) {
            printf("SQLite cannot initialize settings table in user database.\nVerify access rights on user.db!\n\nError: %s\n", sqlite3_errmsg(dbUser));
            //sqlite3_free((char*)freeme);
            sqlite3_close(dbUser);
            goto datadb;
        }
    }

    datadb:
    if (dbData) {
        sqlite3_close(dbData);
        dbUser=NULL;
    }
    rc = sqlite3_open("data.db", &dbData);
    if (rc!=SQLITE_OK) {

        printf("SQLite cannot open data database.\nVerify directory access rights!\n\nError: %d\n", sqlite3_errmsg(dbData));
        MessageBox(HWND_DESKTOP, "SQLite cannot open data database.\nVerify directory access rights!", "The Outcast - Fatal Error", MB_ICONSTOP);
        sqlite3_close(dbData);
        dbData=NULL;
        exit(1);
    }


    return;
}

bool dbTableExists(sqlite3 *db, const char *tablename) {
    return (dbExecPrintf(db, "select * from %s;", NULL, 0, NULL, tablename) == SQLITE_OK);
}

void dbSaveSetting(const char* settingname, const char* value) {
    dbExecPrintf(dbUser, "delete from settings where `field`='%s';", NULL, 0, NULL, settingname);
    dbExecPrintf(dbUser, "insert into settings (`field`, `value`) values ('%q', '%q');", NULL, 0, NULL, settingname, value);
}

bool dbLoadSetting(const char* settingname, char* valuetarget, int maxlen, const char *defaultval) {
    dbLoadSettingReturnValue = NULL;
    if (dbExecPrintf(dbUser, "select `value` from settings where `field` = '%q';", dbLoadSettingFunc, 0, NULL, settingname) == SQLITE_OK) {
        if (dbLoadSettingReturnValue) {
            printf("Returned value %s\n", dbLoadSettingReturnValue);
            memcpy(valuetarget, dbLoadSettingReturnValue, min(maxlen, strlen(dbLoadSettingReturnValue)));
            valuetarget[min(maxlen, strlen(dbLoadSettingReturnValue))] = 0;

            free(dbLoadSettingReturnValue);
            return true;
        } else {
            if (defaultval) {
                memcpy(valuetarget, defaultval, min(maxlen, strlen(defaultval)));
                valuetarget[min(maxlen, strlen(defaultval))] = 0;
                printf("Returned default value %s\n", defaultval);

                return true;
            }
            else {
                valuetarget[0] = 0;
                return false;
            }
        }
    } else {

        printf("SQLite cannot load a setting!\n\nError: %s\n", sqlite3_errmsg(dbUser));
        valuetarget[0] = 0;
        return false;
    }
}
static int dbLoadSettingFunc(void *NotUsed, int argc, char **argv, char **azColName) {
    dbLoadSettingReturnValue = (char*)malloc(strlen(argv[0]+2));
    strcpy(dbLoadSettingReturnValue, argv[0]);
    printf("%s %s\n", argv[0], dbLoadSettingReturnValue);
    //system("pause");
    return 0;
}
int dbExecPrintf(
  sqlite3* db,                     /* An open database */
  const char *sql,                 /* SQL to be executed */
  sqlite3_callback cb,             /* Callback function */
  void *arg,                       /* 1st argument to callback function */
  char **errmsg,                   /* Error msg written here */
  ...) {

	va_list vl;
	va_start(vl, sql);

    char *z = sqlite3_vmprintf(sql, vl);

    printf("QUERY: %s\n", z);
    int rc = sqlite3_exec(db, z, cb, arg, errmsg);
    if (rc != SQLITE_OK) printf("SQLite: Error: '%s', RC: %s, query '%s'\n", sqlite3_errmsg(dbUser), dbProcessRC(rc), z);
    sqlite3_free(z);

	va_end(vl);

	return rc;
}


const char* dbProcessRC(int rc) {
      switch (rc) {
           case SQLITE_OK:
                return "Successful result";
                break;
           case SQLITE_ERROR:
                return "SQL error or missing database";
                break;
           case SQLITE_INTERNAL:
                return "An internal logic error in SQLite";
                break;
           case SQLITE_PERM:
                return "Access permission denied";
                break;
           case SQLITE_ABORT:
                return "Callback routine requested an abort";
                break;
           case SQLITE_BUSY:
                return "The database file is locked";
                break;
           case SQLITE_LOCKED:
                return "A table in the database is locked";
                break;
           case SQLITE_NOMEM:
                return "A malloc() failed";
                break;
           case SQLITE_READONLY:
                return "Attempt to write a readonly database";
                break;
           case SQLITE_INTERRUPT:
                return "Operation terminated by sqlite_interrupt()";
                break;
           case SQLITE_IOERR:
                return "Some kind of disk I/O error occurred";
                break;
           case SQLITE_CORRUPT:
                return "The database disk image is malformed";
                break;
           case SQLITE_NOTFOUND:
                return "(Internal Only) Table or record not found";
                break;
           case SQLITE_FULL:
                return "Insertion failed because database is full";
                break;
           case SQLITE_CANTOPEN:
                return "Unable to open the database file";
                break;
           case SQLITE_PROTOCOL:
                return "Database lock protocol error";
                break;
           case SQLITE_EMPTY:
                return "(Internal Only) Database table is empty";
                break;
           case SQLITE_SCHEMA:
                return "The database schema changed";
                break;
           case SQLITE_TOOBIG:
                return "Too much data for one row of a table";
                break;
           case SQLITE_CONSTRAINT:
                return "Abort due to constraint violation";
                break;
           case SQLITE_MISMATCH:
                return "Data type mismatch";
                break;
           case SQLITE_MISUSE:
                return "Library used incorrectly";
                break;
           case SQLITE_NOLFS:
                return "Uses OS features not supported on host";
                break;
           case SQLITE_AUTH:
                return "Authorization denied";
                break;
           case SQLITE_ROW:
                return "sqlite_step() has another row ready";
                break;
           case SQLITE_DONE:
                return "sqlite_step() has finished executing";
                break;
           default:
                return "Unknown rc";
      }
}
