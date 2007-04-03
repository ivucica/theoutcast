// FIXME crash report #1
/*Error occured on Thursday, January 18, 2007 at 22:16:56.

C:\Documents and Settings\ivucica\My Documents\Development\The Outcast\The Outcast debug.exe caused an Access Violation at location 77c4610f in module C:\WINDOWS\system32\msvcrt.dll Writing to location 0be12000.

Registers:
eax=810100f8 ebx=00000001 ecx=0971ebe4 edx=00000074 esi=0bdecb38 edi=0be12000
eip=77c4610f esp=00238020 ebp=00238040 iopl=0         nv up ei pl zr na po nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010246

Call stack:
77C4610F  C:\WINDOWS\system32\msvcrt.dll:77C4610F  strcat
00458F85  C:\Documents and Settings\ivucica\My Documents\Development\The Outcast\The Outcast debug.exe:00458F85
00402C58  C:\Documents and Settings\ivucica\My Documents\Development\The Outcast\The Outcast debug.exe:00402C58  dbExecPrintf(sqlite3*, char const*, int (*)(void*, int, char**, char**), void*, char**, ...)  C:/Documents and Settings/ivucica/My Documents/Development/The Outcast/database.cpp:125
00402A0E  C:\Documents and Settings\ivucica\My Documents\Development\The Outcast\The Outcast debug.exe:00402A0E  dbLoadSetting(char const*, char*, int, char const*)  C:/Documents and Settings/ivucica/My Documents/Development/The Outcast/database.cpp:76
0040A162  C:\Documents and Settings\ivucica\My Documents\Development\The Outcast\The Outcast debug.exe:0040A162  GM_MainMenu::GM_MainMenu()  C:/Documents and Settings/ivucica/My Documents/Development/The Outcast/gm_mainmenu.cpp:127
00403A71  C:\Documents and Settings\ivucica\My Documents\Development\The Outcast\The Outcast debug.exe:00403A71  GameModeEnter(gamemode_t)  C:/Documents and Settings/ivucica/My Documents/Development/The Outcast/gamemode.cpp:46
00404AF0  C:\Documents and Settings\ivucica\My Documents\Development\The Outcast\The Outcast debug.exe:00404AF0  GM_Gameworld::KeyPress(unsigned char, int, int)  C:/Documents and Settings/ivucica/My Documents/Development/The Outcast/gm_gameworld.cpp:37
0040425A  C:\Documents and Settings\ivucica\My Documents\Development\The Outcast\The Outcast debug.exe:0040425A  glut_Key(unsigned char, int, int)  C:/Documents and Settings/ivucica/My Documents/Development/The Outcast/glutwin.cpp:82
00426ACA  C:\Documents and Settings\ivucica\My Documents\Development\The Outcast\The Outcast debug.exe:00426ACA  glictPanel::Paint()  C:/Documents and Settings/ivucica/My Documents/Development/[CPP] GLICT/GLICT/panel.cpp:66

APPEARS TO BE RESOLVED?!?!?!?!

*/


#ifdef WIN32
	#include <windows.h>
#endif
#include <stdio.h>
//#include <sqlite3.h>
#include "debugprint.h"
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
    #ifndef SQLITE_OLD
    rc = sqlite3_open("user.db", &dbUser);
    #else
    dbUser = sqlite_open("user.db", 0, NULL);
	if (dbUser) rc = SQLITE_OK; else rc = SQLITE_OK + 1;
    #endif
    if( rc != SQLITE_OK ){
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "SQLite cannot open user database. Verify directory access rights!\nError: %s", sqlite3_errmsg(dbUser));
        //sqlite3_free((char*)freeme);
        //sqlite3_close(dbUser);
        dbUser=NULL;
        goto datadb;
    }
    // try to access settings table
    if (!dbTableExists(dbUser, "settings")) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_WARNING, "SQLite could not access table 'settings'. Reason: %s. Trying to create...\n", sqlite3_errmsg(dbUser));
        //sqlite3_free((char*)freeme);
        if (dbExec(dbUser, "create table settings (`field` text, `value` text);", NULL, 0, NULL) != SQLITE_OK) {
            DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "SQLite cannot initialize settings table in user database. Verify access rights on user.db!\nError: %s", sqlite3_errmsg(dbUser));
            //sqlite3_free((char*)freeme);
            sqlite3_close(dbUser);
            goto datadb;
        }
    }
    // try to access map table
        // try to access table
    if (!dbTableExists(dbUser, "map")) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_WARNING, "SQLite could not access table 'map'. Reason: %s. Trying to create...\n", sqlite3_errmsg(dbUser));
        //sqlite3_free((char*)freeme);
        if (dbExec(dbUser, "create table map (`server` string, `port` integer, `x` integer, `y` integer, `z` integer, `stackpos` integer, `itemid` integer);", NULL, 0, NULL) != SQLITE_OK) {
            DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "SQLite cannot initialize settings table in user database. Verify access rights on user.db!\nError: %s", sqlite3_errmsg(dbUser));
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
    #ifndef SQLITE_OLD
    rc = sqlite3_open("data.db", &dbData);
    #else
	{
		char **tmp;
		dbData = sqlite_open("data.db", 0, tmp);
		if (dbData)
			rc = SQLITE_OK;
		else {
			DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "SQLite cannot open data database. Verify directory access rights!\nError: %s", *tmp);
			rc = SQLITE_OK + 1;
		}
	}
    #endif
    if (rc!=SQLITE_OK) {

        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "SQLite cannot open data database. Verify directory access rights!\nError: %s", sqlite3_errmsg(dbData));
        //MessageBox(HWND_DESKTOP, "SQLite cannot open data database.\nVerify directory access rights!", "The Outcast - Fatal Error", MB_ICONSTOP);
        dbData=NULL;
		DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "This was a fatal error. Quitting.");
        exit(1);
    }


    return;
}

