#ifndef __TILE_H
#define __TILE_H

#include <vector>
#include "thing.h"
#include "types.h"
#include "threads.h"
#include "creature.h"
#include "item.h"
class Tile {
    public:
        Tile();
        ~Tile();

        void insert(Thing *obj);
        //void remove(Thing *obj);
        void remove(unsigned char stackpos);

        void replace(Thing *original, Thing *newobject);
        void replace(unsigned char stackpos, Thing *newobject);

        void empty ();

        Thing *getstackpos(unsigned char stackpos);
        Thing *getground() {return ground;} // TEMPORARY F. that is TO BE REMOVED.

        void setpos(position_t *p); // so tile can know its position
        void render();
        void rendercreatures();

        unsigned int getitemcount();


    private:
        std::vector<Item*> itemlayers[4];
        std::vector<Creature*> creatures;
        Thing *ground;
        position_t pos;
        ONCriticalSection threadsafe;
        unsigned int itemcount;
};

#endif
