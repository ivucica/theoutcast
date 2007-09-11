#ifndef __TILE_H
#define __TILE_H

#include <vector>
#include "thing.h"
#include "types.h"
#include "threads.h"
#include "creature.h"
#include "item.h"
#include "effect.h"
class Tile {
    public:
        Tile();
        ~Tile();

        void Insert(Thing *obj, bool begin);
        void Insert(Item *obj, bool begin);
        void Insert(Creature *obj, bool begin);
        void Insert(Effect *obj, bool begin);
        //void remove(Thing *obj);
        void Remove(unsigned char stackpos) {Remove(stackpos, false);}
        void Remove(Thing *obj) {Remove(obj, false);}
        void Remove(unsigned char stackpos, bool moving);
        void Remove(Thing *obj, bool moving);

        void Replace(Thing *original, Thing *newobject);
        void Replace(unsigned char stackpos, Thing *newobject);

        void Empty ();

        Thing *GetStackPos(unsigned char stackpos);
        Thing *GetGround() {return ground;} // TEMPORARY F. that is TO BE REMOVED.
        Creature *GetCreature();

        unsigned char GetTopUsableStackpos();
        unsigned char GetTopLookAt();
        unsigned char GetTopCreatureStackpos() {
        	return (ground ? 1 : 0) + itemlayers[3].size() + itemlayers[2].size() + itemlayers[1].size() + creatures.size() - 1;
        }


        void SetPos(const position_t *p); // so tile can know its position
        void Render(int layer);
        void RenderStrayCreatures(const position_t *p);

        unsigned int GetItemCount();

        void ShowContents();
        void StoreToDatabase();
	void StoreToMinimap();

	void DelayedRemove(Thing* t);
	void CommitDelayedRemove();
    private:



        std::vector<Item*> itemlayers[4];
        std::vector<Creature*> creatures;
        std::vector<Effect*> effects;

        std::vector<Thing*> delayedremove;
        Thing *ground;
        position_t pos;
        ONCriticalSection threadsafe;
        unsigned int itemcount;
        #ifdef INCLUDE_SP
        friend class ProtocolSP;
        #endif
};

#endif
