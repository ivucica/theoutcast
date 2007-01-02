#ifndef __DATABASE_H
#define __DATABASE_H

#include <sqlite3.h>

void DBInit();
bool dbTableExists(const char *tablename);
int dbExecPrintf(
  sqlite3*,                     /* An open database */
  const char *sql,              /* SQL to be executed */
  sqlite3_callback,             /* Callback function */
  void *,                       /* 1st argument to callback function */
  char **errmsg,                /* Error msg written here */
  ...);
#define dbExec sqlite3_exec
bool dbLoadSetting(const char* settingname, char* valuetarget, int maxlen, const char *defaultval);
void dbSaveSetting(const char* settingname, const char* value);

extern sqlite3 *dbData, *dbUser;
#endif
