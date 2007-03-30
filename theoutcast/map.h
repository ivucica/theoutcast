#ifndef __MAP_H_OUTCAST // it's pretty much possible that some other header file is named map.h and uses just this syntax to guard :/
#define __MAP_H_OUTCAST

// May the map be with you.
// Always.
//           -- J. U. U. Lucas ;)

#include <map> // stl map is not Map. map != Map :D
//#include <ext/hash_map> // ok, maybe hashmap is smarter?

#include "tile.h"
#include "types.h"
#include "creature.h"


typedef std::map<unsigned long long, Tile*> maptype_t;
typedef std::map<unsigned long, Creature*> creaturelist_t;

/*
typedef __gnu_cxx::hash_map<unsigned long long, Tile*> maptype_t;
typedef __gnu_cxx::hash_map<unsigned long, Creature*> creaturelist_t;
*/
class Map {
    public:
        Map();
        ~Map();

        Tile* GetTile(position_t *pos);
        Creature* GetCreature(unsigned long creatureid, Creature *cr);

        void Lock();
        void Unlock();

        unsigned long SetAttackedCreature(unsigned long creatureid);
    private:
        maptype_t m;
        creaturelist_t c;
        ONCriticalSection threadsafe;
        Creature *attackedcreature;
};

extern Map gamemap;

#endif // __MAP_H
