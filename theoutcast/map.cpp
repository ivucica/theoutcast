#include "map.h"

Map::Map() {
}
Map::~Map() {
}

Tile* Map::GetTile(position_t *pos) {
    for (std::vector<Tile*>::iterator it = tiles.begin(); it != tiles.end(); it++) {
        if ((*it)->SamePosition(pos)) {
            return (*it);
        }
    }
}