bool dbTableExists(sqlite3 *db, const char *tablename) {
    return (dbExecPrintf(db, NULL, 0, NULL, "select * from %s;", tablename) == SQLITE_OK);
}

void dbSaveSetting(const char* settingname, const char* value) {
    dbExecPrintf(dbUser, NULL, 0, NULL, "delete from settings where `field`='%s';", settingname);
    dbExecPrintf(dbUser, NULL, 0, NULL, "insert into settings (`field`, `value`) values ('%q', '%q');", settingname, value);
}

bool dbLoadSetting(const char* settingname, char* valuetarget, int maxlen, const char *defaultval) {
    dbLoadSettingReturnValue = NULL;
    if (dbExecPrintf(dbUser, dbLoadSettingFunc, 0, NULL, "select `value` from settings where `field` = '%q';", settingname) == SQLITE_OK) { // Crash report #1
        if (dbLoadSettingReturnValue) {
            //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Returned value %s\n", dbLoadSettingReturnValue);
            memcpy(valuetarget, dbLoadSettingReturnValue, min(maxlen, strlen(dbLoadSettingReturnValue)));
            valuetarget[min(maxlen, strlen(dbLoadSettingReturnValue))] = 0;

            free(dbLoadSettingReturnValue);
            dbLoadSettingReturnValue = NULL;
            return true;
        } else {
            if (defaultval) {
                memcpy(valuetarget, defaultval, min(maxlen, strlen(defaultval)));
                valuetarget[min(maxlen, strlen(defaultval))] = 0;
                DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "Returned default value %s\n", defaultval);

                return true;
            }
            else {
                valuetarget[0] = 0;
                return false;
            }
        }
    } else {

        DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_ERROR, "SQLite cannot load a setting!\n\nError: %s\n", sqlite3_errmsg(dbUser));
        valuetarget[0] = 0;
        return false;
    }
}
static int dbLoadSettingFunc(void *NotUsed, int argc, char **argv, char **azColName) {
    dbLoadSettingReturnValue = (char*)malloc(strlen(argv[0])+2);
    //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "%s\n", argv[0]);
    if (!dbLoadSettingReturnValue) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_ERROR, "Could not alloc %d bytes\n", strlen(argv[0]+2));
        return 0;
    }
    memcpy(dbLoadSettingReturnValue, argv[0], strlen(argv[0])+1);
    //strcpy(dbLoadSettingReturnValue, argv[0]);

    //system("pause");
    return 0;
}
int dbExecPrintf(
  sqlite3* db,                     /* An open database */
  sqlite3_callback cb,             /* Callback function */
  void *arg,                       /* 1st argument to callback function */
  char **errmsg,                   /* Error msg written here */
  const char *sql,                 /* SQL to be executed */
  ...) {

	va_list vl;
	va_start(vl, sql);

    char *z = sqlite3_vmprintf(sql, vl);

	va_end(vl);


    int rc = sqlite3_exec(db, z, cb, arg, errmsg); // Crash report #1
    if (rc != SQLITE_OK) {

        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "SQLite: Error: '%s', RC: %s, query '%s'\n", sqlite3_errmsg(dbUser), dbProcessRC(rc), z);
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_NORMAL, "QUERY: %s\n", z);
    } else {
        //DEBUGPRINT(DEBUGPRINT_LEVEL_JUNK, DEBUGPRINT_NORMAL, "SQLite query success\n");
    }
    sqlite3_free(z);


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
