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

        void Insert(Thing *obj);
        //void remove(Thing *obj);
        void Remove(unsigned char stackpos);

        void Replace(Thing *original, Thing *newobject);
        void Replace(unsigned char stackpos, Thing *newobject);

        void Empty ();

        Thing *GetStackPos(unsigned char stackpos);
        Thing *GetGround() {return ground;} // TEMPORARY F. that is TO BE REMOVED.
        Creature *GetCreature();

        void SetPos(position_t *p); // so tile can know its position
        void Render(int layer);

        unsigned int GetItemCount();

        void ShowContents();
        void StoreToDatabase();
    private:
        std::vector<Item*> itemlayers[4];
        std::vector<Creature*> creatures;
        Thing *ground;
        position_t pos;
        ONCriticalSection threadsafe;
        unsigned int itemcount;
};

#endif
