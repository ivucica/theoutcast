#include "assert.h"
#include "objspr.h"
#include "items.h"
#include "creatures.h"
#include "effects.h"
#include "distances.h"
#include "types.h"
#include "simple_effects.h"
#include "defines.h"
#include "protocol.h"//remove me and replace the switch(protocol->GetPRotcoolVersion) with something that'll return the spr filename

ONCriticalSection objsprthreadsafe;
//#define ONThreadSafe
//#define ONThreadUnsafe
void ObjSprInit() {
    ONInitThreadSafe(objsprthreadsafe);
}
void ObjSprDeinit() {
    ONDeinitThreadSafe(objsprthreadsafe);
}
ObjSpr::ObjSpr() {
    memset(&sli, 0, sizeof(sli));
    direction = NORTH;
}
ObjSpr::ObjSpr(unsigned int itemid, unsigned char type, unsigned int protocolversion) { // no network version
    ONThreadSafe(objsprthreadsafe);
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
	else if (type == 3)
		LoadDistance(itemid, protocolversion);
    else
        exit(1);
    this->itemid = itemid;
    this->type = type;
    this->direction = NORTH;
    ONThreadUnsafe(objsprthreadsafe);

}
ObjSpr::ObjSpr(unsigned int itemid, unsigned char type) {
    ONThreadSafe(objsprthreadsafe);
    memset(&sli, 0, sizeof(sli));
    offsetx = 0;
    offsety = 0;
    //printf("1\n");
    this->t = NULL;
    //printf("2\n");
    if (type == 0)
        LoadItem(itemid);
    else if (type == 1)
        LoadCreature(itemid);
    else if (type == 2)
        LoadEffect(itemid);
    else if (type == 3)
        LoadDistance(itemid);
    else
        exit(1);
	//printf("3\n");
    this->itemid = itemid;
    this->type = type;
    this->direction = NORTH;

    ONThreadUnsafe(objsprthreadsafe);
}

ObjSpr::ObjSpr(unsigned int creaturetype, unsigned char head, unsigned char body, unsigned char legs, unsigned char feet) {
    ONThreadSafe(objsprthreadsafe);
    memset(&sli, 0, sizeof(sli));
    offsetx = 0;
    offsety = 0;
    this->t = NULL;
    LoadCreature(creaturetype, head, body, legs, feet);
    this->itemid = creaturetype;
    this->type = 1;
    this->direction = NORTH;
    ONThreadUnsafe(objsprthreadsafe);
}
ObjSpr::ObjSpr(unsigned int creaturetype, unsigned int protocolversion, unsigned char head, unsigned char body, unsigned char legs, unsigned char feet) {
    ONThreadSafe(objsprthreadsafe);
    memset(&sli, 0, sizeof(sli));
    offsetx = 0;
    offsety = 0;
    this->t = NULL;
    LoadCreature(creaturetype, protocolversion, head, body, legs, feet);
    this->itemid = creaturetype;
    this->type = 1;
    this->direction = NORTH;
    ONThreadUnsafe(objsprthreadsafe);
}



