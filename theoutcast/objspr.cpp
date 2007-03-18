#include "assert.h"
#include "objspr.h"
#include "items.h"
#include "creatures.h"
#include "types.h"
#include "simple_effects.h"
#include "defines.h"
#include "protocol.h"//remove me and replace the switch(protocol->GetPRotcoolVersion) with something that'll return the spr filename
ObjSpr::ObjSpr() {
    memset(&sli, 0, sizeof(sli));
    direction = NORTH;
}
ObjSpr::ObjSpr(unsigned int itemid, unsigned char type, unsigned int protocolversion) { // no network version
    memset(&sli, 0, sizeof(sli));
    offsetx = 0;
    offsety = 0;
    if (type == 0)
        LoadItem(itemid, protocolversion);
    else if (type == 1)
        LoadCreature(itemid, protocolversion);
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
    if (type == 0)
        LoadItem(itemid);
    else if (type == 1)
        LoadCreature(itemid);
    else
        exit(1);
    this->itemid = itemid;
    this->type = type;
    this->direction = NORTH;

}
ObjSpr::~ObjSpr() {
    /*if (sli.spriteids)  {
        free(sli.spriteids);
        for (int i = 0 ; i < sli.numsprites; i++) {
            delete(t[i]);
        }
        free(t);
    }*/
}
bool ObjSpr::Render() {
    position_t p;
    p.x = 0; p.y = 0; p.z = 0;
    return Render(&p);
}
bool ObjSpr::Render(position_t *pos) {
    glEnable(GL_TEXTURE_2D);
    int currentframe = (animation_percent/100.) * (float)sli.animcount;
    //if (sli.animcount!=1) printf("%f - %d out of %d; %g * %g\n", animation_percent, currentframe, sli.animcount, (animation_percent/100.), (float)sli.animcount);
   // if (itemid == 5022) printf("%s, %d\n",  items[itemid].spritelist, sli.animcount);
   int activeframe;
    for (int i = 0; i < sli.height; i++)
        for (int j = 0; j < sli.width; j++) {
            for (int k = 0; k < sli.blendframes; k++) {

                //if (sli.xdiv>1 && sli.ydiv>1)
                //    activeframe = currentframe *  sli.width * sli.height * sli.xdiv * sli.ydiv * sli.blendframes + ((i*sli.width + j) * sli.ydiv + pos->y % sli.ydiv) * sli.xdiv + pos->x % sli.xdiv;
                //else
                    /*activeframe = currentframe * sli.xdiv * sli.ydiv * sli.blendframes * sli.width * sli.height +
                                  k * sli.width * sli.height +
                                  i * sli.width + j;*/


                switch (type) {
                    case 0: // item
                        activeframe =   (((((( // same number of ( as *
                                        currentframe)
                                        * sli.ydiv + pos->y % sli.ydiv)
                                        * sli.xdiv + pos->x % sli.xdiv)
                                        * sli.blendframes + k)  // k == subblendframes  (stacked sprite)
                                        * sli.height + i)           // i == subheight       (y coordinate)
                                        * sli.width + j)        // j == subwidth        (x coordinate)

                                        ;
                        break;
                    case 1:
                        activeframe =   (((((( // same number of ( as *
                                        currentframe)
                                        * sli.ydiv + 0)//pos->y % sli.ydiv)
                                        * sli.xdiv + direction)//pos->x % sli.xdiv)
                                        * sli.blendframes + k)  // k == subblendframes  (stacked sprite)
                                        * sli.height + i)           // i == subheight       (y coordinate)
                                        * sli.width + j)        // j == subwidth        (x coordinate)

                                        ;
                        break;
                }

                //printf("w %d h %d xdiv %d ydiv %d blendframes %d animcount %d unknown %d\n", sli.width, sli.height, sli.xdiv, sli.ydiv, sli.blendframes, sli.animcount, sli.unknown);

                ASSERT(activeframe < sli.numsprites )
                if (activeframe < sli.numsprites) t[activeframe]->Bind();

                StillEffect(-32*j, 32*i, 32 - 32*j, 32 + 32*i, 2, 2, false, false, true); // divisions were 40 10
            }
        }
    //printf("\n");
    glTranslatef(offsetx, offsety, 0);
    glDisable(GL_TEXTURE_2D);
    return true;
}
bool ObjSpr::Render(unsigned char stackcount) {
    glEnable(GL_TEXTURE_2D);
    t[min(stackcount,sli.numsprites-1)]->Bind();
    StillEffect(0, 0, 32, 32, 2, 2); // divisions were 40 10
    glTranslatef(offsetx, offsety, 0);
    glDisable(GL_TEXTURE_2D);
    return true;
}
void ObjSpr::LoadCreature(unsigned int creatureid) {
    LoadCreature(creatureid, protocol->GetProtocolVersion());

}
void ObjSpr::LoadCreature(unsigned int creatureid, unsigned int protocolversion) {
    char temp[256];
    sprintf(temp, "invalid creatureid %d out of %d in ObjSpr::LoadCreature", creatureid, creatures_n-1);

    ASSERTFRIENDLY(creatureid <= creatures_n-1, temp);

    if (creatures[creatureid].textures) {
        t = (Texture**)creatures[creatureid].textures;
        sli = creatures[creatureid].sli;

        offsetx = 0;
        offsety = 0;

        animation_framecount[0] = creatures[creatureid].animation_framecount[0]; //stand
        animation_framecount[1] = creatures[creatureid].animation_framecount[0];// walk
        return;
    }

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

    char filename [256];
    FILE *f;
    for (int i = 0; i < sli.numsprites; i++) {
        sscanf(p, "%hd", &sli.spriteids[i]); p = strchr(p, ' ')+1;


        switch (protocolversion) {
            case 750:
                sprintf(filename, "tibia75/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("tibia75.spr", sli.spriteids[i]);
                }
                break;
            case 760:
            case 770:
                sprintf(filename, "tibia76/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("tibia76.spr", sli.spriteids[i]);
                }
                break;
            case 790:
            case 792:
                sprintf(filename, "tibia79/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("tibia79.spr", sli.spriteids[i]);
                }

                break;
        }


    }

    creatures[creatureid].textures = t;
    creatures[creatureid].sli = sli;
    offsetx = 0;
    offsety = 0;

    animation_framecount[0] = creatures[creatureid].animation_framecount[0] = 1; //stand
    animation_framecount[1] = creatures[creatureid].animation_framecount[1] = sli.animcount; //walk

}
void ObjSpr::LoadItem(unsigned int itemid) {
    LoadItem(itemid, protocol->GetProtocolVersion());
}
void ObjSpr::LoadItem(unsigned int itemid, unsigned int protocolversion) {
    char temp[256];
    sprintf(temp, "invalid itemid %d out of %d in ObjSpr::LoadItem", itemid, items_n);
    ASSERTFRIENDLY(!itemid || itemid >= 100 && itemid <= items_n, temp);


    if (items[itemid].textures) {
        t = (Texture**)items[itemid].textures;
        sli = items[itemid].sli;

        offsetx = items[itemid].height2d_x * 4;
        offsety = items[itemid].height2d_y * 4;

        animation_framecount[0] = items[itemid].animation_framecount[0]; //stand
        animation_framecount[1] = items[itemid].animation_framecount[1];// walk
        return;
    }

    if (!itemid) {
        sli.width = 1; sli.height = 1; sli.blendframes = 1; sli.xdiv = 1; sli.ydiv = 1; sli.unknown = 1; sli.animcount = 1;
        sli.numsprites = 1;
        sli.spriteids = (unsigned short*)malloc(sli.numsprites * sizeof(unsigned short));
        sli.spriteids[0]=0;
        t = (Texture**)malloc(sli.numsprites * sizeof(Texture*));
        items[itemid].textures = t;
        t[0] = new Texture("tibia76.spr", 0);
        return;
    }
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

    char filename [256];
    FILE *f;
    for (int i = 0; i < sli.numsprites; i++) {
        sscanf(p, "%hd", &sli.spriteids[i]); p = strchr(p, ' ')+1;

        switch (protocolversion) {
            case 750:
                sprintf(filename, "tibia75/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("tibia75.spr", sli.spriteids[i]);
                }
                break;
            case 760:
            case 770:
                sprintf(filename, "tibia76/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("tibia76.spr", sli.spriteids[i]);
                }
                break;
            case 790:
            case 792:
                sprintf(filename, "tibia79/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("tibia79.spr", sli.spriteids[i]);
                }
        }

    }
    items[itemid].textures = t;
    items[itemid].sli = sli;

    animation_framecount[0] = items[itemid].animation_framecount[0] = sli.animcount; //stand
    animation_framecount[1] = items[itemid].animation_framecount[1] = sli.animcount; //walk

}

void ObjSpr::SetDirection(direction_t dir) {
    direction = dir;
}
