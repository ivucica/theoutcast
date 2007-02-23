#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

/* defines */
#define BOOL char
#define TRUE 1
#define FALSE 0
/* structs */
typedef struct {
    unsigned char width;
    unsigned char height;
    unsigned char blendframes;
    unsigned char xdiv;
    unsigned char ydiv;
    unsigned char animcount;
    unsigned char unknown;
    unsigned short numsprites; /* to  remove need to calculate this */
    unsigned short *spriteids;
} spritelist_t;

typedef struct {
    char graphics[50];
    char graphics2d[50];
    BOOL ground;
    unsigned short speedindex;
    unsigned char topindex;
    BOOL container;
    BOOL stackable;
    BOOL usable;
    BOOL readable;
    BOOL writable;
    BOOL fluidcontainer;
    BOOL splash;
    BOOL movable;
    BOOL pickupable;
    BOOL blocking;
    BOOL floorchange;
    unsigned short readability_len;
    unsigned char lightcolor;
    unsigned char lightradius;
    double height;
    unsigned char height2d_x, height2d_y;
    unsigned short minimapcolor;
    char spritelist[4096];
    unsigned short otid;

    spritelist_t *sl;
} item_t;

/* files */
FILE *fi;
sqlite3 *fo;

/* ui stuff */
char lastpercentage;

/* options */
int datversion;

/* header data */
unsigned short dat_items, dat_creatures, dat_effects, dat_distance;



