#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

/* defines */
#define BOOL char
#define TRUE 1
#define FALSE 0
/* structs */
typedef struct {
    char graphics[50];
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
    unsigned short otid;
} item_t;

/* files */
FILE *fi;
sqlite3 *fo;

/* ui stuff */
char lastpercentage;

/* options */
int datversion;

/* header data */
unsigned short dat_items, dat_monsters, dat_effects, dat_distance;



int dbexecprintf(sqlite3* db, const char *sql, sqlite3_callback cb, void *arg, char **errmsg, ...) {
	va_list vl;
	va_start(vl, sql);

	char *z = sqlite3_vmprintf(sql, vl);

    /*printf("QUERY: %s\n", z);*/

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
	if (!tableexists("items")) {
		printf("Creating table 'items'.\n");
		if (dbexec(fo, "create table items ("
			"itemid integer primary key," /* item id, as the server sends it to us */
			"graphics varchar[50], " /* 3d graphics file */
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
			"otid integer, " /* under what id does OTserv store this item */
			"protocolversion integer " /* under what id does OTserv store this item */
			"); ",NULL, 0, NULL) != SQLITE_OK) {
				printf("Table 'items' creation failed\n");
				return 0;
		}
	}
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
    fread(&dat_monsters, 2, 1, fi);
    printf("Monsters: %d\n", dat_monsters);
    fread(&dat_effects, 2, 1, fi);
    printf("Effects: %d\n", dat_effects);
    fread(&dat_distance, 2, 1, fi);
    printf("Distance shot effects: %d\n", dat_distance);

}
void clear_item(item_t* item) {

    item->graphics[0] = 0;
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
                    case 0x06: /* ladder */
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
                    case 0x1B: /* some monsters */
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


            default:
                printf("UNKNOWN DAT VERSION! Can't read any chunks.\n");
                return 0;
        }
    }

    switch (datversion) {
        case 770:
            width = fgetc(fi);
            height = fgetc(fi);
            if (width > 1 || height > 1) fgetc(fi);
            blendframes = fgetc(fi);
            xdiv = fgetc(fi);
            ydiv = fgetc(fi);
            animcount = fgetc(fi);
            unknown = fgetc(fi); /* this does not exist for versions before 7.55 */
            numsprites = width * height * blendframes * xdiv * ydiv * animcount * unknown;
            /*printf("width %d height %d blendframes %d xdiv %d ydiv %d animcount %d unknown %d\n", width, height, blendframes, xdiv, ydiv, animcount, unknown);
            printf("sprites %d\n", numsprites);*/
            for(i = 0; i < numsprites; ++i)
                readu16(); /* sprite id */
            break;
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

    if (dbexecprintf(fo, "select * from items where itemid='%d';", &extryexistsfunc, &returner, NULL, itemid) == SQLITE_OK) return returner; else return FALSE;

}
BOOL gettrue () { return TRUE ; }
BOOL insertitem (unsigned short itemid, item_t *i) {
    char command[20];
    char where[35]={0};

    if (!entryexists_itemid(itemid)) {

        if (dbexecprintf(fo, "insert into items ("
                        "itemid, "
                        "graphics, "
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
                        "otid, "
                        "protocolversion"
                        ") values (%d, '%q', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %f, %d, %d, %d, %d, %d);", NULL, NULL, NULL,

                        itemid,
                        i->graphics,
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
                        i->otid,
                        datversion
                        ) != SQLITE_OK) return FALSE; else return TRUE;
    } else {
        if (dbexecprintf(fo, "update items set "
                        "graphics = '%q', "
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
                        "otid = '%d', "
                        "protocolversion = '%d'"


                        " where itemid = '%d' and protocolversion = '%d';", NULL, 0, NULL,
                        i->graphics,
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
                        i->otid,
                        datversion,

                        itemid, datversion) != SQLITE_OK) return FALSE; else return TRUE;

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
        if (!strcmp(azColName[i], "height")) {
            sscanf(argv[i], "%lf", &item->height);
        }
    }
    /*printf("Patched!\n");*/
    return 0;
}
void patchitem (unsigned int itemid, item_t *item) {
    dbexecprintf(fo, "select graphics, height from items where itemid = '%d';", patchitemfunc, item, NULL, itemid);

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
		printf("Outcast-specific data are kept (fields: graphics, height)\n");
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
        }

        currentid ++;

	}
	dbexec(fo, "end transaction;", NULL, NULL, NULL);
    printf("DONE\n");
	return 0;
}
