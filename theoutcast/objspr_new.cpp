#ifdef OBJSPR_NEW
#warning Using EXPERIMENTAL objspr!


#include "assert.h"
#include "objspr.h"
#include "items.h"
#include "creatures.h"
#include "effects.h"
#include "types.h"
#include "simple_effects.h"
#include "defines.h"
#include "protocol.h"//remove me and replace the switch(protocol->GetPRotcoolVersion) with something that'll return the spr filename
ObjSpr::ObjSpr() {
}
ObjSpr::ObjSpr(unsigned int itemid, unsigned char type, unsigned int protocolversion) { // no network version
    memset(&sli, 0, sizeof(sli));
    offsetx = 0;
    offsety = 0;
    this->t = NULL;
    if (type == 0)
        LoadItem(itemid, protocolversion);
    else if (type == 1)
        LoadCreature(itemid, protocolversion);
    else if (type == 2)
        LoadEffect(itemid, protocolversion);
    else
        exit(1);
    this->itemid = itemid;
    this->type = type;
    this->direction = NORTH;

}
ObjSpr::ObjSpr(unsigned int itemid, unsigned char type) {
    memset(&sli, 0, sizeof(sli));
    offsetx = 0;
    offsety = 0;
    this->t = NULL;
    if (type == 0)
        LoadItem(itemid);
    else if (type == 1)
        LoadCreature(itemid);
    else if (type == 2)
        LoadEffect(itemid);
    else
        exit(1);
    this->itemid = itemid;
    this->type = type;
    this->direction = NORTH;
}

ObjSpr::ObjSpr(unsigned int creaturetype, unsigned char head, unsigned char body, unsigned char legs, unsigned char feet) {
    memset(&sli, 0, sizeof(sli));
    offsetx = 0;
    offsety = 0;
    this->t = NULL;
    LoadCreature(creaturetype, head, body, legs, feet);
    this->itemid = creaturetype;
    this->type = 1;
    this->direction = NORTH;
}
ObjSpr::ObjSpr(unsigned int creaturetype, unsigned int protocolversion, unsigned char head, unsigned char body, unsigned char legs, unsigned char feet) {
    memset(&sli, 0, sizeof(sli));
    offsetx = 0;
    offsety = 0;
    this->t = NULL;
    LoadCreature(creaturetype, protocolversion, head, body, legs, feet);
    this->itemid = creaturetype;
    this->type = 1;
    this->direction = NORTH;
}



