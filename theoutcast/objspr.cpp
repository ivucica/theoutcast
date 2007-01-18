#include "assert.h"
#include "objspr.h"
#include "items.h"
#include "types.h"
#include "simple_effects.h"
ObjSpr::ObjSpr() {
    memset(&sli, 0, sizeof(sli));
}
ObjSpr::ObjSpr(unsigned int itemid) {
    //printf("Loading sprite for item %d\n", itemid);
    memset(&sli, 0, sizeof(sli));
    LoadItem(itemid);
    //printf("OMFG sprite loaded WTF\n", itemid);
}
ObjSpr::~ObjSpr() {
    if (sli.spriteids)
        free(sli.spriteids);
}
bool ObjSpr::Render() {

    glEnable(GL_TEXTURE_2D);
    for (int i = 0; i < sli.height; i++)
        for (int j = 0; j < sli.width; j++) {
            t[i*sli.width + j]->Bind();
            StillEffect(-32*j, 32*i, 32 - 32*j, 32 + 32*i, 2, 2); // divisions were 40 10
        }

    glDisable(GL_TEXTURE_2D);
}
void ObjSpr::LoadItem(unsigned int itemid) {
//    ASSERTFRIENDLY(items, "Item list, including SPR definition, is NULL in ObjSpr::LoadItem.")


    printf("Spritelist: %s\n", items[itemid].spritelist);
    if (!strlen(items[itemid].spritelist)) return;
    char *p = items[itemid].spritelist;
    sscanf(p, "%d", &sli.width); p = strchr(p, ' ')+1;
    printf("Width %d\n", sli.width);
    sscanf(p, "%d", &sli.height); p = strchr(p, ' ')+1;
    printf("Height %d\n", sli.height);
    sscanf(p, "%d", &sli.blendframes); p = strchr(p, ' ')+1;
    printf("Blendframes %d\n", sli.blendframes);
    sscanf(p, "%d", &sli.xdiv); p = strchr(p, ' ')+1;
    printf("Xdiv %d\n", sli.xdiv);
    sscanf(p, "%d", &sli.ydiv); p = strchr(p, ' ')+1;
    printf("Ydiv %d\n", sli.ydiv);
    sscanf(p, "%d", &sli.animcount); p = strchr(p, ' ')+1;
    printf("Animcount %d\n", sli.animcount);
    sscanf(p, "%d", &sli.unknown); p = strchr(p, ' ')+1;
    printf("Unknown %d\n", sli.unknown);
    sscanf(p, "%d", &sli.numsprites); p = strchr(p, ' ')+1;
    printf("Numsprites %d\n", sli.numsprites);

    // now shall we read sprite ids
    // beware! :D
    if (sli.spriteids)
        free(sli.spriteids);

    sli.spriteids = (unsigned short*)malloc(sli.numsprites * sizeof(unsigned short));
    t = (Texture**)malloc(sli.numsprites * sizeof(Texture*));
    for (int i = 0; i < sli.numsprites; i++) {
        sscanf(p, "%d", &sli.spriteids[i]); p = strchr(p, ' ')+1;
        printf("Spriteid %d\n", sli.spriteids[i]);

        t[i] = new Texture("tibia76.spr", sli.spriteids[i]);
        printf("End of texloading\n", sli.width);
    }


}
