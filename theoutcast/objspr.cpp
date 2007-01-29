#include "assert.h"
#include "objspr.h"
#include "items.h"
#include "creatures.h"
#include "types.h"
#include "simple_effects.h"
#include "defines.h"
ObjSpr::ObjSpr() {
    memset(&sli, 0, sizeof(sli));
}
ObjSpr::ObjSpr(unsigned int itemid, unsigned char type) {
    memset(&sli, 0, sizeof(sli));
    if (type == 0)
        LoadItem(itemid);
    else if (type == 1)
        LoadCreature(itemid);
    else
        exit(1);
    this->itemid = itemid;
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
    int currentframe = (animation_percent/100.) * (float)sli.animcount;
   // if (sli.animcount!=1) printf("%f - %d out of %d; %g * %g\n", animation_percent, currentframe, sli.animcount, (animation_percent/100.), (float)sli.animcount);
   // if (itemid == 5022) printf("%s, %d\n",  items[itemid].spritelist, sli.animcount);
    for (int i = 0; i < sli.height; i++)
        for (int j = 0; j < sli.width; j++) {
            int activeframe;
            if (sli.xdiv>1 && sli.ydiv>1)
                activeframe = currentframe *  sli.width * sli.height * sli.xdiv * sli.ydiv * sli.blendframes + ((i*sli.width + j) * sli.ydiv + pos->y % sli.ydiv) * sli.xdiv + pos->x % sli.xdiv;
            else
                activeframe = currentframe *  sli.width * sli.height * sli.xdiv * sli.ydiv * sli.blendframes + i*sli.width + j;
            ASSERT(activeframe < sli.numsprites )
            t[activeframe]->Bind();

            StillEffect(-32*j, 32*i, 32 - 32*j, 32 + 32*i, 2, 2); // divisions were 40 10

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
void ObjSpr::LoadCreature(unsigned int creatureid) {
    ASSERTFRIENDLY(creatureid <= creatures_n-1, "invalid creatureid in ObjSpr::LoadCreature");
    ASSERTFRIENDLY(creatures[creatureid].loaded, "creature not loaded");

    if (!strlen(creatures[creatureid].spritelist)) return;
    char *p = creatures[creatureid].spritelist;
    sscanf(p, "%hhd", &sli.width); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.height); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.blendframes); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.xdiv); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.ydiv); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.unknown); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.animcount); p = strchr(p, ' ')+1;
    sscanf(p, "%hd", &sli.numsprites); p = strchr(p, ' ')+1;

    // now shall we read sprite ids
    // beware! :D
    if (sli.spriteids)
        free(sli.spriteids);

    sli.spriteids = (unsigned short*)malloc(sli.numsprites * sizeof(unsigned short));
    t = (Texture**)malloc(sli.numsprites * sizeof(Texture*));
    for (int i = 0; i < sli.numsprites; i++) {
        sscanf(p, "%hd", &sli.spriteids[i]); p = strchr(p, ' ')+1;

        t[i] = new Texture("tibia76.spr", sli.spriteids[i]);
    }
    animation_framecount[0] = 1; //stand
    animation_framecount[1] = sli.animcount; //walk

}
void ObjSpr::LoadItem(unsigned int itemid) {
    ASSERTFRIENDLY(itemid >= 100 && itemid <= items_n, "invalid itemid in ObjSpr::LoadItem");
    ASSERTFRIENDLY(items[itemid].loaded, "itemid not loaded");

    if (!strlen(items[itemid].spritelist)) return;
    char *p = items[itemid].spritelist;
    sscanf(p, "%hhd", &sli.width); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.height); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.blendframes); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.xdiv); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.ydiv); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.unknown); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.animcount); p = strchr(p, ' ')+1;
    sscanf(p, "%hd", &sli.numsprites); p = strchr(p, ' ')+1;

    // now shall we read sprite ids
    // beware! :D
    if (sli.spriteids)
        free(sli.spriteids);

    sli.spriteids = (unsigned short*)malloc(sli.numsprites * sizeof(unsigned short));
    t = (Texture**)malloc(sli.numsprites * sizeof(Texture*));
    for (int i = 0; i < sli.numsprites; i++) {
        sscanf(p, "%hd", &sli.spriteids[i]); p = strchr(p, ' ')+1;

        t[i] = new Texture("tibia76.spr", sli.spriteids[i]);
    }
    animation_framecount[0] = sli.animcount;// stand
    animation_framecount[1] = sli.animcount;// walk
}
