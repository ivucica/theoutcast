#include "debugprint.h"
#include "map.h"
#include "player.h"
#include "protocol.h"

#define MINIMAPW 32
#define MINIMAPH 32

Map gamemap;

Map::Map() {
    ONInitThreadSafe(threadsafe);
    attackedcreature = NULL;
    minimaptex = 0;

}
Map::~Map() {
    ONDeinitThreadSafe(threadsafe);
	glDeleteTextures(1, &minimaptex);
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


void Map::Clear() {
	for (maptype_t::iterator it = m.begin(); it != m.end(); it=m.begin()) {
    	if (it->second) {
            delete it->second;
            m.erase(it);
        } else {
            it++;
        }
    }

}
#include "imgfmts.h"
#ifdef WIN32
	#include <windows.h>
#endif
#include <GL/gl.h>
#include <sstream>
#include <iomanip>
// FIXME (Khaos#1#) Perhaps this should move into imgfmts?
unsigned int Map::GetMinimapTexture() {
	return minimaptex;
}
void Map::RebuildMinimap() {
	RGBA ar [MINIMAPW*MINIMAPH];
	unsigned char c;
	for (int i = 0; i<MINIMAPW*MINIMAPH; i++) {
		ar[i].r = 0;
		ar[i].g = 0;
		ar[i].b = 0;
		ar[i].a = 255;
	}


    std::stringstream x, y, z, minimapfn;
    x << setw(3) << setfill('0') << player->GetPosX() / 256;
    y << setw(3) << setfill('0') << player->GetPosY() / 256;
    z << setw(2) << setfill('0') << (int)player->GetPosZ();
    minimapfn << x.str() << y.str() << z.str() << ".map";

	FILE *f = fopen(minimapfn.str().c_str(), "rb");
	if (!f) return;
	for (int i = 0; i < MINIMAPW; i++)
		for (int j = 0; j < MINIMAPH; j++) {
			int fx, fy; // file x, file y
			int ox, oy; // original x, original y
			fx = (player->GetPosX() + i - MINIMAPW/2) % 256;
			fy = (player->GetPosY() + j - MINIMAPH/2) % 256;

			// FIXME (Khaos#1#) This way we force the 'unoptimized method'. Remove two lines below, and implement it smarter.
			fx = -1; fy = -1;

			if (fx >= 0 && fx < 256 && fy >= 0 && fy < 256) {
				fseek(f, fy*256 + fx, SEEK_SET);
				fread(&c, 1, 1, f);
				ar[j*MINIMAPW+i].b = (c % 6) / 5. * 255;
                ar[j*MINIMAPW+i].g = ((c / 6) % 6) / 5. * 255;
                ar[j*MINIMAPW+i].r = (c / 36) / 5. * 255;

			}  else {


				ox = player->GetPosX() + i - MINIMAPW/2;
				oy = player->GetPosY() + j - MINIMAPH/2;
				x.str("");
				y.str("");
				z.str("");

				x << setw(3) << setfill('0') << ox / 256;
				y << setw(3) << setfill('0') << oy / 256;
				z << setw(2) << setfill('0') << (int)player->GetPosZ();

				minimapfn.str("");
				minimapfn << x.str() << y.str() << z.str() << ".map";


				FILE *f2 = fopen(minimapfn.str().c_str(), "rb");
				if (f2) {

					int fx = ox % 256;
					int fy = oy % 256;

					fseek(f2, fy*256 + fx, SEEK_SET);
					fread(&c, 1, 1, f2);
					ar[j*MINIMAPW+i].b = (c % 6) / 5. * 255;
					ar[j*MINIMAPW+i].g = ((c / 6) % 6) / 5. * 255;
					ar[j*MINIMAPW+i].r = (c / 36) / 5. * 255;
					fclose(f2);

					//ar[j*32+i].b = 255;
				}
			}


		}
	fclose(f);


	ar[MINIMAPH/2*MINIMAPH+MINIMAPH/2].r  = 255; ar[MINIMAPH/2*MINIMAPH+MINIMAPH/2].g  = 255; ar[MINIMAPH/2*MINIMAPH+MINIMAPH/2].b  = 255; ar[MINIMAPH/2*MINIMAPH+MINIMAPH/2].a  = 255;

	if (minimaptex)
		glDeleteTextures(1, &minimaptex);
	glGenTextures(1, &minimaptex);

    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, minimaptex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, MINIMAPW, MINIMAPH, GL_RGBA, GL_UNSIGNED_BYTE, ar );

}
