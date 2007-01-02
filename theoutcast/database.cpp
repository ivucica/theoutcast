#include <stdio.h>
#include <sqlite3.h>
#include "assert.h"
#include "database.h"
#include "defines.h"
sqlite3 *dbData, *dbUser;

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
    if (!dbTableExists("settings")) {
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
    return;
}

bool dbTableExists(const char *tablename) {
    return (dbExecPrintf(dbUser, "select * from %s;", NULL, 0, NULL, tablename) == SQLITE_OK);
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
    dbLoadSettingReturnValue = (char*)malloc(strlen(argv[0]+1));
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

    int rc = sqlite3_exec(db, z, cb, arg, errmsg);
    if (rc != SQLITE_OK) printf("SQLite: Error: '%s', RC: %d, query '%s'\n", sqlite3_errmsg(dbUser), rc, z);
    sqlite3_free(z);

	va_end(vl);

	return rc;
}