int dbexecprintf(sqlite3* db, const char *sql, sqlite3_callback cb, void *arg, char **errmsg, ...) {
	va_list vl;
	va_start(vl, sql);

	char *z = sqlite3_vmprintf(sql, vl);

    /* printf("QUERY: %s\n", z); */

	int rc = sqlite3_exec(db, z, cb, arg, errmsg);
	if (rc != SQLITE_OK) printf("SQLite: Error: '%s', RC: %d, query '%s'\n", sqlite3_errmsg(fo), rc, z);

	sqlite3_free(z);

	va_end(vl);
	return rc;
}
int dbexec(sqlite3* db, const char *sql, sqlite3_callback cb, void *arg, char **errmsg) {
    /*printf("QUERY: %s\n", sql);*/
    sqlite3_exec(db, sql, cb, arg, errmsg);
}
char tableexists(const char *tablename) {
	return (dbexecprintf(fo, "select * from %s;", NULL, 0, NULL, tablename) == SQLITE_OK);
}
char check_tables() {
    char tablename[10];
    sprintf(tablename, "items%d", datversion);
	if (!tableexists(tablename)) {
		printf("Creating table '%s'.\n", tablename);
		if (dbexecprintf(fo, "create table %s ("

			"itemid integer primary key," /* item id, as the server sends it to us */
			"graphics varchar[50], " /* 3d graphics file */
			"graphics2d varchar[50], " /* 3d graphics file */
			"ground boolean, " /* is this a ground item */
			"speedindex integer," /* what is the speed index of the item */
			"topindex integer, " /* what is the topindex of the item */
			"container boolean, " /* is this a container */
			"stackable boolean, " /* is this item countable */
			"usable boolean, " /* can this item be used; useful only to FORCE that item is NOT usable */
			"readable boolean, " /* can stuff be read from this item in a separate window */
			"writable boolean, " /* can stuff be written on this item */
			"fluidcontainer boolean, " /* is this item a fluid container */
			"splash boolean, " /* is this a 'splash' item, meaning, colorable */
			"movable boolean, " /* can this item be moved */
			"pickupable boolean, " /* can this item be picked up */
			"blocking boolean, " /* is this item blocking movement */
			"floorchange boolean, " /* is floor changed by walking onto this item */
			"readability_len integer, " /* how much data can be stored into a readable/writable item */
			"lightcolor integer, " /* if so, which color */
			"lightradius integer, " /* if so, which radius */
			"height double, " /* how much does this item alter the height of items above it */
			"height2d_x integer, height2d_y integer, " /* how much does this item alter the height of items above it, in x and y*/
			"minimapcolor integer," /* what is the color of this item on the minimap */
			"spritelist varchar[4096], " /* spritelist */
			"otid integer" /* under what id does OTserv store this item */
			"); ",NULL, 0, NULL, tablename) != SQLITE_OK) {
				printf("Table '%s' creation failed\n", tablename);
				return 0;
		}
	}
    sprintf(tablename, "creatures%d", datversion);
	if (!tableexists(tablename)) {
		printf("Creating table '%s'.\n", tablename);
		if (dbexecprintf(fo, "create table %s ("
            "creatureid integer primary key," /* creature id, as the server sends it to us */
            "graphics varchar[50], " /* 3d graphics file */
			"graphics2d varchar[50], " /* 3d graphics file */
            "spritelist varchar[4096] " /* spritelist */
            "); ", NULL, 0, NULL, tablename) != SQLITE_OK) {
                printf("Table '%s' creation failed\n", tablename);
				return 0;
            }
	}

    return 1;
}
unsigned short readu16() {
    unsigned short tmp;
    fread(&tmp, 2, 1, fi);
    return tmp;
}
char dat_load_header() {
    unsigned long signature;
    fread(&signature, 4, 1, fi);
    printf("DAT signature: 0x%08x\n", signature);
    fread(&dat_items, 2, 1, fi);
    printf("Items: %d\n", dat_items);
    fread(&dat_creatures, 2, 1, fi);
    printf("Creatures: %d\n", dat_creatures);
    fread(&dat_effects, 2, 1, fi);
    printf("Effects: %d\n", dat_effects);
    fread(&dat_distance, 2, 1, fi);
    printf("Distance shot effects: %d\n", dat_distance);

}
void clear_item(item_t* item) {

    item->graphics[0] = 0;
    item->graphics2d[0] = 0;
    item->ground = FALSE;
    item->speedindex = 0;
    item->topindex = 0;
    item->container = FALSE;
    item->stackable = FALSE;
    item->usable = FALSE;
    item->readable = FALSE;
    item->writable = FALSE;
    item->fluidcontainer = FALSE;
    item->splash = FALSE;
    item->movable = TRUE;
    item->pickupable = FALSE;
    item->blocking = FALSE;
    item->floorchange = FALSE;
    item->readability_len = 0;
    item->lightcolor = 0;
    item->lightradius = 0;
    item->height = 0.;
    item->height2d_x = 0; item->height2d_y = 0;
    item->minimapcolor = 0;
    item->spritelist[0] = 0;
    item->otid = 0;

}
char dat_readitem(item_t *item) {
    unsigned char option;
    unsigned char tmpchar;

    unsigned char width, height, blendframes, xdiv, ydiv, animcount, unknown;
    unsigned int i;

    unsigned short numsprites;


    clear_item(item);

    for (option = fgetc(fi); option != 0xFF; option = fgetc(fi)) {
        /*printf("Byte %02x\n", option);*/
        switch (datversion) {
            case 750:

                switch (option) {

                    case 0x00: /* ground */
                        item->ground = TRUE;
                        item->speedindex = readu16(); /* speed index */
                        break;
                    case 0x01: /* alwaysontop 1 */
                        item->topindex = 1;
                        break;
                    case 0x02: /* alwaysontop 2 */
                        item->topindex = 2;
                        break;
                    case 0x03: /* container */
                        item->container = TRUE;
                        break;
                    case 0x04: /* stackable */
                        item->stackable = TRUE;
                        break;
                    case 0x05: /* usable? */
                        item->usable = TRUE;
                        break;
                    case 0x06: /* ladder */
                        break;
                    case 0x07: /* writable */
                        item->writable = TRUE;
                        item->readability_len = readu16(); /* max writable text size? maybe...! */
                        break;
                    case 0x08: /* readable */
                        item->readable = TRUE;
                        item->readability_len = readu16(); /* max readable text size? maybe...! */
                        break;
                    case 0x09: /* fluid container */
                        item->fluidcontainer = TRUE;
                        break;
                    case 0x0A: /* 'splash' */
                        item->splash = TRUE;
                        break;
                    case 0x0B: /* is blocking */
                        item->blocking = TRUE;
                        break;
                    case 0x0C: /* is not movable */
                        item->movable = FALSE;
                        break;
                    case 0x0D: /* blocks missiles */
                        break;
                    case 0x0E: /* block monster movement */
                        break;
                    case 0x0F: /* pickupable */
                        item->pickupable = TRUE;
                        break;
                    case 0x10: /* light emitter */
                        item->lightradius = readu16(); /* radius */
                        item->lightcolor = readu16(); /* color */
                        break;
                    case 0x11: /* floor change? or can see what's through? (ladder holes, stair holes, etc) */
                        item->floorchange = TRUE;
                        break;
                    case 0x12: /* no floor change */
                        /*item->floorchange = FALSE;*/
                        break;
                    case 0x13: /* items that have height */
                        item->height2d_x = fgetc(fi);
                        item->height2d_y = fgetc(fi);
                        break;
                    case 0x14: /* "sprite drawing related" ?! */
                        break;
                    case 0x16: /* minimap color */
                        item->minimapcolor = readu16();
                        break;
                    case 0x17:  /* seems like decorables with 4 states of turning (exception first 4 are unique statues) */
                        break;

                    case 0x18: /* "draw with height offset for all parts (2x2) of the sprite" ?! */
                        break;
                    case 0x19:  /* wall items -- hangable? */
                        break;
                    case 0x1A: /* unknown */
                        break;
                    case 0x1B:  /* walls 2 types of them same material (total 4 pairs) */

                        break;

                    case 0x1C: /* monsters that are animated even when idle */
                        break;
                    case 0x1D:/* line spot ?!? */

                        tmpchar = fgetc(fi); /* 86 -> openable holes, 77-> can be used to go down, 76 can be used to go up, 82 -> stairs up, 79 switch, */

                        if(tmpchar == 0x58)
                            item->readable = TRUE;
                        fgetc(fi); /* always 4 */
                        break;
                    case 0x1E: /* ground items */
                        break;
                    case 0xFF: /* end of section */
                        printf("Kraj!\n");
                        break;
                    default:
                        printf("unknown byte: %d\n", (unsigned short)option);
                        return 0;
                        break;

                }

                break;






            case 760:
            case 770:
                switch (option) {
                    case 0x00: /* ground */
                        item->ground = TRUE;
                        item->speedindex = readu16(); /* speed index */
                        break;
                    case 0x01: /* alwaysontop 1 */
                        item->topindex = 1;
                        break;
                    case 0x02: /* alwaysontop 2 */
                        item->topindex = 2;
                        break;
                    case 0x03: /* alwaysontop 3 */
                        item->topindex = 3;
                        break;
                    case 0x04: /* container */
                        item->container = TRUE;
                        break;
                    case 0x05: /* stackable */
                        item->stackable = TRUE;
                        break;
                    case 0x06: /* ladder OR CORPSE?!?!?! */
                        break;
                    case 0x07: /* usable? */
                        item->usable = TRUE;
                        break;
                    case 0x08: /* writable */
                        item->writable = TRUE;
                        item->readability_len = readu16(); /* max writable text size? maybe...! */
                        break;
                    case 0x09: /* readable */
                        item->readable = TRUE;
                        item->readability_len = readu16(); /* max readable text size? maybe...! */
                        break;
                    case 0x0A: /* fluid container */
                        item->fluidcontainer = TRUE;
                        break;
                    case 0x0B: /* 'splash' */
                        item->splash = TRUE;
                        break;
                    case 0x0C: /* is blocking */
                        item->blocking = TRUE;
                        break;
                    case 0x0D: /* is not movable */
                        item->movable = FALSE;
                        break;
                    case 0x0E: /* blocks missiles */
                        break;
                    case 0x0F: /* block monster movement */
                        break;
                    case 0x10: /* pickupable */
                        item->pickupable = TRUE;
                        break;
                    case 0x11: /* hangable */
                        break;
                    case 0x12: /* horizontal ?? */
                        break;
                    case 0x13: /* vertical ?? */
                        break;
                    case 0x14: /* rotateable */
                        break;
                    case 0x15: /* light emitter */
                        item->lightradius = readu16(); /* radius */
                        item->lightcolor = readu16(); /* color */
                        break;
                    case 0x17: /* floor change */
                        item->floorchange = TRUE;
                        break;
                    case 0x18: /* unknown, 4 bytes argument */
                        readu16();
                        readu16();
                        break;
                    case 0x19: /* height offset */
                        /* byte1 = x, byte2 = y? perhaps! they're always 8! */
                        item->height2d_x = fgetc(fi);
                        item->height2d_y = fgetc(fi);
                        break;
                    case 0x1A: /* "draw with height offset for all parts (2x2) of the sprite" ?! */
                        break;
                    case 0x1B: /* monsters that are animated even when idle */
                        break;
                    case 0x1C: /* minimap color */
                        item->minimapcolor = readu16();
                        break;
                    case 0x1D:/* line spot ?!? */

                        tmpchar = fgetc(fi); /* 86 -> openable holes, 77-> can be used to go down, 76 can be used to go up, 82 -> stairs up, 79 switch, */

                        if(tmpchar == 0x58)
                            item->readable = TRUE;
                        fgetc(fi); /* always 4 */
                        break;
                    case 0x1E: /* ground items */
                        break;
                    case 0xFF: /* end of section */
                        printf("Kraj!\n");
                        break;
                    default:
                        printf("unknown byte: %d\n", (unsigned short)option);
                        return 0;
                        break;

                }

                break;





            case 790:
            case 792:
                switch (option) {
                    case 0x00: /* ground */
                        item->ground = TRUE;
                        item->speedindex = readu16(); /* speed index */
                        break;
                    case 0x01: /* alwaysontop 1 */
                        item->topindex = 1;
                        break;
                    case 0x02: /* alwaysontop 2 */
                        item->topindex = 2;
                        break;
                    case 0x03: /* alwaysontop 3 */
                        item->topindex = 3;
                        break;
                    case 0x04: /* container */
                        item->container = TRUE;
                        break;
                    case 0x05: /* stackable */
                        item->stackable = TRUE;
                        break;
                    case 0x06: /* ladder OR CORPSE?!?!*/
                        break;
                    case 0x07: /* usable? */
                        item->usable = TRUE;
                        break;
                    case 0x08: /* runes */
                        break;
                    case 0x09: /* writable */
                        item->writable = TRUE;
                        item->readability_len = readu16(); /* max writable text size? maybe...! */
                        break;
                    case 0x0A: /* readable */
                        item->readable = TRUE;
                        item->readability_len = readu16(); /* max readable text size? maybe...! */
                        break;
                    case 0x0B: /* fluid container */
                        item->fluidcontainer = TRUE;
                        break;
                    case 0x0C: /* 'splash' */
                        item->splash = TRUE;
                        break;
                    case 0x0D: /* is blocking */
                        item->blocking = TRUE;
                        break;
                    case 0x0E: /* is not movable */
                        item->movable = FALSE;
                        break;
                    case 0x0F: /* blocks missiles */
                        break;
                    case 0x10: /* block monster movement */
                        break;
                    case 0x11: /* pickupable */
                        item->pickupable = TRUE;
                        break;
                    case 0x12: /* hangable */
                        break;
                    case 0x13: /* horizontal ?? */
                        break;
                    case 0x14: /* vertical ?? */
                        break;
                    case 0x15: /* rotateable */
                        break;
                    case 0x16: /* light emitter */
                        item->lightradius = readu16(); /* radius */
                        item->lightcolor = readu16(); /* color */
                        break;
                    case 0x17: /* floor change */
                        item->floorchange = TRUE;
                        break;
                    case 0x18: /* nothing?! */
                        break;
                    case 0x19: /* unknown, 4 bytes argument */
                        readu16();
                        readu16();
                        break;
                    case 0x1A: /* height offset */
                        /* byte1 = x, byte2 = y? perhaps! they're always 8! */
                        item->height2d_x = fgetc(fi);
                        item->height2d_y = fgetc(fi);
                        break;
                    case 0x1B: /* "draw with height offset for all parts (2x2) of the sprite" ?! */
                        break;
                    case 0x1C: /* monsters that are animated even when idle */
                        break;
                    case 0x1D: /* minimap color */
                        item->minimapcolor = readu16();
                        break;
                    case 0x1E:/* line spot ?!? */

                        tmpchar = fgetc(fi); /* 86 -> openable holes, 77-> can be used to go down, 76 can be used to go up, 82 -> stairs up, 79 switch, */

                        if(tmpchar == 0x58)
                            item->readable = TRUE;
                        fgetc(fi); /* always 4 */
                        break;
                    case 0x1F: /* ground items */
                        break;
                    case 0xFF: /* end of section */
                        printf("Kraj!\n");
                        break;
                    default:
                        printf("unknown byte: %d\n", (unsigned short)option);
                        return 0;
                        break;

                }

                break;




            default:
                printf("UNKNOWN DAT VERSION! Can't read any chunks.\n");
                return 0;
        }
    }

    switch (datversion) {
        case 750:
        case 760:
        case 770:
        case 790:
        case 792: {
            spritelist_t *sl = (spritelist_t*)malloc(sizeof(spritelist_t));
            if (!sl) {
                printf("Cannot alloc enough memory for spritelist\n");
                return 0;
            }
            sl->width = fgetc(fi);
            sl->height = fgetc(fi);
            if (sl->width > 1 || sl->height > 1) fgetc(fi);
            sl->blendframes = fgetc(fi);
            sl->xdiv = fgetc(fi);
            sl->ydiv = fgetc(fi);
            sl->animcount = fgetc(fi);
            if (datversion<=750)
                sl->unknown = 1;
            else
                sl->unknown = fgetc(fi); /* this does not exist for versions before 7.55 - in such case, it's 1 */
            sl->numsprites = sl->width * sl->height * sl->blendframes * sl->xdiv * sl->ydiv * sl->animcount * sl->unknown;

/*
            printf("Spritegrid width: %d\n", sl->width);
            printf("Spritegrid height: %d\n", sl->height);
            printf("Spritegrid blendframes: %d\n", sl->blendframes);
            printf("Spritegrid xdiv: %d\n", sl->xdiv);
            printf("Spritegrid ydiv: %d\n", sl->ydiv);
            printf("Spritegrid animcount: %d\n", sl->animcount);
            printf("Spritegrid unknown: %d\n", sl->unknown);
            printf("Spritegrid numsprites: %d\n", sl->numsprites);
*/

            sl->spriteids = (unsigned short*)malloc(sizeof(unsigned short) * sl->numsprites);
            if (!sl->spriteids) {
                printf("Cannot alloc enough memory for spritelist\n");
                return 0;
            }
            for(i = 0; i < sl->numsprites; ++i) {
                sl->spriteids[i] = readu16(); /* sprite id */
                /*printf("Sprite id %d: %d\n", (unsigned int)i, (unsigned int)sl->spriteids[i]);*/
            }
            item->sl = sl;
            break;
        }

        default:
            printf("YOU SHOULD NOT REACH THIS POINT.\n");
            return 0;
    }

    return 1;
}
static int extryexistsfunc(void *returndestvoid, int argc, char **argv, char **azColName) {
    BOOL *returndest = (BOOL*)returndestvoid;
    *returndest = TRUE;
    return 0;
}
char entryexists_itemid(unsigned int itemid) {
    BOOL returner = FALSE;

    if (dbexecprintf(fo, "select * from items%d where itemid='%d';", &extryexistsfunc, &returner, NULL, datversion, itemid, datversion) == SQLITE_OK) return returner; else return FALSE;
}
char entryexists_creatureid(unsigned int itemid) {
    BOOL returner = FALSE;

    if (dbexecprintf(fo, "select * from creatures%d where creatureid='%d';", &extryexistsfunc, &returner, NULL, datversion, itemid, datversion) == SQLITE_OK) return returner; else return FALSE;
}
BOOL gettrue () {
    return TRUE ;
}
BOOL insertitem (unsigned short itemid, item_t *i) {

    char spritelist[4096];
    char *spritelistptr;
    unsigned short j;
    spritelist_t *sl = i->sl;

    spritelistptr = spritelist + sprintf(spritelist, "%d %d %d %d %d %d %d %d ", (unsigned int)sl->width, (unsigned int)sl->height, (unsigned int)sl->blendframes, (unsigned int)sl->xdiv, (unsigned int)sl->ydiv, (unsigned int)sl->animcount, (unsigned int)sl->unknown, (unsigned int)sl->numsprites);

    for (j = 0; j < sl->numsprites; ++j) {
        spritelistptr += sprintf(spritelistptr, "%d ", (unsigned int)sl->spriteids[j]);
    }

    if (!entryexists_itemid(itemid)) {

        if (dbexecprintf(fo, "insert into items%d ("
                        "itemid, "
                        "graphics, "
                        "graphics2d, "
                        "ground, "
                        "speedindex, "
                        "topindex, "
                        "container, "
                        "stackable, "
                        "usable, "
                        "readable, "
                        "writable, "
                        "fluidcontainer, "
                        "splash, "
                        "movable, "
                        "pickupable, "
                        "blocking, "
                        "floorchange, "
                        "readability_len, "
                        "lightcolor, "
                        "lightradius, "
                        "height, "
                        "height2d_x, height2d_y, "
                        "minimapcolor, "
                        "spritelist, "
                        "otid "
                        ") values (%d, '%q', '%q', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %f, %d, %d, %d, '%q', %d);", NULL, NULL, NULL,

                        datversion, /* part of table name */


                        itemid,
                        i->graphics,
                        i->graphics2d,
                        i->ground,
                        i->speedindex,
                        i->topindex,
                        i->container,
                        i->stackable,
                        i->usable,
                        i->readable,
                        i->writable,
                        i->fluidcontainer,
                        i->splash,
                        i->movable,
                        i->pickupable,
                        i->blocking,
                        i->floorchange,
                        i->readability_len,
                        i->lightcolor,
                        i->lightradius,
                        i->height,
                        i->height2d_x, i->height2d_y,
                        i->minimapcolor,
                        spritelist,
                        i->otid
                        ) != SQLITE_OK) return FALSE; else return TRUE;
    } else {
        if (dbexecprintf(fo, "update items%d set "
                        "graphics = '%q', "
                        "graphics2d = '%q', "
                        "ground = '%d', "
                        "speedindex = '%d', "
                        "topindex = '%d', "
                        "container = '%d', "
                        "stackable = '%d', "
                        "usable = '%d', "
                        "readable = '%d', "
                        "writable = '%d', "
                        "fluidcontainer = '%d', "
                        "splash = '%d', "
                        "movable = '%d', "
                        "pickupable = '%d', "
                        "blocking = '%d', "
                        "floorchange = '%d', "
                        "readability_len = '%d', "
                        "lightcolor = '%d', "
                        "lightradius = '%d', "
                        "height = '%f', "
                        "height2d_x = '%d', height2d_y = '%d', "
                        "minimapcolor = '%d', "
                        "spritelist = '%q', "
                        "otid = '%d' "


                        " where itemid = '%d';", NULL, 0, NULL,
                        datversion,
                        i->graphics,
                        i->graphics2d,
                        i->ground,
                        i->speedindex,
                        i->topindex,
                        i->container,
                        i->stackable,
                        i->usable,
                        i->readable,
                        i->writable,
                        i->fluidcontainer,
                        i->splash,
                        i->movable,
                        i->pickupable,
                        i->blocking,
                        i->floorchange,
                        i->readability_len,
                        i->lightcolor,
                        i->lightradius,
                        i->height,
                        i->height2d_x, i->height2d_y,
                        i->minimapcolor,
                        spritelist,
                        i->otid,

                        itemid) != SQLITE_OK) return FALSE; else return TRUE;

    }

}




