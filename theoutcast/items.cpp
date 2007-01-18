#include "database.h"
#include "protocol.h"
#include "types.h"
#include "glutwin.h"
int items_n;
item_t *items=NULL;
void GWLogon_Status(glictMessageBox* mb, const char* txt);



void ItemClear(item_t* item) {

    item->graphics[0] = 0;
    item->ground = false;
    item->speedindex = 0;
    item->topindex = 0;
    item->container = false;
    item->stackable = false;
    item->usable = false;
    item->readable = false;
    item->writable = false;
    item->fluidcontainer = false;
    item->splash = false;
    item->movable = true;
    item->pickupable = false;
    item->blocking = false;
    item->floorchange = false;
    item->readability_len = 0;
    item->lightcolor = 0;
    item->lightradius = 0;
    item->height = 0.;
    item->height2d_x = 0; item->height2d_y = 0;
    item->minimapcolor = 0;
    item->spritelist[0] = 0;
    item->otid = 0;
}


static int ItemsLoadFunc(void *NotUsed, int argc, char **argv, char **azColName) {
    int itemid;
    int i, iTmp;
    for (i = 0; i < argc; ++i) {
        if (!strcmp(azColName[i], "itemid")) break;
    }
    sscanf(argv[i], "%d", &itemid);
    if (!itemid || itemid > items_n) {
        glutHideWindow();
        MessageBox(HWND_DESKTOP, "There was an error in reading items database.\nItem ID appears to be invalid!", "The Outcast - Fatal Error", MB_ICONSTOP);
        exit(1);
    }

    ItemClear(items + itemid);

    for (i = 0; i < argc; ++i) {
        if (!strcmp(azColName[i], "splash")) {
            sscanf(argv[i], "%d", &iTmp);
            if (iTmp) items[itemid].splash = true;
            //if (items[itemid].splash) printf("SPLASH ITEM %d\n", itemid);
        }
        if (!strcmp(azColName[i], "fluidcontainer")) {
            sscanf(argv[i], "%d", &iTmp);
            if (iTmp) items[itemid].fluidcontainer = true;
            //if (items[itemid].fluidcontainer) printf("FLUIDCONTAINER ITEM %d\n", itemid);
        }
        if (!strcmp(azColName[i], "stackable")) {
            sscanf(argv[i], "%d", &iTmp);
            if (iTmp) items[itemid].stackable = true;
            //if (items[itemid].stackable) printf("STACKABLE ITEM %d\n", itemid);
        }
        if (!strcmp(azColName[i], "spritelist")) {

            strcpy(items[itemid].spritelist, argv[i]);
            /*if (itemid==101) {
                printf("%s\n", items[itemid].spritelist);
                system("pause");
            }*/
        }


    }
    return 0;
}
static int ItemsLoadNumFunc(void *NotUsed, int argc, char **argv, char **azColName) {
    //printf("A result!\n");
    items_n = atoi(argv[0]);
    //printf("%d\n", items_n);
    return 0;
}
void ItemsLoad() {

    GWLogon_Status(&((GM_MainMenu*)game)->charlist, "Fetching item properties...");


    items_n = 0;
    dbExecPrintf(dbData, "select max(itemid) from items%d;", ItemsLoadNumFunc, 0, NULL, protocol->GetProtocolVersion());
    printf("%d items in database for protocol %d\n", items_n, protocol->GetProtocolVersion());
    if (!items_n) {
        glutHideWindow();
        MessageBox(HWND_DESKTOP, "There was an error in reading items database.\nIt appears that current protocol has no items in database.\nPlease reinstall!", "The Outcast - Fatal Error", MB_ICONSTOP);
        exit(1);
    }
    items = (item_t*)malloc(sizeof(item_t)*(items_n+1));

    ItemClear(items); // hurz was bugged for a long time and carries item 0 in inventory .. so lets be smarter than tibia client and allow item 0 ... ;)
    dbExecPrintf(dbData, "select * from items%d;", ItemsLoadFunc, 0, NULL, protocol->GetProtocolVersion());

    GWLogon_Status(&((GM_MainMenu*)game)->charlist, "Entering game...");
    //system("pause");

}
