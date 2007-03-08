#include "map.h"
Map gamemap;
Map::Map() {
    ONInitThreadSafe(threadsafe);

}
Map::~Map() {
    ONDeinitThreadSafe(threadsafe);
// FIXME destroy every tile and remove it from the std::map
}

Tile* Map::GetTile(position_t *pos) {
    // check if this is well generated with, idk, %08x ? :D

    unsigned long long tileid;
    maptype_t::iterator it;

    tileid = (unsigned long long )pos->z << 32 | (unsigned long long)pos->x << 16 | (unsigned long long )pos->y;
    it = m.find( tileid );
    if (it==m.end()) {
        Tile* t = new Tile;
        m[tileid] = t;
        t->setpos(pos);
        return t;
    } else {
        return it->second;
    }

}
Creature* Map::GetCreature(unsigned int creatureid, Creature *cr) {
    creaturelist_t::iterator it = c.find( creatureid );
    if (it==c.end()) {
        if (!cr) return NULL;
        c[creatureid] = cr;
        (cr)->SetCreatureID(creatureid);
        return cr;
    } else {
        delete cr;
        return it->second;
    }
}


void Map::Lock() {
    ONThreadSafe(threadsafe);
}
void Map::Unlock() {
    ONThreadUnsafe(threadsafe);
}
