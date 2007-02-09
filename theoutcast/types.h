#ifndef __TYPES_H
#define __TYPES_H

#include <GLICT/button.h>
#include <string>


// first simple type renames
typedef		unsigned short int					itemid_t;
// then some structs
typedef struct {
    char charactername[128];
    char worldname[128];
    unsigned long ipaddress;
    unsigned short port;
    glictButton* button;
} character_t; // character list element


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
    bool ground;
    unsigned short speedindex;
    unsigned char topindex;
    bool container;
    bool stackable;
    bool usable;
    bool readable;
    bool writable;
    bool fluidcontainer;
    bool splash;
    bool movable;
    bool pickupable;
    bool blocking;
    bool floorchange;
    unsigned short readability_len;
    unsigned char lightcolor;
    unsigned char lightradius;
    double height;
    unsigned char height2d_x, height2d_y;
    unsigned short minimapcolor;
    char spritelist[4096];
    unsigned short otid;

    void *textures;
    unsigned char animation_framecount[2];
    spritelist_t sli;
    bool loaded;
} item_t;
typedef struct {
    char graphics[50];
    char graphics2d[50];
    char spritelist[4096];

    void *textures;
    unsigned char animation_framecount[2];
    spritelist_t sli;
    bool loaded;
} creature_t;

typedef struct {
    unsigned short x, y;
    unsigned char z;
} position_t;

typedef struct {
    unsigned short type;
    unsigned short head;
    unsigned short body;
    unsigned short legs;
    unsigned short feet;
    unsigned short addons;

    unsigned short extendedlook;
} creaturelook_t;
// then some enums
typedef enum {
    NONE = 0,
    CHARLIST = 1,
    GAMEWORLD = 2
} connectiontype_t;
typedef enum {
    FIST = 0,
    CLUB,
    SWORD,
    AXE,
    DISTANCE,
    SHIELDING,
    FISHING
} skill_t;

typedef enum {
    OFFENSIVE=1,
    BALANCED=2,
    DEFENSIVE=3
} stanceaggression_t;
typedef enum {
    STAND=0,
    CHASE=1
} stancechase_t;


#endif