ObjSpr::~ObjSpr() {
    // FIXME (Khaos#1#): Crash happens because of this function !!
    #if DEBUGLEVEL_BUILDTIME == 0
    return;
    #endif

    ONThreadSafe(objsprthreadsafe);
    //printf("Objspr unloadin %d\n", itemid);
    //system("sleep 1");
    switch (this->type) {
        case 0:
            if (items[itemid]->sli.usecount==1 && sli.spriteids) {
                for (int i = 0 ; i < sli.numsprites; i++)
                    delete(t[i]);
                free(items[itemid]->textures);
                items[itemid]->textures = NULL;
                free(sli.spriteids);
            }
            items[itemid]->sli.usecount--;
            break;
        case 1:
			// colorful creatures are not cached
            if (creatures[itemid]->sli.usecount==1 && sli.spriteids && sli.blendframes == 1) {
                for (int i = 0 ; i < sli.numsprites; i++)
                    delete(t[i]);
                free(creatures[itemid]->textures);
                creatures[itemid]->textures = NULL;
                free(sli.spriteids);
            }
            creatures[itemid]->sli.usecount--;
            break;
        case 2:
            if (effects[itemid]->sli.usecount==1 && sli.spriteids) {
                for (int i = 0 ; i < sli.numsprites; i++)
                    delete(t[i]);
                free(effects[itemid]->textures);
                effects[itemid]->textures = NULL;
                free(sli.spriteids);
            }
            effects[itemid]->sli.usecount--;
            break;
        case 3:
            if (distances[itemid]->sli.usecount==1 && sli.spriteids) {
                for (int i = 0 ; i < sli.numsprites; i++)
                    delete(t[i]);
                free(distances[itemid]->textures);
                distances[itemid]->textures = NULL;
                free(sli.spriteids);
            }
            distances[itemid]->sli.usecount--;
            break;
        default:
            ASSERTFRIENDLY(false, "BOO! BOOO!");

    }
    ONThreadUnsafe(objsprthreadsafe);
}
bool ObjSpr::Render() {
    position_t p(0,0,0);

    return Render(&p);
}
bool ObjSpr::Render(const position_t *pos) {
	if (!itemid) return false;
    glEnable(GL_TEXTURE_2D);
    ONThreadSafe(objsprthreadsafe);
    int currentframe = (int)((animation_percent/100.) * (float)sli.animcount);
    //if (sli.animcount!=1) printf("%f - %d out of %d; %g * %g\n", animation_percent, currentframe, sli.animcount, (animation_percent/100.), (float)sli.animcount);
    // if (itemid == 5022) printf("%s, %d\n",  items[itemid].spritelist, sli.animcount);
    int activeframe;

/*	{
	char  tmp[512];
	sprintf(tmp, "Object %d with no sprites? Very strange!\n", itemid);
	ASSERTFRIENDLY(sli.numsprites || !itemid, tmp);
	}*/
    for (int i = 0; i < sli.height; i++)
        for (int j = 0; j < sli.width; j++) {
            for (int k = 0; k < (type ? min(1, sli.blendframes) : sli.blendframes); k++) { // if anything except item, there won't be blendframes...

                //if (sli.xdiv>1 && sli.ydiv>1)
                //    activeframe = currentframe *  sli.width * sli.height * sli.xdiv * sli.ydiv * sli.blendframes + ((i*sli.width + j) * sli.ydiv + pos->y % sli.ydiv) * sli.xdiv + pos->x % sli.xdiv;
                //else
                    /*activeframe = currentframe * sli.xdiv * sli.ydiv * sli.blendframes * sli.width * sli.height +
                                  k * sli.width * sli.height +
                                  i * sli.width + j;*/


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
                                        //printf("%d %d\n", i, j);
                        break;
                    case 2: // effect
                    case 3: // distance
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
                    if (activeframe >= sli.numsprites) activeframe = 0;
                }
                if (activeframe < sli.numsprites) t[activeframe]->Bind();

                StillEffect(-32*j, 32*i, 32 - 32*j, 32 + 32*i, 2, 2, false, false, true); // divisions were 40 10
            }
        }
    //printf("\n");
    glTranslatef(-offsetx, offsetx, 0);
    glDisable(GL_TEXTURE_2D);
    ONThreadUnsafe(objsprthreadsafe);
    return true;
}
bool ObjSpr::Render(unsigned char stackcount) {
    glEnable(GL_TEXTURE_2D);
    //t[min(stackcount,sli.numsprites-1)]->Bind();
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


	// Since creatures can have different outfits, we must not cache them this naively
    /*if (creatures[creatureid]->textures) {
        t = (Texture**)creatures[creatureid]->textures;
        sli = creatures[creatureid]->sli;

		printf("w %d h %d bf %d xd %d yd %d u %d a %d ns %d uc %d\n", sli.width, sli.height, sli.blendframes, sli.xdiv, sli.ydiv, sli.unknown, sli.animcount, sli.numsprites, sli.usecount );
        printf("w %d h %d bf %d xd %d yd %d u %d a %d ns %d uc %d\n", effects[effectid]->sli.width, effects[effectid]->sli.height, effects[effectid]->sli.blendframes, effects[effectid]->sli.xdiv, effects[effectid]->sli.ydiv, effects[effectid]->sli.unknown, effects[effectid]->sli.animcount, effects[effectid]->sli.numsprites, effects[effectid]->sli.usecount );

        creatures[creatureid]->sli.usecount++;
        offsetx = 0;
        offsety = 0;

        animation_framelist_stand = creatures[creatureid]->animation_framelist_stand; //stand
        animation_framelist_move = creatures[creatureid]->animation_framelist_move;// walk
        return;
    }*/
	printf("Forming creature %d\n", creatureid);
    {
        char tmp[256]; sprintf(tmp, "Creature %d not loaded\n", creatureid);
        ASSERTFRIENDLY(creatures[creatureid]->loaded, tmp);
    }

    if (!strlen(creatures[creatureid]->spritelist)) return;
    char *p = creatures[creatureid]->spritelist;
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

    char filename [512];
    FILE *f;
    //printf("%d\n", creatureid);
    for (int i = 0; i < sli.numsprites; i++) {
        sscanf(p, "%hd", &sli.spriteids[i]); p = strchr(p, ' ')+1;
    }

    for (int i = 0 ; i < sli.numsprites; i++) {
        if (sli.blendframes > 1) {
            if ((i / (sli.height * sli.width)) % 2 ) {
                t[i] = NULL;
                continue;
            }
        }
        switch (protocolversion) {
            case 750:
                if (sli.blendframes > 1)
                    t[i] = new Texture("Tibia75.spr", sli.spriteids[i], sli.spriteids[i+sli.height*sli.width], head, body, legs, feet);
                else
                    t[i] = new Texture("Tibia75.spr", sli.spriteids[i]);
                break;
            case 760:
            case 770:
                if (sli.blendframes > 1)
                    t[i] = new Texture("Tibia76.spr", sli.spriteids[i], sli.spriteids[i+sli.height*sli.width], head, body, legs, feet);
                else
                    t[i] = new Texture("Tibia76.spr", sli.spriteids[i]);
                break;
            case 790:
                if (sli.blendframes > 1)
                    t[i] = new Texture("Tibia79.spr", sli.spriteids[i], sli.spriteids[i+sli.height*sli.width], head, body, legs, feet);
                else
                    t[i] = new Texture("Tibia79.spr", sli.spriteids[i]);
                break;
            case 792:
                if (sli.blendframes > 1)
                    t[i] = new Texture("Tibia792.spr", sli.spriteids[i], sli.spriteids[i+sli.height*sli.width], head, body, legs, feet);
                else
                    t[i] = new Texture("Tibia792.spr", sli.spriteids[i]);
                break;
            case 800:
                if (sli.blendframes > 1)
                    t[i] = new Texture("Tibia80.spr", sli.spriteids[i], sli.spriteids[i+sli.height*sli.width], head, body, legs, feet);
                else
                    t[i] = new Texture("Tibia80.spr", sli.spriteids[i]);
                break;

        }

    }

    if (sli.blendframes > 1) creatures[creatureid]->textures = NULL; else creatures[creatureid]->textures = t;
    creatures[creatureid]->sli = sli;
    creatures[creatureid]->sli.usecount++;
    offsetx = 0;
    offsety = 0;


    animation_framelist_stand.insert(animation_framelist_stand.end(), 0);
    creatures[creatureid]->animation_framelist_stand.insert(creatures[creatureid]->animation_framelist_stand.end(), 0);

    animation_framelist_move.insert(animation_framelist_move.end(), 1);
    animation_framelist_move.insert(animation_framelist_move.end(), 2);

    creatures[creatureid]->animation_framelist_move.insert(creatures[creatureid]->animation_framelist_move.end(), 1);
    creatures[creatureid]->animation_framelist_move.insert(creatures[creatureid]->animation_framelist_move.end(), 2);

	this->itemid = creatureid;

}
void ObjSpr::LoadItem(unsigned int itemid) {
	ASSERTFRIENDLY(protocol, "ObjSpr::LoadItem(): No protocol loaded, and it must be loaded for this function to work. It's possible that author actually wanted to use 'offline' version of this function here.")
	LoadItem(itemid, protocol->GetProtocolVersion());
}
void ObjSpr::LoadItem(unsigned int itemid, unsigned int protocolversion) {
    char temp[512];
    sprintf(temp, "invalid itemid %d out of %d in ObjSpr::LoadItem", itemid, items_n);
    ASSERTFRIENDLY(!itemid || itemid >= 100 && itemid <= items_n, temp);

    if (items[itemid]->textures) {
        t = (Texture**)items[itemid]->textures;
        sli = items[itemid]->sli;

        items[itemid]->sli.usecount++;
        offsetx = items[itemid]->height2d_x ;
        offsety = items[itemid]->height2d_y ;

        animation_framelist_stand = items[itemid]->animation_framelist_stand; //stand
        animation_framelist_move = items[itemid]->animation_framelist_move;// walk
        return;
    }

    if (!itemid) {
        sli.width = 1; sli.height = 1; sli.blendframes = 1; sli.xdiv = 1; sli.ydiv = 1; sli.unknown = 1; sli.animcount = 1;
        sli.numsprites = 1;
        sli.spriteids = (unsigned short*)malloc(sli.numsprites * sizeof(unsigned short));
        sli.spriteids[0]=0;
        t = (Texture**)malloc(sli.numsprites * sizeof(Texture*));
        items[itemid]->textures = t;
        t[0] = new Texture("anything.spr", 0);
        return;
    }
    ASSERTFRIENDLY(items[itemid]->loaded, "Item with the ID that server transmitted is not loaded");


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

    // now shall we read sprite ids
    // beware! :D
    if (sli.spriteids)
        free(sli.spriteids);

    sli.spriteids = (unsigned short*)malloc(sli.numsprites * sizeof(unsigned short));
    t = (Texture**)malloc(sli.numsprites * sizeof(Texture*));

    char filename [512];
    FILE *f;
    for (int i = 0; i < sli.numsprites; i++) {
        sscanf(p, "%hd", &sli.spriteids[i]); p = strchr(p, ' ')+1;

        switch (protocolversion) {
            case 750:
                sprintf(filename, "Tibia75/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("Tibia75.spr", sli.spriteids[i]);
                }
                break;
            case 760:
            case 770:
                sprintf(filename, "Tibia76/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("Tibia76.spr", sli.spriteids[i]);
                }
                break;
            case 790:

                sprintf(filename, "Tibia79/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("Tibia79.spr", sli.spriteids[i]);
                }
                break;
            case 792:
                sprintf(filename, "Tibia792/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("Tibia792.spr", sli.spriteids[i]);
                }
                break;

            case 800:
                sprintf(filename, "Tibia80/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("Tibia80.spr", sli.spriteids[i]);
                }
                break;
        }

    }
    items[itemid]->textures = t;
    items[itemid]->sli = sli;
    items[itemid]->sli.usecount = 1;

    for (int i = 0 ; i < sli.animcount; i++) {
        animation_framelist_stand.insert(animation_framelist_stand.end(), i);
        items[itemid]->animation_framelist_stand.insert(items[itemid]->animation_framelist_stand.end(), i);


        animation_framelist_move.insert(animation_framelist_move.end(), i);
        items[itemid]->animation_framelist_move.insert(items[itemid]->animation_framelist_move.end(), i);

    }

    offsetx = items[itemid]->height2d_x ;
    offsety = items[itemid]->height2d_y ;


}

