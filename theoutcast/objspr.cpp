#include "assert.h"
#include "objspr.h"
#include "items.h"
#include "types.h"
#include "simple_effects.h"
ObjSpr::ObjSpr() {
}
ObjSpr::ObjSpr(unsigned int itemid) {
    LoadItem(itemid);
}
ObjSpr::~ObjSpr() {
    if (sli.spriteids) free(sli.spriteids);
}
bool ObjSpr::Render() {

    StillEffect(200, 0, 425, 100, 2, 2); // divisions were 40 10
}
void ObjSpr::LoadItem(unsigned int itemid) {
//    ASSERTFRIENDLY(items, "Item list, including SPR definition, is NULL in ObjSpr::LoadItem.")


    char *p = items[itemid].spritelist;
    sscanf(p, "%d", &sli.width); p = strchr(p, ' ');
    sscanf(p, "%d", &sli.height); p = strchr(p, ' ');
    sscanf(p, "%d", &sli.blendframes); p = strchr(p, ' ');
    sscanf(p, "%d", &sli.xdiv); p = strchr(p, ' ');
    sscanf(p, "%d", &sli.ydiv); p = strchr(p, ' ');
    sscanf(p, "%d", &sli.animcount); p = strchr(p, ' ');
    sscanf(p, "%d", &sli.unknown); p = strchr(p, ' ');
    sscanf(p, "%d", &sli.numsprites); p = strchr(p, ' ');

    // now shall we read sprite ids
    // beware! :D
    if (sli.spriteids) free(sli.spriteids);
    sli.spriteids = (unsigned short*)malloc(sli.numsprites * sizeof(unsigned short));
    t = (Texture**)malloc(sli.numsprites * sizeof(Texture*));
    for (int i = 0; i < sli.numsprites; i++) {
        sscanf(p, "%d", &sli.spriteids[i]); p = strchr(p, ' ');
        t[i] = new Texture("tibia76.spr", sli.spriteids[i]);
    }


}