BOOL insertcreature (unsigned short itemid, item_t *i) {

    char spritelist[4096];
    char *spritelistptr;
    unsigned short j;
    spritelist_t *sl = i->sl;

    spritelistptr = spritelist + sprintf(spritelist, "%d %d %d %d %d %d %d %d ", (unsigned int)sl->width, (unsigned int)sl->height, (unsigned int)sl->blendframes, (unsigned int)sl->xdiv, (unsigned int)sl->ydiv, (unsigned int)sl->animcount, (unsigned int)sl->unknown, (unsigned int)sl->numsprites);

    for (j = 0; j < sl->numsprites; ++j) {
        spritelistptr += sprintf(spritelistptr, "%d ", (unsigned int)sl->spriteids[j]);
    }

    if (!entryexists_creatureid(itemid)) {

        if (dbexecprintf(fo, "insert into creatures%d ("
                        "creatureid, "
                        "graphics, "
                        "graphics2d, "
                        "spritelist"
                        ") values (%d, '%q', '%q', '%q');", NULL, NULL, NULL,

                        datversion, /* part of table name */


                        itemid,
                        i->graphics,
                        i->graphics2d,
                        spritelist
                        ) != SQLITE_OK) return FALSE; else return TRUE;
    } else {
        if (dbexecprintf(fo, "update creatures%d set "
                        "graphics = '%q', "
                        "graphics2d = '%q', "
                        "spritelist = '%q'"

                        " where creatureid = '%d';", NULL, 0, NULL,
                        datversion,
                        i->graphics,
                        i->graphics2d,
                        spritelist,

                        itemid) != SQLITE_OK) return FALSE; else return TRUE;

    }

}