void ObjSpr::LoadEffect(unsigned int effectid) {
	printf("loading effect...\n");
    LoadEffect(effectid, protocol->GetProtocolVersion());
}
void ObjSpr::LoadEffect(unsigned int effectid, unsigned int protocolversion) {
    char temp[512];
    sprintf(temp, "invalid effectid %d out of %d in ObjSpr::LoadEffect", effectid, effects_n);
    ASSERTFRIENDLY(effectid <= effects_n, temp);
    if (!(effectid <= effects_n)) {// someone pressed ignore
        // try skipping the loading of this effect
        return;
    }
    printf("Loading effectid %d out of %d in ObjSpr::LoadEffect\n", effectid, effects_n);
#if 1
	printf("Checking if it's already loaded\n");
    if (effects[effectid]->textures) {
    	printf("It's already there!\n");
        t = (Texture**)effects[effectid]->textures;
        //printf("1\n");
        sli = effects[effectid]->sli;
        //printf("2\n");

        printf("w %d h %d bf %d xd %d yd %d u %d a %d ns %d uc %d\n", sli.width, sli.height, sli.blendframes, sli.xdiv, sli.ydiv, sli.unknown, sli.animcount, sli.numsprites, sli.usecount );
        printf("w %d h %d bf %d xd %d yd %d u %d a %d ns %d uc %d\n", effects[effectid]->sli.width, effects[effectid]->sli.height, effects[effectid]->sli.blendframes, effects[effectid]->sli.xdiv, effects[effectid]->sli.ydiv, effects[effectid]->sli.unknown, effects[effectid]->sli.animcount, effects[effectid]->sli.numsprites, effects[effectid]->sli.usecount );

        effects[effectid]->sli.usecount++;
        //printf("3\n");
        offsetx = 0;//effects[effectid]->height2d_x ;
        //printf("4\n");
        offsety = 0;//effects[effectid]->height2d_y ;
        //printf("5\n");

        animation_framelist_stand = effects[effectid]->animation_framelist_stand; //stand
        printf("#\n");
        animation_framelist_move = effects[effectid]->animation_framelist_move;// walk

        printf("Finished\n");
        return;
    }
	printf("Nope, creating new one\n");
#endif

    /*if (!effectid) {
        sli.width = 1; sli.height = 1; sli.blendframes = 1; sli.xdiv = 1; sli.ydiv = 1; sli.unknown = 1; sli.animcount = 1;
        sli.numsprites = 1;
        sli.spriteids = (unsigned short*)malloc(sli.numsprites * sizeof(unsigned short));
        sli.spriteids[0]=0;
        t = (Texture**)malloc(sli.numsprites * sizeof(Texture*));
        effects[effectid]->textures = t;
        t[0] = new Texture("Tibia76.spr", 0);
        return;
    }*/
    ASSERTFRIENDLY(effectid, "Invalid effectid -- it shouldn't be zero");
    ASSERTFRIENDLY(effects[effectid]->loaded, "Effect with the ID that server transmitted is not loaded");


    if (!strlen(effects[effectid]->spritelist)) return;
    char *p = effects[effectid]->spritelist;
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

    char filename [512];
    FILE *f;
    for (int i = 0; i < sli.numsprites; i++) {
        sscanf(p, "%hd", &sli.spriteids[i]); p = strchr(p, ' ')+1;

        switch (protocolversion) {
            case 750:
                sprintf(filename, "Tibia75/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("Tibia75.spr", sli.spriteids[i]);
                }
                break;
            case 760:
            case 770:
                sprintf(filename, "Tibia76/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("Tibia76.spr", sli.spriteids[i]);
                }
                break;
            case 790:

                sprintf(filename, "Tibia79/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("Tibia79.spr", sli.spriteids[i]);
                }
                break;
            case 792:
                sprintf(filename, "Tibia792/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("Tibia792.spr", sli.spriteids[i]);
                }
                break;

            case 800:
                sprintf(filename, "Tibia80/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("Tibia80.spr", sli.spriteids[i]);
                }
                break;

        }

    }
    effects[effectid]->textures = t;
    effects[effectid]->sli = sli;
    effects[effectid]->sli.usecount = 1;

    for (int i = 0 ; i < sli.animcount; i++) {
        animation_framelist_stand.insert(animation_framelist_stand.end(), i);
        effects[effectid]->animation_framelist_stand.insert(effects[effectid]->animation_framelist_stand.end(), i);


        animation_framelist_move.insert(animation_framelist_move.end(), i);
        effects[effectid]->animation_framelist_move.insert(effects[effectid]->animation_framelist_move.end(), i);

    }

        offsetx = 0;//effects[effectid]->height2d_x ;
        offsety = 0;//effects[effectid]->height2d_y ;
	this->itemid = effectid;

}



