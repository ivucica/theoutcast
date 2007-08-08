#include "database.h"
#include "protocol.h"
#include "types.h"
#include "windowing.h"
#include "debugprint.h"
int distances_n;
distance_t **distances=NULL;
void GWLogon_Status(glictMessageBox* mb, const char* txt);

void DistanceClear(distance_t* distance) {

    distance->graphics[0] = 0;
    distance->graphics2d[0] = 0;
    distance->spritelist[0] = 0;


    if (distance->textures) for (int i=0;i<distance->sli.numsprites;i++)
        if (distance->textures) {
            delete ((Texture**)distance->textures)[i];
        } else {
            printf("distance->textures is NULL!\n");
        }
    if (distance->textures) free(distance->textures);
    distance->sli.numsprites = 0;
    distance->textures = NULL;

    distance->loaded = false;

}
void DistanceInit(distance_t* distance) {

    distance->textures = NULL;
    distance->sli.numsprites = 0;

    DistanceClear(distance);
}


static int DistancesLoadFunc(void *NotUsed, int argc, char **argv, char **azColName) {
    int distanceid;
    int i;//, iTmp; // FIXME unused, remove me
    for (i = 0; i < argc; ++i) {
        if (!strcmp(azColName[i], "distanceid")) break;
    }
    sscanf(argv[i], "%d", &distanceid);
    if (distanceid > distances_n) {
        //glutHideWindow();
        //MessageBox(HWND_DESKTOP, "There was an error in reading distances database.\nDistance ID appears to be invalid!", "The Outcast - Fatal Error", MB_ICONSTOP);
        exit(1);
    }


    DistanceClear(*(distances + distanceid));

    distances[distanceid]->loaded = true;



    for (i = 0; i < argc; ++i) {
        if (!strcmp(azColName[i], "spritelist")) {

            strcpy(distances[distanceid]->spritelist, argv[i]);
        }

    }

    return 0;
}
static int DistancesLoadNumFunc(void *NotUsed, int argc, char **argv, char **azColName) {
	printf("Argc: %d argv: %s\n", argc, argv[0]);
    if (argc!=0)
		distances_n = argv[0] ? atoi(argv[0]) : 0;
	else {
		ASSERT(false);
		distances_n = 0;
	}
    return 0;
}
void DistancesLoad() {
    GWLogon_Status(&((GM_MainMenu*)game)->charlist, "Fetching distance properties...");

    distances_n = 0;
    dbExecPrintf(dbData, DistancesLoadNumFunc, 0, NULL, "select max(distanceid) from distances%d;", protocol->GetProtocolVersion());
    DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "%d distances in database for protocol %d\n", distances_n, protocol->GetProtocolVersion());
    if (!distances_n) {
        //glutHideWindow();
        //MessageBox(HWND_DESKTOP, "There was an error in reading distances database.\nIt appears that current protocol has no distances in database.\nPlease reinstall!", "The Outcast - Fatal Error", MB_ICONSTOP);
        exit(1);
    }
    distances = (distance_t**)malloc(sizeof(distance_t*)*(distances_n+1));

    for (int i = 0; i < distances_n+1; i++) {

        distances[i] = new distance_t;
        DistanceInit(distances[i]);

    }
    dbExecPrintf(dbData, DistancesLoadFunc, 0, NULL, "select * from distances%d;", protocol->GetProtocolVersion());

    GWLogon_Status(&((GM_MainMenu*)game)->charlist, "Entering game...");
    //system("pause");
}
void DistancesLoad_NoUI(unsigned int protocolversion) {
    distances_n = 0;
    dbExecPrintf(dbData, DistancesLoadNumFunc, 0, NULL, "select max(distanceid) from distances%d;", protocolversion);
    DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "%d distances in database for protocol %d\n", distances_n, protocolversion);

    if (!distances_n) {
        //glutHideWindow();
        //MessageBox(HWND_DESKTOP, "There was an error in reading distances database.\nIt appears that current protocol has no distances in database.\nPlease reinstall!", "The Outcast - Fatal Error", MB_ICONSTOP);
        exit(1);
    }
    distances = (distance_t**)malloc(sizeof(distance_t*)*(distances_n+1));

    for (int i = 0; i < distances_n+1; i++) {
        distances[i] = new distance_t;
        DistanceInit(distances[i]);
    }
    dbExecPrintf(dbData, DistancesLoadFunc, 0, NULL, "select * from distances%d;", protocolversion);

}

void DistancesUnload() {
    for (int i=0;i<distances_n;i++) {
        DistanceClear(*(distances + i));
        delete *(distances + i);
    }
    free(distances);
    distances = NULL;
}

