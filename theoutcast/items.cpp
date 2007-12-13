#include "database.h"
#include "protocol.h"
#include "types.h"
#include "windowing.h"
#include "debugprint.h"
#include "sprfmts.h"
#include "util.h"
int items_n;
item_t **items=NULL;
void GWLogon_Status(glictMessageBox* mb, const char* txt);

void ItemClear(item_t* item) {

    item->graphics[0] = 0;
    item->graphics2d[0] = 0;
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
    item->rune = false;
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
    item->extraproperty = 0;
    item->ladder = false;
    item->spritelist[0] = 0;
    item->otid = 0;
/*
	if (item->textures)
		for (int i=0;i<item->sli.numsprites;i++)
            delete ((Texture**)item->textures)[i];*/

    /*else
        printf("item->textures is NULL!\n");*/

    if (item->textures) free(item->textures);
    item->sli.numsprites = 0;
    item->textures = NULL;
    // sli does not really need to be cleared ... it will be rebuilt each time
    // same for animcount

    item->loaded = false;
}

void ItemInit(item_t *item) {
    item->textures = NULL;
    item->sli.numsprites = 0;
    ItemClear(item);
}

static int ItemsLoadFunc(void *NotUsed, int argc, char **argv, char **azColName) {
    int itemid;
    int i, iTmp;
    for (i = 0; i < argc; ++i) {
        if (!strcmp(azColName[i], "itemid")) break;
    }
    sscanf(argv[i], "%d", &itemid);
    if (!itemid || itemid > items_n) {
        //glutHideWindow();
        //MessageBox(HWND_DESKTOP, "There was an error in reading items database.\nItem ID appears to be invalid!", "The Outcast - Fatal Error", MB_ICONSTOP);
        exit(1);
    }

    ItemClear(*(items + itemid));

    items[itemid]->loaded = true;

    for (i = 0; i < argc; ++i) {
        if (!strcmp(azColName[i], "ground")) {
            sscanf(argv[i], "%d", &iTmp);
            if (iTmp) items[itemid]->ground = true;
        }
        if (!strcmp(azColName[i], "topindex")) {
            sscanf(argv[i], "%d", &iTmp);
            items[itemid]->topindex = iTmp;
        }

        if (!strcmp(azColName[i], "splash")) {
            sscanf(argv[i], "%d", &iTmp);
            if (iTmp) items[itemid]->splash = true;

        }
        if (!strcmp(azColName[i], "fluidcontainer")) {
            sscanf(argv[i], "%d", &iTmp);
            if (iTmp) items[itemid]->fluidcontainer = true;
            //if (items[itemid].fluidcontainer) printf("FLUIDCONTAINER ITEM %d\n", itemid);
        }
        if (!strcmp(azColName[i], "stackable")) {
            sscanf(argv[i], "%d", &iTmp);
            if (iTmp) items[itemid]->stackable = true;
            //if (items[itemid].stackable) printf("STACKABLE ITEM %d\n", itemid);
        }
        if (!strcmp(azColName[i], "rune")) {
            sscanf(argv[i], "%d", &iTmp);
            if (iTmp) items[itemid]->rune = true;
            //if (items[itemid].stackable) printf("RUNE ITEM %d\n", itemid);
        }
        if (!strcmp(azColName[i], "usable")) {
            sscanf(argv[i], "%d", &iTmp);
            items[itemid]->usable = iTmp;
            //if (items[itemid].splash) printf("SPLASH ITEM %d\n", itemid);
        }

        if (!strcmp(azColName[i], "spritelist")) {
            strcpy(items[itemid]->spritelist, argv[i]);
        }
        if (!strcmp(azColName[i], "height2d_x")) {
            sscanf(argv[i], "%d", &iTmp);
            items[itemid]->height2d_x = iTmp;
        }
        if (!strcmp(azColName[i], "height2d_y")) {
            sscanf(argv[i], "%d", &iTmp);
            items[itemid]->height2d_y = iTmp;
        }
        if (!strcmp(azColName[i], "speedindex")) {
            sscanf(argv[i], "%d", &iTmp);
            items[itemid]->speedindex = iTmp;
        }
        if (!strcmp(azColName[i], "extraproperty")) {
            sscanf(argv[i], "%d", &iTmp);
            items[itemid]->extraproperty = iTmp;
        }

        if (!strcmp(azColName[i], "ladder")) {
            sscanf(argv[i], "%d", &iTmp);
            items[itemid]->ladder = iTmp;
            //if (items[itemid].splash) printf("SPLASH ITEM %d\n", itemid);
        }

        if (!strcmp(azColName[i], "minimapcolor")) {
            sscanf(argv[i], "%d", &iTmp);
            items[itemid]->minimapcolor = iTmp;
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


    switch (protocol->GetProtocolVersion()) {
        case 750:
            SPRLoader("Tibia75.spr");
            break;

        case 760:
        case 770:
            SPRLoader("Tibia76.spr");
            break;
        case 790:
            SPRLoader("Tibia79.spr");
            break;
        case 792:
            SPRLoader("Tibia792.spr");
            break;
        case 800:
            SPRLoader("Tibia80.spr");
            break;
        case 810:
            SPRLoader("Tibia81.spr");
            break;
        default:
            printf("!(Y$*#)QY$()!$(!&#)($\n");
            system("pause");
    }

    GWLogon_Status(&((GM_MainMenu*)game)->charlist, "Fetching item properties...");


    items_n = 0;
    dbExecPrintf(dbData, ItemsLoadNumFunc, 0, NULL, "select max(itemid) from items%d;", protocol->GetProtocolVersion());
    DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "%d items in database for protocol %d\n", items_n, protocol->GetProtocolVersion());
    if (!items_n) {
        //glutHideWindow();
        //MessageBox(HWND_DESKTOP, "There was an error in reading items database.\nIt appears that current protocol has no items in database.\nPlease reinstall!", "The Outcast - Fatal Error", MB_ICONSTOP);
        exit(1);
    }

    items = (item_t**)malloc(sizeof(item_t*)*(items_n+1));

    for (int i = 0; i < items_n+1; i++) {
        items[i] = new item_t;
        ItemInit(items[i]);
    }

    dbExecPrintf(dbData, ItemsLoadFunc, 0, NULL, "select * from items%d;", protocol->GetProtocolVersion());

    GWLogon_Status(&((GM_MainMenu*)game)->charlist, "Entering game...");
    //system("pause");

}

void ItemsLoad_NoUI(unsigned int protocolversion) {
    items_n = 0;
    dbExecPrintf(dbData, ItemsLoadNumFunc, 0, NULL, "select max(itemid) from items%d;", protocolversion);
    DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "%d items in database for protocol %d\n", items_n, protocolversion);
    if (!items_n) {
        //glutHideWindow();

        NativeGUIError("There was an error in reading items database.\nIt appears that current protocol has no items in database.\nPlease reinstall!", "The Outcast - Fatal Error");
        exit(1);
    }
    items = (item_t**)malloc(sizeof(item_t*)*(items_n+1));

    for (int i = 0; i < items_n+1; i++) {
        items[i] = new item_t;
        ItemInit(items[i]);
    }

    dbExecPrintf(dbData, ItemsLoadFunc, 0, NULL, "select * from items%d;", protocolversion);

}

void ItemsUnload() {

    for (int i=100;i<items_n;i++) {
        ItemClear(*(items + i));
        delete *(items + i);
    }
    free(items);
}
