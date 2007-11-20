#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <malloc.h>


#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "importotb.h"
#include "importotxml.h"

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
    BOOL rune;
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
    unsigned char extraproperty;
    BOOL ladder;

    char spritelist[4096];
    unsigned short otid;
    char otname[255];
    char materialname[50];
    spritelist_t *sl;
} item_t;

/* files */
FILE *fi;
sqlite3 *fo;

/* ui stuff */
char lastpercentage;

char **mainargv;
int mainargc;


/* options */
int datversion;
BOOL reverse_spr_lookup;
BOOL no_dat;
BOOL import_otb, import_otxml, import_materials;

/* header and file info */
unsigned short dat_items, dat_creatures, dat_effects, dat_distances;
int currentid, insize;
int maxsprid=0;
int current_rsl_entry;


#ifndef WIN32
static int filesize (FILE* f) {
	int loc = ftell(f);
	int size = 0;

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, loc, SEEK_SET);
	return size;
}
#else
static int filesize (FILE* f) {
#ifdef _MSC_VER
	return _filelength(_fileno(f));
#else
	return filelength(fileno(f)) ;
#endif
}
#endif


BOOL debugqueries = FALSE;

int dbexecprintf(sqlite3* db, sqlite3_callback cb, void *arg, char **errmsg, const char *sql, ...) {
	va_list vl;
	va_start(vl, sql);

	if (debugqueries) printf("vmprint for %s\n", sql);

	char *z = sqlite3_vmprintf(sql, vl);

	if (debugqueries) printf("query\n");
	if (debugqueries) printf("QUERY: %s\n", z);

	int rc = sqlite3_exec(db, z, cb, arg, errmsg);
	if (rc != SQLITE_OK) printf("SQLite: Error: '%s', RC: %d, query '%s'\n", sqlite3_errmsg(fo), rc, z);

	sqlite3_free(z);

	va_end(vl);
	return rc;
}
int dbexec(sqlite3* db, const char *sql, sqlite3_callback cb, void *arg, char **errmsg) {
    if (debugqueries) printf("QUERY: %s\n", sql);
    sqlite3_exec(db, sql, cb, arg, errmsg);
}
char tableexists(const char *tablename) {
	return (dbexecprintf(fo, NULL, 0, NULL, "select * from %s;", tablename) == SQLITE_OK);
}
char check_tables() {
    char tablename[30];
    sprintf(tablename, "items%d", datversion);
	if (!tableexists(tablename)) {
		printf("Creating table '%s'.\n", tablename);
		if (dbexecprintf(fo, NULL, 0, NULL, "create table %s ("

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
			"rune boolean, " /* is this a rune item, meaning, same as countable but with only one spr look */
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
			"extraproperty integer," /* additional property, set up with packet 0x1D */
			"ladder boolean," /* determines if the object are ladders giving them extra priority */
			"spritelist varchar[4096], " /* spritelist */
			"otid integer, " /* under what id does OTserv store this item */
			"otname varchar[255]," /* under what id does OTserv store this item */
			"materialname varchar[50]" /* what's the category in simone's mapedit */
			"); ",tablename) != SQLITE_OK) {
				printf("Table '%s' creation failed\n", tablename);
				return 0;
		}
	}
    sprintf(tablename, "creatures%d", datversion);
	if (!tableexists(tablename)) {
		printf("Creating table '%s'.\n", tablename);
		if (dbexecprintf(fo, NULL, 0, NULL, "create table %s ("
            "creatureid integer primary key," /* creature id, as the server sends it to us */
            "graphics varchar[50], " /* 3d graphics file */
			"graphics2d varchar[50], " /* 3d graphics file */
            "spritelist varchar[4096] " /* spritelist */
            "); ", tablename) != SQLITE_OK) {
                printf("Table '%s' creation failed\n", tablename);
				return 0;
            }
	}
    sprintf(tablename, "effects%d", datversion);
	if (!tableexists(tablename)) {
		printf("Creating table '%s'.\n", tablename);
		if (dbexecprintf(fo, NULL, 0, NULL, "create table %s ("
            "effectid integer primary key," /* creature id, as the server sends it to us */
            "graphics varchar[50], " /* 3d graphics file */
			"graphics2d varchar[50], " /* 3d graphics file */
            "spritelist varchar[4096], " /* spritelist */
            "soundfile varchar[50] " /* sound that'll be played for this effect */
            "); ", tablename) != SQLITE_OK) {
                printf("Table '%s' creation failed\n", tablename);
				return 0;
            }
	}
    sprintf(tablename, "distances%d", datversion);
	if (!tableexists(tablename)) {
		printf("Creating table '%s'.\n", tablename);
		if (dbexecprintf(fo, NULL, 0, NULL, "create table %s ("
            "distanceid integer primary key," /* creature id, as the server sends it to us */
            "graphics varchar[50], " /* 3d graphics file */
			"graphics2d varchar[50], " /* 3d graphics file */
            "spritelist varchar[4096], " /* spritelist */
            "soundfile varchar[50] " /* sound that'll be played for this effect */
            "); ", tablename) != SQLITE_OK) {
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
    fread(&dat_distances, 2, 1, fi);
    printf("Distance shots: %d\n", dat_distances);

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
    item->rune = FALSE;
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
    item->extraproperty = 0;
    item->ladder = FALSE;
    item->spritelist[0] = 0;
    item->otid = 0;
    item->otname[0] = 0;
    item->materialname[0] = 0;

}
char dat_readitem(item_t *item) {
    unsigned char option;
    unsigned char tmpchar;

    unsigned char width, height, blendframes, xdiv, ydiv, animcount, unknown;
    unsigned int i;

    unsigned short numsprites;

    int givemoreinfo=0;

    clear_item(item);

    for (option = fgetc(fi); option != 0xFF; option = fgetc(fi)) {

#if 0
        if (currentid == 2177 || currentid == 2178 ) {
            givemoreinfo = 1;
            printf("%d Byte %02x\n", currentid, option);
        } else
            givemoreinfo = 0;
#endif

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
                        item->ladder = TRUE;
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
                        item->extraproperty = tmpchar;
                        if(tmpchar == 0x58)
                            item->readable = TRUE;

                        fgetc(fi); /* always 4 */
                        break;
                    case 0x1E: /* ground items */
                        break;
                    case 0xFF: /* end of section?  */

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
                        item->ladder = TRUE;
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
                        item->extraproperty = tmpchar;
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
			case 800:
				/*if (currentid == 54  || currentid == 67 || currentid == 93 || currentid == 97 || currentid == 99) printf("%d -- option %02x\n", currentid, option);*/
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
                        item->ladder = TRUE;
                        break;
                    case 0x07: /* usable? */
                        item->usable = TRUE;
                        break;
                    case 0x08: /* runes */
                        item->rune = TRUE;
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
                        /*if (currentid == 54  || currentid == 67 || currentid == 93 || currentid == 97 || currentid == 99) {*/
                        	/*printf("%d -- %04x %04x\n", currentid, readu16(), readu16());*/
						/*} else {*/
							readu16();
							readu16();
						/*}*/
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
                        item->extraproperty = tmpchar;
                        if (givemoreinfo) printf("%d\n", tmpchar);
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
        case 792:
		case 800: {
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
                if (sl->spriteids[i] > maxsprid) maxsprid = sl->spriteids[i];
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

    if (dbexecprintf(fo, &extryexistsfunc, &returner, NULL, "select * from items%d where itemid='%d';", datversion, itemid, datversion) == SQLITE_OK) return returner; else return FALSE;
}
char entryexists_creatureid(unsigned int itemid) {
    BOOL returner = FALSE;

    if (dbexecprintf(fo, &extryexistsfunc, &returner, NULL, "select * from creatures%d where creatureid='%d';", datversion, itemid, datversion) == SQLITE_OK) return returner; else return FALSE;
}
char entryexists_effectid(unsigned int itemid) {
    BOOL returner = FALSE;

    if (dbexecprintf(fo, &extryexistsfunc, &returner, NULL, "select * from effects%d where effectid='%d';", datversion, itemid, datversion) == SQLITE_OK) return returner; else return FALSE;
}
char entryexists_distanceid(unsigned int itemid) {
    BOOL returner = FALSE;

    if (dbexecprintf(fo, &extryexistsfunc, &returner, NULL, "select * from distances%d where distanceid='%d';", datversion, itemid, datversion) == SQLITE_OK) return returner; else return FALSE;
}
BOOL gettrue () {
    return TRUE ;
}
BOOL insertitem (unsigned short itemid, item_t *i) {

    char spritelist[4096];
    char *spritelistptr;
    unsigned short j;
    spritelist_t *sl = i->sl;

    spritelistptr = spritelist + sprintf(spritelist, "%d, %d, %d, %d, %d, %d, %d, %d, ", (unsigned int)sl->width, (unsigned int)sl->height, (unsigned int)sl->blendframes, (unsigned int)sl->xdiv, (unsigned int)sl->ydiv, (unsigned int)sl->animcount, (unsigned int)sl->unknown, (unsigned int)sl->numsprites);

    for (j = 0; j < sl->numsprites; ++j) {
        spritelistptr += sprintf(spritelistptr, "%d, ", (unsigned int)sl->spriteids[j]);
    }

    if (!entryexists_itemid(itemid)) {

        if (dbexecprintf(fo, NULL, NULL, NULL, "insert into items%d ("
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
                        "rune, "
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
                        "extraproperty, "
                        "ladder, "
                        "spritelist, "
                        "otid, "
                        "otname, "
                        "materialname"
                        ") values (%d, '%q', '%q', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %f, %d, %d, %d, %d, %d, '%q', %d, '%q', '%q');",

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
                        i->rune,
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
                        i->extraproperty,
                        i->ladder,
                        spritelist,
                        i->otid,
                        i->otname,
                        i->materialname
                        ) != SQLITE_OK) return FALSE; else return TRUE;
    } else {
        if (dbexecprintf(fo, NULL, 0, NULL, "update items%d set "
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
                        "rune = '%d', "
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
                        "extraproperty = '%d', "
                        "ladder = '%d', "
                        "spritelist = '%q', "
                        "otid = '%d', "
                        "otname = '%q', "
                        "materialname = '%q' "


                        " where itemid = '%d';",
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
                        i->rune,
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
                        i->extraproperty,
                        i->ladder,
                        spritelist,
                        i->otid,
                        i->otname,
                        i->materialname,

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

        if (dbexecprintf(fo, NULL, NULL, NULL,"insert into creatures%d ("
                        "creatureid, "
                        "graphics, "
                        "graphics2d, "
                        "spritelist"
                        ") values (%d, '%q', '%q', '%q');",

                        datversion, /* part of table name */


                        itemid,
                        i->graphics,
                        i->graphics2d,
                        spritelist
                        ) != SQLITE_OK) return FALSE; else return TRUE;
    } else {
        if (dbexecprintf(fo, NULL, 0, NULL,"update creatures%d set "
                        "graphics = '%q', "
                        "graphics2d = '%q', "
                        "spritelist = '%q'"

                        " where creatureid = '%d';",
                        datversion,
                        i->graphics,
                        i->graphics2d,
                        spritelist,

                        itemid) != SQLITE_OK) return FALSE; else return TRUE;

    }

}

BOOL inserteffect (unsigned short itemid, item_t *i) {

    char spritelist[4096];
    char *spritelistptr;
    unsigned short j;
    spritelist_t *sl = i->sl;

    spritelistptr = spritelist + sprintf(spritelist, "%d %d %d %d %d %d %d %d ", (unsigned int)sl->width, (unsigned int)sl->height, (unsigned int)sl->blendframes, (unsigned int)sl->xdiv, (unsigned int)sl->ydiv, (unsigned int)sl->animcount, (unsigned int)sl->unknown, (unsigned int)sl->numsprites);

    for (j = 0; j < sl->numsprites; ++j) {
        spritelistptr += sprintf(spritelistptr, "%d ", (unsigned int)sl->spriteids[j]);
    }

    if (!entryexists_effectid(itemid)) {

        if (dbexecprintf(fo, NULL, NULL, NULL,"insert into effects%d ("
                        "effectid, "
                        "graphics, "
                        "graphics2d, "
                        "spritelist"
                        ") values (%d, '%q', '%q', '%q');",

                        datversion, /* part of table name */


                        itemid,
                        i->graphics,
                        i->graphics2d,
                        spritelist
                        ) != SQLITE_OK) return FALSE; else return TRUE;
    } else {
        if (dbexecprintf(fo, NULL, 0, NULL,"update effects%d set "
                        "graphics = '%q', "
                        "graphics2d = '%q', "
                        "spritelist = '%q'"

                        " where effectid = '%d';",
                        datversion,
                        i->graphics,
                        i->graphics2d,
                        spritelist,

                        itemid) != SQLITE_OK) return FALSE; else return TRUE;

    }

}



BOOL insertdistance (unsigned short itemid, item_t *i) {

    char spritelist[4096];
    char *spritelistptr;
    unsigned short j;
    spritelist_t *sl = i->sl;

    spritelistptr = spritelist + sprintf(spritelist, "%d %d %d %d %d %d %d %d ", (unsigned int)sl->width, (unsigned int)sl->height, (unsigned int)sl->blendframes, (unsigned int)sl->xdiv, (unsigned int)sl->ydiv, (unsigned int)sl->animcount, (unsigned int)sl->unknown, (unsigned int)sl->numsprites);

    for (j = 0; j < sl->numsprites; ++j) {
        spritelistptr += sprintf(spritelistptr, "%d ", (unsigned int)sl->spriteids[j]);
    }

    if (!entryexists_distanceid(itemid)) {

        if (dbexecprintf(fo, NULL, NULL, NULL,"insert into distances%d ("
                        "distanceid, "
                        "graphics, "
                        "graphics2d, "
                        "spritelist"
                        ") values (%d, '%q', '%q', '%q');",

                        datversion, /* part of table name */


                        itemid,
                        i->graphics,
                        i->graphics2d,
                        spritelist
                        ) != SQLITE_OK) return FALSE; else return TRUE;
    } else {
        if (dbexecprintf(fo, NULL, 0, NULL,"update distances%d set "
                        "graphics = '%q', "
                        "graphics2d = '%q', "
                        "spritelist = '%q'"

                        " where distanceid = '%d';",
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
        fflush(stdout);
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
        if (!strcmp(azColName[i], "otid")) {
            sscanf(argv[i], "%hd", &item->otid );
        }
        if (!strcmp(azColName[i], "otname")) {
            strcpy(item->otname, argv[i]);
        }
        if (!strcmp(azColName[i], "materialname")) {
            strcpy(item->materialname, argv[i]);
        }
    }
    /*printf("Patched!\n");*/
    return 0;
}
void patchitem (unsigned int itemid, item_t *item) {
    dbexecprintf(fo, patchitemfunc, item, NULL, "select graphics, graphics2d, height, otid, otname, materialname from items%d where itemid = '%d';", datversion, itemid);

}
int read_items() {
	item_t item;
	printf("READING ITEMS...\n");
	currentid = 100;
	lastpercentage = -100;

	while (ftell(fi) < insize && currentid <= dat_items) {
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
	return 0;
}
int  read_monsters() {
	item_t item;
	printf("READING MONSTERS...\n");
	currentid = 1;
	lastpercentage = -100;
	while (ftell(fi) < insize && currentid <= dat_creatures) {
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
	return 0;
}
int read_effects() {
	item_t item;
	printf("READING EFFECTS...\n");
	currentid = 1;
	lastpercentage = -100;
	while (ftell(fi) < insize && currentid <= dat_effects) {
	    /*printf("Effect %d\n", currentid);*/
        show_progress(currentid, dat_effects);
        if (!dat_readitem(&item)) {
            printf("Reading effect %d failed.\n", currentid);
            return 5;
        }
        patchitem (currentid, &item);
        if (!inserteffect(currentid, &item)) {
            printf("Inserting effect %d failed.\n", currentid);
            return 6;
        } else {
            free(item.sl->spriteids);
            free(item.sl);
        }
        currentid ++;
	}
	printf("End reading effects at %d\n", ftell(fi));
	return 0;

}
int read_distances() {
	item_t item;
	printf("READING DISTANCE SHOTS...\n");
	currentid = 1;
	lastpercentage = -100;
	while (ftell(fi) < insize && currentid <= dat_effects) {
	    /*printf("Effect %d\n", currentid);*/
        show_progress(currentid, dat_distances);
        if (!dat_readitem(&item)) {
            printf("Reading distance shot %d failed.\n", currentid);
            return 5;
        }
        patchitem (currentid, &item);
        if (!insertdistance(currentid, &item)) {
            printf("Inserting distance shot %d failed.\n", currentid);
            return 6;
        } else {
            free(item.sl->spriteids);
            free(item.sl);
        }
        currentid ++;
	}
	printf("End reading distance shots at %d\n", ftell(fi));
	return 0;

}
void print_usage() {
	printf("usage: %s tibia.dat outcast.db datversion [tibia.opq [options]]\n", strrchr(mainargv[0], '\\'));
	printf("\n");
	printf("Non-empty DB will not be purged, except the items table\n");
	printf("for specified datversion\n");
	printf("Outcast-specific data are kept (fields: graphics, graphics2d, height)\n");
	printf("OTID is also not touched, neither is OTName\n");
	printf("If specified, queries in tibia.opq are executed (opq == outcast patch queries)\n");
	printf("\n");
	printf("Options are as follows:\n");
	printf("* reverse_spr_lookup: generates table for looking up which items, monsters and \n");
	printf("                      effects use the particular sprite\n");
	printf("* no_dat: skips .DAT conversion. Note that .DAT file unfortunately must exists\n");
	printf("          anyway.\n");
	printf("\n");
}



int reverse_spr_lookup_cb(void *arg, int argc, char **argv, char **azColName) {
	unsigned short currentsprite;
	spritelist_t sli;
	int type= (int)arg;
	int i, j,k;
	unsigned short* sprids;

	for (i = 0; i < argc; ++i) {
		if (type==0) {
			if (!strcmp(azColName[i], "itemid")) break;
        } else if (type==1) {
        	if (!strcmp(azColName[i], "creatureid")) break;
        } else if (type==2) {
        	if (!strcmp(azColName[i], "effectid")) break;
        } else if (type==3) {
        	if (!strcmp(azColName[i], "distanceid")) break;
        }

    }
    sscanf(argv[i], "%d", &currentid);


	switch (type) {
		case 0:
			type ='I';
			show_progress(currentid, dat_items);
			break;
		case 1:
			type = 'C';
			show_progress(currentid, dat_creatures );
			break;
		case 2:
			type = 'E';
			show_progress(currentid, dat_effects);
			break;
		case 3:
			type = 'D';
			show_progress(currentid, dat_distances);
			break;
	}


	for (i = 0; i < argc; ++i) {
        if (!strcmp(azColName[i], "spritelist")) {
			char *p = argv[i];
			sscanf(p, "%hhd", &sli.width); p = strchr(p, ' ')+1;
			sscanf(p, "%hhd", &sli.height); p = strchr(p, ' ')+1;
			sscanf(p, "%hhd", &sli.blendframes); p = strchr(p, ' ')+1;
			sscanf(p, "%hhd", &sli.xdiv); p = strchr(p, ' ')+1;
			sscanf(p, "%hhd", &sli.ydiv); p = strchr(p, ' ')+1;
			sscanf(p, "%hhd", &sli.unknown); p = strchr(p, ' ')+1;
			sscanf(p, "%hhd", &sli.animcount); p = strchr(p, ' ')+1;
			sscanf(p, "%hd", &sli.numsprites); p = strchr(p, ' ')+1;
			/*printf("read into sli--%d sprites\n", sli.numsprites);*/

			sprids = (unsigned short*)malloc(sli.numsprites * sizeof(unsigned short));
			for (j = 0; j < sli.numsprites; j++) {
				char tmp[255];
				BOOL canadd = TRUE;
				sscanf(p, "%hd", &currentsprite); p = strchr(p, ' ')+1;
				sprids[j] = currentsprite;
				for (k = 0; k < j; k++) {
					if (currentsprite == sprids[k]) {
						canadd = FALSE;
						break;
					}
				}
				if (canadd)
					dbexecprintf(fo, NULL, 0, NULL, "insert into rspritelookup%d (entryid, spriteid, type, itemid) values (%d, %d, '%c', %d);",
								datversion, (current_rsl_entry++), currentsprite, type, currentid);
				/*printf("inserted %s\n", tmp);*/

			}
			free(sprids);


        }

    }
	/*printf("ok\n");*/
	return 0;
}

void reverse_spr_lookup_build() {
	/* check and build tables */

    char tablename[30];
    sprintf(tablename, "rspritelookup%d", datversion);
	if (!tableexists(tablename)) {
		printf("Creating table '%s'.\n", tablename);
		if (dbexecprintf(fo, NULL, 0, NULL, "create table %s ("

			"entryid integer primary key,"
			"spriteid integer, "
			"type integer, " /* is it a I, C, E or D -- item, creature, effect or distance shot entry */
			"itemid integer " /* item, creature or effect id */
			"); ",tablename) != SQLITE_OK) {
				printf("Table '%s' creation failed\n", tablename);
				return;
		}
	} else {
		dbexecprintf(fo, NULL, 0, NULL, "delete from %s;", tablename);
	}


	current_rsl_entry = 0;
	lastpercentage = -100; printf("Items:    ");
	dbexecprintf(fo, reverse_spr_lookup_cb, (void*)0, NULL, "select itemid, spritelist from items%d order by itemid;", datversion);
	lastpercentage = -100; printf("Monsters: ");
	dbexecprintf(fo, reverse_spr_lookup_cb, (void*)1, NULL, "select creatureid, spritelist from creatures%d order by creatureid;", datversion);
	lastpercentage = -100; printf("Effects:  ");
	dbexecprintf(fo, reverse_spr_lookup_cb, (void*)2, NULL, "select effectid, spritelist from effects%d order by effectid;", datversion);
	lastpercentage = -100; printf("Distance shots:  ");
	dbexecprintf(fo, reverse_spr_lookup_cb, (void*)3, NULL, "select distanceid, spritelist from distances%d order by distanceid;", datversion);
}
static int readxmlinteger (xmlNodePtr node, const char* tag)
{
		int value;
        char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
        if(nodeValue){
                value = atoi(nodeValue);
                xmlFree(nodeValue);
                return value;
        }

        return -1;
}

void import_materials_do(const char *filename) {
	xmlDocPtr doc = xmlParseFile(filename);
	xmlNodePtr root, material, item;
	const char *name, *tmp;
	int ground, hangable, nogroup, from, to;
	char ground_s[32], hangable_s[32], nogroup_s[32];
	char query[4097], itemdeps[4097], itemdep[70];
	if(!doc){
		printf("Not a valid XML file.\n");
		return;
	}
	root = xmlDocGetRootElement(doc);
	if(xmlStrcmp(root->name,(const xmlChar*)"materials") != 0){
		printf("Not a materials XML file.\n");
		xmlFreeDoc(doc);
		return;
	}
	material = root->children;
	while (material) {
		name = xmlGetProp(material, (xmlChar*)"name");

		if (name) {

			/* only if we have a name can we have some hope of getting other props...*/
			printf("Setting up %s\n", name);
			ground = readxmlinteger (material, "ground");
			hangable = readxmlinteger (material, "hangable");
			nogroup = readxmlinteger (material, "nogroup");


			if (ground!=-1) sprintf(ground_s, "ground=\"%d\" and", ground); else ground_s[0] = 0;
			if (hangable!=-1) sprintf(hangable_s, "hangable=\"%d\" and", hangable); else hangable_s[0]=0;
			if (nogroup!=-1) sprintf(nogroup_s, "materials=\"\" and", nogroup); else nogroup_s[0]=0;

			itemdeps[0]  = 0;
			item = material->children;
			while (item) {
				if(xmlStrcmp(item->name,(const xmlChar*)"item") == 0){

					from = readxmlinteger(item, "fromid");
					to = readxmlinteger(item, "toid");
					if (to == -1) {
						sprintf(itemdep, "otid='%d' or ", from);
					} else {
						sprintf(itemdep, "(otid>='%d' and otid<='%d') or ", from, to);
					}
					strcat(itemdeps, itemdep);
				}
				item = item->next;
			}

			sprintf(query, "update items%d set materialname=\"%s\" where (%s%s%s%s %d);", datversion, name,
				ground_s,
				hangable_s,
				nogroup_s,
				itemdeps,
				itemdeps[0] ? 0 : 1);

			printf("%s\n", query);
			dbexec(fo,query, NULL, NULL, NULL);
			/*update items%d set (materialname) values () where ground=1 and itemid="" or*/





			xmlFree(name);

		}
		material = material->next;
	}


	xmlFreeDoc(doc);
}

int main (int argc, char **argv) {
	int rc,i;

	mainargv = argv;
	mainargc = argc;

	printf("\nThe Outcast DAT Convertor\n---\n");

	if (argc < 4) {
		print_usage();
		exit(0);
	}

	sscanf(argv[3], "%d\n", &datversion);
	fi = fopen(argv[1], "rb");
	if (!fi) {
		printf("Input file does not exist (%s)\n", argv[1]);
		return 1;
	}
	fseek(fi,0,SEEK_END);
	insize = ftell(fi);
	fseek(fi,0,SEEK_SET);

	rc = sqlite3_open(argv[2], &fo);
	if (rc != SQLITE_OK) {
		printf("Output file can't be opened (%s)\n", argv[2]);
		return 2;
	}



	/* ///////////////////// now parse other cmd line options /////////////////////////// */

	reverse_spr_lookup = FALSE;
	no_dat = FALSE;
	printf("Argc: %d\n", argc);
	for (i=5;i<argc;i++) {
		printf("Option: %s\n", argv[i]);
        if (!strcmp(argv[i], "reverse_spr_lookup")) {
        	printf("Reverse spr lookup\n");
        	reverse_spr_lookup = TRUE;
        } else if (!strcmp(argv[i], "no_dat")) {
        	printf("No dat parsing\n");
        	no_dat = TRUE;
        } else if (!strcmp(argv[i], "import_otb")) {
        	printf("Importing OTB\n");
        	import_otb = TRUE;
        } else if (!strcmp(argv[i], "import_otxml")) {
        	printf("Importing OTXML\n");
        	import_otxml = TRUE;
        } else if (!strcmp(argv[i], "import_materials")) {
        	printf("Importing MATERIALS\n");
        	import_materials = TRUE;
        } else {
        	print_usage();
        	return 0;
        }

    }


	if (!check_tables()) {
		printf("Table check & correction failed.\n");
		return 3;
	}
	if (!dat_load_header()) {
	    printf("Header loading failed.\n");
	    return 4;
	}
	if (!no_dat) {
		dbexec(fo, "begin transaction;", NULL, NULL, NULL);
		rc = read_items(); if (rc) { printf("Error reading items, exiting\n"); return rc; }
		rc = read_monsters(); if (rc) { printf("Error reading monsters, exiting\n"); return rc; }
		rc = read_effects(); if (rc) { printf("Error reading effects, exiting\n"); return rc; }
		rc = read_distances(); if (rc) { printf("Error reading distances, exiting\n"); return rc; }
		dbexec(fo, "end transaction;", NULL, NULL, NULL);
	} else {
		printf("Skipping DAT conversion\n");
	}
    fclose(fi);

    if (argc >= 5) {
        char query[8000]={0};
        int line = 1;
        printf("\nNow patching with outcast patch queries file...\n");
        FILE *f = fopen(argv[4], "r");
        if (!f) {
            printf("Patch file %s does not exist, aborting\n", argv[4]);
            goto skip_patch;
        }
        lastpercentage = -100;
        dbexec(fo, "begin transaction;", NULL, NULL, NULL);
        while (!feof(f)) {
            fgets(query, 8000, f);
            if (strlen(query)==0) break;
            show_progress(ftell(f), filesize(f));
            if (query[0] != '#' && query[0] != 13 && query[0] != 10) {
                char *errmsg;
                dbexec(fo, query, NULL, NULL, &errmsg);
                if (errmsg && strlen(errmsg)) printf("=> Query error at line %d %s\n", line, errmsg);
                free(errmsg);
                line ++;
            }
            query[0] = 0;

        }
        dbexec(fo, "end transaction;", NULL, NULL, NULL);
        fclose(f);

    }

    skip_patch:



    if (reverse_spr_lookup) {
		printf("Directed to build reverse SPR lookup table\n");
		reverse_spr_lookup_build();
		printf("End reverse_spr_lookup_build()\n");
    }

	if (import_otb) {
		char tmp[255];
		printf("Directed to import OTB\n");
/*		debugqueries = TRUE;*/
		sprintf(tmp, "items%d.otb", datversion);
		import_otb_do(tmp);
		printf("End import_otb()\n");
	}
	if (import_otxml) {
		char tmp[255];
		printf("Directed to import OTXML\n");
/*		debugqueries = TRUE;*/
		sprintf(tmp, "items%d.xml", datversion);
		import_otxml_do(tmp);
		printf("End import_otxml()\n");
	}
	if (import_materials) {
		char tmp[255];
		printf("Directed to import MATERIALS\n");
/*		debugqueries = TRUE;*/
		sprintf(tmp, "materials%d.xml", datversion);
		import_materials_do(tmp);
		printf("End import_materials()\n");
	}

    printf("Done.\n\n");

	return 0;
}
