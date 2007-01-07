#include "database.h"
#include "protocol.h"
int items_n;

void GWLogon_Status(glictMessageBox* mb, const char* txt);


static int ItemsLoadFunc(void *NotUsed, int argc, char **argv, char **azColName) {
    printf("OK\n");
    printf("REZULTIRA: %s\n", argv[0]);
    return 0;
}
static int ItemsLoadNumFunc(void *NotUsed, int argc, char **argv, char **azColName) {
    sscanf(argv[0], "%d", &items_n);
    return 0;
}
void ItemsLoad() {

    GWLogon_Status(&((GM_MainMenu*)game)->charlist, "Fetching item properties...");

    items_n = 0;
    dbExecPrintf(dbData, "select max(itemid) from items where protocolversion = '%d';", ItemsLoadNumFunc, 0, NULL, protocol->GetProtocolVersion());
    printf("%d items in database for protocol %d\n", items_n, protocol->GetProtocolVersion());
    if (!items_n) {
        MessageBox(HWND_DESKTOP, "There was an error in reading items database.", "The Outcast - Fatal Error", MB_ICONSTOP);
        exit(1);
    }

    dbExecPrintf(dbData, "select * from items where protocolversion = '%d';", ItemsLoadFunc, 0, NULL, protocol->GetProtocolVersion());

    GWLogon_Status(&((GM_MainMenu*)game)->charlist, "Entering game...");
    //system("pause");

}
