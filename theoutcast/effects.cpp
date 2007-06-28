#include "database.h"
#include "protocol.h"
#include "types.h"
#include "windowing.h"
#include "debugprint.h"
int effects_n;
effect_t **effects=NULL;
void GWLogon_Status(glictMessageBox* mb, const char* txt);

void EffectClear(effect_t* effect) {

    effect->graphics[0] = 0;
    effect->graphics2d[0] = 0;
    effect->spritelist[0] = 0;


    if (effect->textures) for (int i=0;i<effect->sli.numsprites;i++)
        if (((Texture**)effect->textures)[i]) delete ((Texture**)effect->textures)[i];
    if (effect->textures) free(effect->textures);
    effect->textures = NULL;

    effect->loaded = false;

}
void EffectInit(effect_t* effect) {

    effect->textures = NULL;
    effect->sli.numsprites = 0;

    EffectClear(effect);
}


static int EffectsLoadFunc(void *NotUsed, int argc, char **argv, char **azColName) {
    int effectid;
    int i, iTmp;
    for (i = 0; i < argc; ++i) {
        if (!strcmp(azColName[i], "effectid")) break;
    }
    sscanf(argv[i], "%d", &effectid);
    if (effectid > effects_n) {
        //glutHideWindow();
        //MessageBox(HWND_DESKTOP, "There was an error in reading effects database.\nEffect ID appears to be invalid!", "The Outcast - Fatal Error", MB_ICONSTOP);
        exit(1);
    }


    EffectClear(*(effects + effectid));

    effects[effectid]->loaded = true;



    for (i = 0; i < argc; ++i) {
        if (!strcmp(azColName[i], "spritelist")) {

            strcpy(effects[effectid]->spritelist, argv[i]);
        }

    }

    return 0;
}
static int EffectsLoadNumFunc(void *NotUsed, int argc, char **argv, char **azColName) {
    effects_n = atoi(argv[0]);
    return 0;
}
void EffectsLoad() {
    GWLogon_Status(&((GM_MainMenu*)game)->charlist, "Fetching effect properties...");

    effects_n = 0;
    dbExecPrintf(dbData, EffectsLoadNumFunc, 0, NULL, "select max(effectid) from effects%d;", protocol->GetProtocolVersion());
    DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "%d effects in database for protocol %d\n", effects_n, protocol->GetProtocolVersion());
    if (!effects_n) {
        //glutHideWindow();
        //MessageBox(HWND_DESKTOP, "There was an error in reading effects database.\nIt appears that current protocol has no effects in database.\nPlease reinstall!", "The Outcast - Fatal Error", MB_ICONSTOP);
        exit(1);
    }
    effects = (effect_t**)malloc(sizeof(effect_t*)*(effects_n+1));

    for (int i = 0; i < effects_n+1; i++) {

        effects[i] = new effect_t;
        EffectInit(effects[i]);

    }
    dbExecPrintf(dbData, EffectsLoadFunc, 0, NULL, "select * from effects%d;", protocol->GetProtocolVersion());

    GWLogon_Status(&((GM_MainMenu*)game)->charlist, "Entering game...");
    //system("pause");
}
void EffectsLoad_NoUI(unsigned int protocolversion) {
    effects_n = 0;
    dbExecPrintf(dbData, EffectsLoadNumFunc, 0, NULL, "select max(effectid) from effects%d;", protocolversion);
    DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "%d effects in database for protocol %d\n", effects_n, protocolversion);
    if (!effects_n) {
        //glutHideWindow();
        //MessageBox(HWND_DESKTOP, "There was an error in reading effects database.\nIt appears that current protocol has no effects in database.\nPlease reinstall!", "The Outcast - Fatal Error", MB_ICONSTOP);
        exit(1);
    }
    effects = (effect_t**)malloc(sizeof(effect_t*)*(effects_n+1));

    for (int i = 0; i < effects_n+1; i++) {
        effects[i] = new effect_t;
        EffectInit(effects[i]);
    }
    dbExecPrintf(dbData, EffectsLoadFunc, 0, NULL, "select * from effects%d;", protocolversion);

}

void EffectsUnload() {
    for (int i=0;i<effects_n;i++) {
        EffectClear(*(effects + i));
        delete *(effects + i);
    }
    free(effects);
    effects = NULL;
}
