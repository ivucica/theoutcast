#include "map.h"
Map gamemap;
Map::Map() {
    ONInitThreadSafe(threadsafe);

}
Map::~Map() {
    ONDeinitThreadSafe(threadsafe);
//destroy every tile and remove it from the std::map
}

Tile* Map::GetTile(position_t *pos) {
    // check if this is well generated with, idk, %08x ? :D

    unsigned long long tileid = (unsigned long long )pos->z << 32 | (unsigned long long)pos->x << 16 | (unsigned long long )pos->y;
    //printf("position %d %d %d: %08x%d%08x\n", pos->x, pos->y, pos->z, tileid>>32,  tileid );
    //return NULL;

    maptype_t::iterator it = m.find( tileid );
    if (it==m.end()) {
        Tile* t = new Tile;
        m[tileid] = t;
        t->setpos(pos);
        return t;
    } else {
        //printf("Tile found\n");
        return it->second;
    }

}
void Map::Lock() {
    ONThreadSafe(threadsafe);
}
void Map::Unlock() {
    ONThreadUnsafe(threadsafe);
}
