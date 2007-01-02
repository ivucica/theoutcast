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

// then some enums
typedef enum {
    NONE = 0,
    CHARLIST = 1,
    GAMEWORLD = 2
} connectiontype_t;


#endif
