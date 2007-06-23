#ifndef __TYPES_H
#define __TYPES_H

#include <GLICT/button.h>
#include <string>


// first simple type renames
typedef		unsigned short int					itemid_t;
// then some structs
struct character_t {
    char charactername[128];
    char worldname[128];
    unsigned long ipaddress;
    unsigned short port;
    glictButton* button;
} ; // character list element


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

    unsigned short usecount;
} spritelist_t;


struct item_t { // this type of struct declaration is necessary to be able to place stl variables inside struct
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
    bool rune;
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
    unsigned char extraproperty;
    char spritelist[4096];
    unsigned short otid;

    void *textures;
    std::vector<int> animation_framelist_stand;
    std::vector<int> animation_framelist_move;
    spritelist_t sli;
    bool loaded;
} ;
struct creature_t {
    char graphics[50];
    char graphics2d[50];
    char spritelist[4096];

    void *textures;
    std::vector<int> animation_framelist_stand;
    std::vector<int> animation_framelist_move;
    spritelist_t sli;
    bool loaded;
} ;
struct effect_t {
    char graphics[50];
    char graphics2d[50];
    char spritelist[4096];

    void *textures;
    std::vector<int> animation_framelist_stand;
    std::vector<int> animation_framelist_move;
    spritelist_t sli;
    bool loaded;
} ;

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

typedef enum {
    NORMAL=0x01,
    YELL,
    BROADCAST,
    PRIVATE=0x04
} speaktype_t;
typedef enum {
    NORTH = 0,
    EAST,
    SOUTH,
    WEST,
    STOP,
    NORTHEAST,
    SOUTHEAST,
    SOUTHWEST,
    NORTHWEST
} direction_t;

typedef struct  {
    unsigned char major, minor;
    unsigned int revision;
} version_t;
typedef struct {
    float r,g,b,a;
} color_t;


typedef enum {
	FLUID_EMPTY   = 0x00,
	FLUID_BLUE	  = 0x01,
	FLUID_RED	  = 0x02,
	FLUID_BROWN   = 0x03,
	FLUID_GREEN   = 0x04,
	FLUID_YELLOW  = 0x05,
	FLUID_WHITE   = 0x06,
	FLUID_PURPLE  = 0x07
} fluidcolors_t;

typedef enum {
	SKULL_NONE = 0,
	SKULL_YELLOW = 1,
	SKULL_GREEN = 2,
	SKULL_WHITE = 3,
	SKULL_RED = 4,
} skull_t;

typedef enum {
	ICON_POISON = 1,
	ICON_BURN = 2,
	ICON_ENERGY =  4,
	ICON_DRUNK = 8,
	ICON_MANASHIELD = 16,
	ICON_PARALYZE = 32,
	ICON_HASTE = 64,
	ICON_SWORDS = 128,
	ICON_DROWNING = 256
} statusicons_t;

#endif
