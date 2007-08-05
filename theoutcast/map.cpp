#include "debugprint.h"
#include "map.h"
#include "player.h"
#include "protocol.h"
Map gamemap;

Map::Map() {
    ONInitThreadSafe(threadsafe);
    attackedcreature = NULL;

}
Map::~Map() {
    ONDeinitThreadSafe(threadsafe);
// FIXME destroy every tile and remove it from the std::map

    for (maptype_t::iterator it = m.begin(); it != m.end(); it=m.begin()) {
    	if (it->second) {
            delete it->second;
            m.erase(it);
        } else {
            it++;
        }
    }

}

Tile* Map::GetTile(const position_t *pos) {
    // check if this is well generated with, idk, %08x ? :D

    unsigned long long tileid;
    maptype_t::iterator it;

    tileid = (unsigned long long )pos->z << 32 | (unsigned long long)pos->x << 16 | (unsigned long long )pos->y;
    it = m.find( tileid );
    if (it==m.end()) {
        Tile* t = new Tile;
        m[tileid] = t;
        t->SetPos(pos);
        return t;
    } else {
        return it->second;
    }
}

Creature* Map::GetCreature(unsigned long creatureid, Creature *cr) {
    creaturelist_t::iterator it = c.find( creatureid );

    if (it==c.end()) {

        if (!cr) {
            DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY,  DEBUGPRINT_ERROR, "DIRECTED NOT TO FORM NEW CREATURE %d!!!! Arrr...!!\n", creatureid);
            return NULL;
        }
        c[creatureid] = cr;
        (cr)->SetCreatureID(creatureid);
        DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL,  DEBUGPRINT_NORMAL, "FORMING NEW CREATURE!!!!!!\n");
        return cr;
    } else {
    	//DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL,  DEBUGPRINT_NORMAL, "RETURNING OLD CREATURE!!!!!!\n");
        if (cr) {
        	printf("Map::GetCreature: Found old creature and there's new one supplied - Destroying new creature!!\n");
        	delete cr;
        }
        return it->second;
    }
}


void Map::Lock() {
    // it would appear that pthreads do not like same thread locked twice, even with two different mutexes!
    // someone should verify this
    //#ifdef WIN32
    ONThreadSafe(threadsafe);
    locked = true;
    //#endif
}
void Map::Unlock() {
    //#ifdef WIN32
    locked = false;
    ONThreadUnsafe(threadsafe);
    //#endif
}
unsigned long Map::SetAttackedCreature(unsigned long creatureid) {
    Creature *c = GetCreature(creatureid, NULL);
    if (c) {
        if (attackedcreature) attackedcreature->SetAttacked(false);
        if (attackedcreature == c) return (attackedcreature = NULL), 0;
        c->SetAttacked(true);
        attackedcreature = c;
        return creatureid;
    }
    return (attackedcreature = NULL), 0;

}

void Map::FreeUnused(unsigned short minx, unsigned short maxx, unsigned short miny, unsigned short maxy) {

	for (maptype_t::iterator it = m.begin(); it != m.end() ; it++) {
		if (((it->first >> 16) & 0xFFFF)<minx && ((it->first >> 16) & 0xFFFF) > maxx &&
		    (it->first & 0xFFFF) < miny &&  (it->first & 0xFFFF) > maxy) {
		    	delete it->second;
			it->second = NULL;
			m.erase(it);
			it--;
		    }
	}
}

bool Map::IsVisible(const position_t &pos) {
	// FIXME (Khaos#1#) This does not take into account if tile is on different Z level.
	bool visible = 	(pos.x > player->GetPosX() - protocol->maxx/2 ) && (pos.x < player->GetPosX() + protocol->maxx/2 + 1) &&
					(pos.y > player->GetPosY() - protocol->maxy/2 ) && (pos.y < player->GetPosY() + protocol->maxy/2 + 1);
	return visible;
}