void show_progress(int currentid, int dat_items) {

    if ((currentid * 10) / dat_items > lastpercentage/10) {
        printf("%d%%", (currentid * 100) / dat_items == 1 ? 0 : (currentid * 100) / dat_items);
        lastpercentage = (currentid * 100) / dat_items;
        if (lastpercentage == 100) printf("\n"); else printf("...");
    }

}
static int patchitemfunc(void *itemvoid, int argc, char **argv, char **azColName) {
    item_t *item = (item_t*)itemvoid;
    int i;

    for (i=0; i < argc ; i++) {
        if (!strcmp(azColName[i], "graphics")) {
            strcpy(item->graphics, argv[i]);
        }
        if (!strcmp(azColName[i], "graphics2d")) {
            strcpy(item->graphics, argv[i]);
        }
        if (!strcmp(azColName[i], "height")) {
            sscanf(argv[i], "%lf", &item->height);
        }
    }
    /*printf("Patched!\n");*/
    return 0;
}
void patchitem (unsigned int itemid, item_t *item) {
    dbexecprintf(fo, "select graphics, height from items%d where itemid = '%d';", patchitemfunc, item, NULL, datversion, itemid);

}
int main (int argc, char **argv) {
	int rc;
	int size;
	int currentid;
	item_t item;

	printf("The Outcast DAT Convertor\n---\n");

	if (argc != 4) {
		printf("usage: %s tibia.dat outcast.db datversion\n", strrchr(argv[0], '\\'));
		printf("\n");
		printf("Non-empty DB will not be purged, except the items table\n");
		printf("for specified datversion\n");
		printf("Outcast-specific data are kept (fields: graphics, graphics2d, height)\n");
		printf("OTID is also not touched\n");
		printf("\n");
		return 0;
	}
	sscanf(argv[3], "%d\n", &datversion);
	fi = fopen(argv[1], "rb");
	if (!fi) {
		printf("Input file does not exist (%s)\n", argv[1]);
		return 1;
	}
	fseek(fi,0,SEEK_END);
	size = ftell(fi);
	fseek(fi,0,SEEK_SET);

	rc = sqlite3_open(argv[2], &fo);
	if (rc != SQLITE_OK) {
		printf("Output file can't be opened (%s)\n", argv[2]);
		return 2;
	}
	if (!check_tables()) {
		printf("Table check & correction failed.\n");
		return 3;
	}
	if (!dat_load_header()) {
	    printf("Header loading failed.\n");
	    return 4;
	}
	printf("READING ITEMS...\n");
	currentid = 100;
	lastpercentage = -100;
	dbexec(fo, "begin transaction;", NULL, NULL, NULL);
	while (ftell(fi) < size && currentid <= dat_items) {
	    /*printf("Item %d\n", currentid);*/
        show_progress(currentid, dat_items);
        if (!dat_readitem(&item)) {
            printf("Reading item %d failed.\n", currentid);
            return 5;
        }
        patchitem (currentid, &item);
        if (!insertitem(currentid, &item)) {
            printf("Inserting item %d failed.\n", currentid);
            return 6;
        } else {
            free(item.sl->spriteids);
            free(item.sl);
        }
        currentid ++;
	}
	printf("End reading items at %d\n", ftell(fi));

    printf("READING MONSTERS...\n");
	currentid = 1;
	lastpercentage = -100;
	dbexec(fo, "begin transaction;", NULL, NULL, NULL);
	while (ftell(fi) < size && currentid <= dat_creatures) {
	    /*printf("Item %d\n", currentid);*/
        show_progress(currentid, dat_creatures);
        if (!dat_readitem(&item)) {
            printf("Reading creature %d failed.\n", currentid);
            return 5;
        }
        patchitem (currentid, &item);
        if (!insertcreature(currentid, &item)) {
            printf("Inserting creature %d failed.\n", currentid);
            return 6;
        } else {
            free(item.sl->spriteids);
            free(item.sl);
        }
        currentid ++;
	}
	printf("End reading creatures at %d\n", ftell(fi));
	dbexec(fo, "end transaction;", NULL, NULL, NULL);
    fclose(fi);
    printf("DONE\n");
	return 0;
}