void ObjSpr::LoadDistance(unsigned int distanceid) {
	printf("loading distance...\n");
    LoadDistance(distanceid, protocol->GetProtocolVersion());
}
void ObjSpr::LoadDistance(unsigned int distanceid, unsigned int protocolversion) {
    char temp[512];
    sprintf(temp, "invalid distanceid %d out of %d in ObjSpr::LoadDistance", distanceid, distances_n);
    ASSERTFRIENDLY(distanceid <= distances_n, temp);
    if (!(distanceid <= distances_n)) {// someone pressed ignore
        // try skipping the loading of this distance
        return;
    }
    printf("Loading distanceid %d out of %d in ObjSpr::LoadDistance\n", distanceid, distances_n);
#if 1
	printf("Checking if it's already loaded\n");
    if (distances[distanceid]->textures) {
    	printf("It's already there!\n");
        t = (Texture**)distances[distanceid]->textures;
        //printf("1\n");
        sli = distances[distanceid]->sli;
        //printf("2\n");

        printf("w %d h %d bf %d xd %d yd %d u %d a %d ns %d uc %d\n", sli.width, sli.height, sli.blendframes, sli.xdiv, sli.ydiv, sli.unknown, sli.animcount, sli.numsprites, sli.usecount );
        printf("w %d h %d bf %d xd %d yd %d u %d a %d ns %d uc %d\n", distances[distanceid]->sli.width, distances[distanceid]->sli.height, distances[distanceid]->sli.blendframes, distances[distanceid]->sli.xdiv, distances[distanceid]->sli.ydiv, distances[distanceid]->sli.unknown, distances[distanceid]->sli.animcount, distances[distanceid]->sli.numsprites, distances[distanceid]->sli.usecount );

        distances[distanceid]->sli.usecount++;
        //printf("3\n");
        offsetx = 0;//distances[distanceid]->height2d_x ;
        //printf("4\n");
        offsety = 0;//distances[distanceid]->height2d_y ;
        //printf("5\n");

        animation_framelist_stand = distances[distanceid]->animation_framelist_stand; //stand
        printf("#\n");
        animation_framelist_move = distances[distanceid]->animation_framelist_move;// walk

        printf("Finished\n");
        return;
    }
	printf("Nope, creating new one\n");
#endif

    /*if (!distanceid) {
        sli.width = 1; sli.height = 1; sli.blendframes = 1; sli.xdiv = 1; sli.ydiv = 1; sli.unknown = 1; sli.animcount = 1;
        sli.numsprites = 1;
        sli.spriteids = (unsigned short*)malloc(sli.numsprites * sizeof(unsigned short));
        sli.spriteids[0]=0;
        t = (Texture**)malloc(sli.numsprites * sizeof(Texture*));
        distances[distanceid]->textures = t;
        t[0] = new Texture("Tibia76.spr", 0);
        return;
    }*/
    ASSERTFRIENDLY(distanceid, "Invalid distanceid -- it shouldn't be zero");
    ASSERTFRIENDLY(distances[distanceid]->loaded, "Distance with the ID that server transmitted is not loaded");


    if (!strlen(distances[distanceid]->spritelist)) return;
    char *p = distances[distanceid]->spritelist;
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

    char filename [512];
    FILE *f;
    for (int i = 0; i < sli.numsprites; i++) {
        sscanf(p, "%hd", &sli.spriteids[i]); p = strchr(p, ' ')+1;

        switch (protocolversion) {
            case 750:
                sprintf(filename, "Tibia75/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("Tibia75.spr", sli.spriteids[i]);
                }
                break;
            case 760:
            case 770:
                sprintf(filename, "Tibia76/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("Tibia76.spr", sli.spriteids[i]);
                }
                break;
            case 790:

                sprintf(filename, "Tibia79/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("Tibia79.spr", sli.spriteids[i]);
                }
                break;
            case 792:
                sprintf(filename, "Tibia792/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("Tibia792.spr", sli.spriteids[i]);
                }
                break;

            case 800:
                sprintf(filename, "Tibia80/%d.bmp", sli.spriteids[i]);
                f = fopen(filename, "r");
                if (f) {
                    fclose(f);
                    t[i] = new Texture(filename);

                    break;
                }

                if (!f) {
                    t[i] = new Texture("Tibia80.spr", sli.spriteids[i]);
                }
                break;

        }

    }
    distances[distanceid]->textures = t;
    distances[distanceid]->sli = sli;
    distances[distanceid]->sli.usecount = 1;

    for (int i = 0 ; i < sli.animcount; i++) {
        animation_framelist_stand.insert(animation_framelist_stand.end(), i);
        distances[distanceid]->animation_framelist_stand.insert(distances[distanceid]->animation_framelist_stand.end(), i);


        animation_framelist_move.insert(animation_framelist_move.end(), i);
        distances[distanceid]->animation_framelist_move.insert(distances[distanceid]->animation_framelist_move.end(), i);

    }

        offsetx = 0;//distances[distanceid]->height2d_x ;
        offsety = 0;//distances[distanceid]->height2d_y ;
	this->itemid = distanceid;

}


void ObjSpr::SetDirection(direction_t dir) {
    ONThreadSafe(objsprthreadsafe);
    direction = dir;
    ONThreadUnsafe(objsprthreadsafe);
}
