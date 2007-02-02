#include "database.h"
#include "protocol.h"
#include "types.h"
#include "glutwin.h"
#include "debugprint.h"
int creatures_n;
creature_t *creatures=NULL;
void GWLogon_Status(glictMessageBox* mb, const char* txt);

void CreatureClear(creature_t* creature) {

    creature->graphics[0] = 0;
    creature->graphics2d[0] = 0;
    creature->spritelist[0] = 0;

    creature->loaded = false;
}


static int CreaturesLoadFunc(void *NotUsed, int argc, char **argv, char **azColName) {
    int creatureid;
    int i, iTmp;
    for (i = 0; i < argc; ++i) {
        if (!strcmp(azColName[i], "creatureid")) break;
    }
    sscanf(argv[i], "%d", &creatureid);
    if (!creatureid > creatures_n) {
        glutHideWindow();
        MessageBox(HWND_DESKTOP, "There was an error in reading creatures database.\nCreature ID appears to be invalid!", "The Outcast - Fatal Error", MB_ICONSTOP);
        exit(1);
    }

    CreatureClear(creatures + creatureid);

    creatures[creatureid].loaded = true;

    for (i = 0; i < argc; ++i) {
        if (!strcmp(azColName[i], "spritelist")) {

            strcpy(creatures[creatureid].spritelist, argv[i]);
        }

    }
    return 0;
}
static int CreaturesLoadNumFunc(void *NotUsed, int argc, char **argv, char **azColName) {
    creatures_n = atoi(argv[0]);
    return 0;
}
void CreaturesLoad() {
    GWLogon_Status(&((GM_MainMenu*)game)->charlist, "Fetching creature properties...");

    creatures_n = 0;
    dbExecPrintf(dbData, CreaturesLoadNumFunc, 0, NULL, "select max(creatureid) from creatures%d;", protocol->GetProtocolVersion());
    DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "%d creatures in database for protocol %d\n", creatures_n, protocol->GetProtocolVersion());
    if (!creatures_n) {
        glutHideWindow();
        MessageBox(HWND_DESKTOP, "There was an error in reading creatures database.\nIt appears that current protocol has no creatures in database.\nPlease reinstall!", "The Outcast - Fatal Error", MB_ICONSTOP);
        exit(1);
    }
    creatures = (creature_t*)malloc(sizeof(creature_t)*(creatures_n+1));

    CreatureClear(creatures); // hurz was bugged for a long time and carries creature 0 in inventory .. so lets be smarter than tibia client and allow creature 0 ... ;)
    dbExecPrintf(dbData, CreaturesLoadFunc, 0, NULL, "select * from creatures%d;", protocol->GetProtocolVersion());

    GWLogon_Status(&((GM_MainMenu*)game)->charlist, "Entering game...");
    //system("pause");
}
