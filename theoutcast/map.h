#ifndef __MAP_H_OUTCAST // it's pretty much possible that some other header file is named map.h and uses just this syntax to guard :/
#define __MAP_H_OUTCAST

// May the map be with you.
// Always.
//           -- J. U. U. Lucas ;)

#include <map> // stl map is not Map. map != Map :D

#include "tile.h"
#include "types.h"
typedef std::map<unsigned long long, Tile*> maptype_t;
class Map {
    public:
        Map();
        ~Map();

        Tile* GetTile(position_t *pos);
        void Lock();
        void Unlock();
    private:
        maptype_t m;
        ONCriticalSection threadsafe;
};

extern Map gamemap;

#endif // __MAP_H
