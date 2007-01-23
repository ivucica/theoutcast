#include "assert.h"
#include "objspr.h"
#include "items.h"
#include "types.h"
#include "simple_effects.h"
#include "defines.h"
ObjSpr::ObjSpr() {
    memset(&sli, 0, sizeof(sli));
}
ObjSpr::ObjSpr(unsigned int itemid) {
    //printf("Loading sprite for item %d\n", itemid);
    memset(&sli, 0, sizeof(sli));
    LoadItem(itemid);
    this->itemid = itemid;
    //printf("OMFG sprite loaded WTF\n", itemid);
}
ObjSpr::~ObjSpr() {
    if (sli.spriteids)  {
        free(sli.spriteids);
        for (int i = 0 ; i < sli.numsprites; i++) {
            delete(t[i]);
        }
        free(t);
    }
}
bool ObjSpr::Render() {
    position_t p;
    p.x = 0; p.y = 0; p.z = 0;
    return Render(&p);

}
bool ObjSpr::Render(position_t *pos) {
    glEnable(GL_TEXTURE_2D);
    for (int i = 0; i < sli.height; i++)
        for (int j = 0; j < sli.width; j++) {

            if (sli.xdiv>1 && sli.ydiv>1)
                t[((i*sli.width + j) * sli.ydiv + pos->y % sli.ydiv) * sli.xdiv + pos->x % sli.xdiv]->Bind();
            //else if (sli.xdiv > 1)
            //    t[(i*sli.width + j) * sli.xdiv + pos->x % sli.xdiv]->Bind();
            //else if (sli.ydiv > 1)
            //    t[(i*sli.width + j) * sli.ydiv + pos->y % sli.ydiv]->Bind();

            else
                t[i*sli.width + j]->Bind();
            StillEffect(-32*j, 32*i, 32 - 32*j, 32 + 32*i, 2, 2); // divisions were 40 10
      //      printf("%d ", sli.spriteids[i]);
        }
    //printf("\n");

    glDisable(GL_TEXTURE_2D);
    return true;
}
bool ObjSpr::Render(unsigned char stackcount) {
    glEnable(GL_TEXTURE_2D);
    t[min(stackcount,sli.numsprites-1)]->Bind();
    StillEffect(0, 0, 32, 32, 2, 2); // divisions were 40 10
    glDisable(GL_TEXTURE_2D);
    return true;
}
void ObjSpr::LoadItem(unsigned int itemid) {
//    ASSERTFRIENDLY(items, "Item list, including SPR definition, is NULL in ObjSpr::LoadItem.")
    //printf("%d\n", items_n);
    ASSERTFRIENDLY(itemid >= 100 && itemid <= items_n, "invalid itemid in ObjSpr::LoadItem");
    ASSERTFRIENDLY(items[itemid].loaded, "itemid not loaded");

    //printf("Spritelist for %d: ", itemid);
    //if (strlen(items[itemid].spritelist)>200) system("pause");
    //printf("%s\n", items[itemid].spritelist);
    if (!strlen(items[itemid].spritelist)) return;
    char *p = items[itemid].spritelist;
    sscanf(p, "%hhd", &sli.width); p = strchr(p, ' ')+1;
    //printf("Width %d\n", sli.width);
    sscanf(p, "%hhd", &sli.height); p = strchr(p, ' ')+1;
    //printf("Height %d\n", sli.height);
    sscanf(p, "%hhd", &sli.blendframes); p = strchr(p, ' ')+1;
    //printf("Blendframes %d\n", sli.blendframes);
    sscanf(p, "%hhd", &sli.xdiv); p = strchr(p, ' ')+1;
    //printf("Xdiv %d\n", sli.xdiv);
    sscanf(p, "%hhd", &sli.ydiv); p = strchr(p, ' ')+1;
    //printf("Ydiv %d\n", sli.ydiv);
    sscanf(p, "%hhd", &sli.animcount); p = strchr(p, ' ')+1;
    //printf("Animcount %d\n", sli.animcount);
    sscanf(p, "%hhd", &sli.unknown); p = strchr(p, ' ')+1;
    //printf("Unknown %d\n", sli.unknown);
    sscanf(p, "%hd", &sli.numsprites); p = strchr(p, ' ')+1;
    //printf("Numsprites %d\n", sli.numsprites);

    // now shall we read sprite ids
    // beware! :D
    if (sli.spriteids)
        free(sli.spriteids);

    sli.spriteids = (unsigned short*)malloc(sli.numsprites * sizeof(unsigned short));
    t = (Texture**)malloc(sli.numsprites * sizeof(Texture*));
    for (int i = 0; i < sli.numsprites; i++) {
        //printf("Reading sprite id number %d in item %d\n", i, itemid);
        //printf("Reading from %s\n", p);
        sscanf(p, "%hd", &sli.spriteids[i]); p = strchr(p, ' ')+1;
        //printf("Spriteid %d\n", sli.spriteids[i]);

        t[i] = new Texture("tibia76.spr", sli.spriteids[i]);
        //printf("End of texloading\n", sli.width);
    }


}
