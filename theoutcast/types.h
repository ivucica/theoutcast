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
    char graphics[50];
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
    unsigned short otid;
} item_t;

// then some enums
typedef enum {
    NONE = 0,
    CHARLIST = 1,
    GAMEWORLD = 2
} connectiontype_t;




#endif