ObjSpr::~ObjSpr() {

}
bool ObjSpr::Render() {
    position_t p;
    p.x = 0; p.y = 0; p.z = 0;
    return Render(&p);
}
bool ObjSpr::Render(position_t *pos) {
    glEnable(GL_TEXTURE_2D);
    int currentframe = (int)((animation_percent/100.) * (float)sli.animcount);
    int activeframe;



    for (int i = 0; i < sli.height; i++)
        for (int j = 0; j < sli.width; j++) {
            for (int k = 0; k < (type ? min(1, sli.blendframes) : sli.blendframes); k++) { // if anything except item, there won't be blendframes...


                switch (type) {
                    case 0: // item
                        activeframe =   (((((( // same amount of ('s as of *'s
                                        currentframe)
                                        * sli.ydiv + pos->y % sli.ydiv)
                                        * sli.xdiv + pos->x % sli.xdiv)
                                        * sli.blendframes + k)  // k == subblendframes  (stacked sprite)
                                        * sli.height + i)           // i == subheight       (y coordinate)
                                        * sli.width + j)        // j == subwidth        (x coordinate)

                                        ;
                        break;
                    case 1: // creature
                        activeframe =   (((((( // same amount of ('s as of *'s
                                        currentframe)
                                        * sli.ydiv + 0)//pos->y % sli.ydiv)
                                        * sli.xdiv + direction)//pos->x % sli.xdiv)
                                        * sli.blendframes + k)  // k == subblendframes  (stacked sprite)
                                        * sli.height + i)           // i == subheight       (y coordinate)
                                        * sli.width + j)        // j == subwidth        (x coordinate)

                                        ;
                        break;
                    case 2: // effect
                        activeframe =   (((((( // same amount of ('s as of *'s
                                        currentframe)
                                        * sli.ydiv + pos->y % sli.ydiv)
                                        * sli.xdiv + pos->x % sli.xdiv)
                                        * sli.blendframes + k)  // k == subblendframes  (stacked sprite)
                                        * sli.height + i)           // i == subheight       (y coordinate)
                                        * sli.width + j)        // j == subwidth        (x coordinate)

                                        ;
                        break;
                    default:
                        ASSERTFRIENDLY(false, "Unexpected code path encountered in ObjSpr::Render()");
                }

                //printf("w %d h %d xdiv %d ydiv %d blendframes %d animcount %d unknown %d\n", sli.width, sli.height, sli.xdiv, sli.ydiv, sli.blendframes, sli.animcount, sli.unknown);
                {
                    char tmp[512];
                    sprintf(tmp, "Active frame is %d while number of frames is %d. And that is a problem.", activeframe, sli.numsprites);
                    ASSERTFRIENDLY(activeframe < sli.numsprites, tmp )
                    //printf("%s\n", tmp);
                }
                if (activeframe < sli.numsprites) t[activeframe]->Bind();

                StillEffect(-32*j, 32*i, 32 - 32*j, 32 + 32*i, 2, 2, false, false, true); // divisions were 40 10
            }
        }
    //printf("\n");
    glTranslatef(-offsetx, offsetx, 0);
    glDisable(GL_TEXTURE_2D);
    return true;
}
bool ObjSpr::Render(unsigned char stackcount) {
    glEnable(GL_TEXTURE_2D);
    t[stackcount % sli.numsprites]->Bind();

    //glTranslatef(-offsetx, -offsety, 0);
    StillEffect(0, 0, 32 , 32 , 2, 2, false, false, true); // divisions were 40 10

    glDisable(GL_TEXTURE_2D);
    return true;
}
void ObjSpr::LoadCreature(unsigned int creatureid) {
    LoadCreature(creatureid, protocol->GetProtocolVersion());

}
void ObjSpr::LoadCreature(unsigned int creatureid, unsigned int protocolversion) {
    LoadCreature(creatureid, protocolversion, 20,30,40,50);
}
void ObjSpr::LoadCreature(unsigned int creatureid, unsigned char head, unsigned char body, unsigned char legs, unsigned char feet ) {
    LoadCreature(creatureid, protocol->GetProtocolVersion(), head, body, legs, feet);
}
void ObjSpr::LoadCreature(unsigned int creatureid, unsigned int protocolversion, unsigned char head, unsigned char body, unsigned char legs, unsigned char feet ) {
    char temp[512];
    sprintf(temp, "invalid creatureid %d out of %d in ObjSpr::LoadCreature", creatureid, creatures_n-1);
    ASSERTFRIENDLY(creatureid <= creatures_n-1, temp);


}
void ObjSpr::LoadItem(unsigned int itemid) {
    LoadItem(itemid, protocol->GetProtocolVersion());
}
void ObjSpr::LoadItem(unsigned int itemid, unsigned int protocolversion) {
    char temp[512];
    sprintf(temp, "invalid itemid %d out of %d in ObjSpr::LoadItem", itemid, items_n);
    ASSERTFRIENDLY(!itemid || itemid >= 100 && itemid < items_n+1, temp);

    ASSERT(!itemid || items[itemid]->loaded);
    if (!itemid) return;
    if (!strlen(items[itemid]->spritelist)) return;
    char *p = items[itemid]->spritelist;
    sscanf(p, "%hhd", &sli.width); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.height); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.blendframes); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.xdiv); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.ydiv); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.unknown); p = strchr(p, ' ')+1;
    sscanf(p, "%hhd", &sli.animcount); p = strchr(p, ' ')+1;
    sscanf(p, "%hd", &sli.numsprites); p = strchr(p, ' ')+1;

    sli.width = 1; sli.height = 1; sli.blendframes = 1; sli.xdiv = 1; sli.ydiv = 1; sli.unknown = 1; sli.animcount = 1; sli.numsprites = 1;

    sli.spriteids = (unsigned short*)malloc(sizeof(unsigned short));
    t = (Texture**)malloc(sizeof(Texture*));
    sscanf(p, "%hd", &sli.spriteids[0]); p = strchr(p, ' ')+1;

    t[0] = new Texture("Tibia792.spr", sli.spriteids[0]);
}

void ObjSpr::LoadEffect(unsigned int effectid) {
    LoadEffect(effectid, protocol->GetProtocolVersion());
}
void ObjSpr::LoadEffect(unsigned int effectid, unsigned int protocolversion) {
    char temp[512];
    sprintf(temp, "invalid effectid %d out of %d in ObjSpr::LoadEffect", effectid, effects_n);
    ASSERTFRIENDLY(effectid <= effects_n, temp);

}

void ObjSpr::SetDirection(direction_t dir) {
    direction = dir;
}
#endif