#include "assert.h"
#include "tile.h"
#include "thing.h"
#include "player.h"
#include "debugprint.h"
#include "items.h"
extern float fps;
Tile::Tile() {
    //printf("Forging a tile\n");
    ground = NULL;
    this->itemcount = 0;
    ONInitThreadSafe(threadsafe);
}
Tile::~Tile() {
    ONDeinitThreadSafe(threadsafe);
}
unsigned int Tile::GetItemCount() {
    return this->itemcount;
}
void Tile::Insert(Thing *thing) {
    ONThreadSafe(threadsafe);
    ASSERT(thing)
    if (!thing) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_ERROR, "Tile::insert - thing provided is NULL!!!!");
        //system("pause");
        ONThreadUnsafe(threadsafe);
        return;
    }

    if (thing->IsGround()) {
        if (!ground) this->itemcount ++;
        ground = thing;
    } else if (dynamic_cast<Creature*>(thing)) {
        creatures.insert(creatures.begin(), (Creature*)thing);
        this->itemcount ++;
    } else {
        itemlayers[thing->GetTopIndex()].insert(itemlayers[thing->GetTopIndex()].begin(), (Item*)thing);
        this->itemcount ++;
    }

    ONThreadUnsafe(threadsafe);
}
void Tile::Remove(unsigned char pos) {
    ONThreadSafe(threadsafe);

    ASSERT(pos < itemcount)

    // try to recover
    if (pos >= itemcount) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_ERROR, "Tile::remove pos - thing provided doesnt exist - removing %d, got %d!!!!", pos, itemcount);
        ONThreadUnsafe(threadsafe);
        return;
    }

    itemcount --;
    if (ground) {
        if (pos==0) {
            ground=NULL;

            ONThreadUnsafe(threadsafe);
            return;
        }
        pos--;
    }

    for (int i = 3; i >=1 ; i-- ) {

        if (pos < itemlayers[i].size()) {
            std::vector<Item*>::iterator it=itemlayers[i].begin();
            it += pos;
            itemlayers[i].erase(it);
            ONThreadUnsafe(threadsafe);
            return;
        }
        pos -= itemlayers[i].size();
    }
    if (pos < creatures.size()) {
        std::vector<Creature*>::iterator it=creatures.begin();
        it += pos;
        creatures.erase(it);
        ONThreadUnsafe(threadsafe);
        return;
    }
    pos -= creatures.size();

    if (pos < itemlayers[0].size()) {
        std::vector<Item*>::iterator it=itemlayers[0].begin();
        it += pos;
        itemlayers[0].erase(it);

    ONThreadUnsafe(threadsafe);
        return;
    }
    pos -= itemlayers[0].size();
    printf("Tile::remove(unsigned char pos): FAILED\n");


    itemcount ++;
    ONThreadUnsafe(threadsafe);
}
Thing *Tile::GetStackPos(unsigned char pos) {
    ONThreadSafe(threadsafe);

    if (ground) {
        if (pos==0) {
            ONThreadUnsafe(threadsafe);
            return ground;
        }
        pos--;
    }

    for (int i = 3; i >=1 ; i-- ) {

        if (pos < itemlayers[i].size()) {
            ONThreadUnsafe(threadsafe);
            return itemlayers[i][pos];
        }
        pos -= itemlayers[i].size();
    }
    if (pos < creatures.size()) {
        ONThreadUnsafe(threadsafe);
        return creatures[pos];
    }
    pos -= creatures.size();

    if (pos < itemlayers[0].size()) {
        ONThreadUnsafe(threadsafe);
        return itemlayers[0][pos];
    }
    pos -= itemlayers[0].size();
    printf("Tile::getstackpos(unsigned char pos): FAILED\n");

    ONThreadUnsafe(threadsafe);
    return NULL;
}
void Tile::SetPos(position_t *p) {
    ONThreadSafe(threadsafe);
    pos.x = p->x;
    pos.y = p->y;
    pos.z = p->z;
    ONThreadUnsafe(threadsafe);
}
void Tile::Render(int layer) {

    if (!itemcount) {
        //ONThreadUnsafe(threadsafe);
        return;
    }

    ONThreadSafe(threadsafe);



    static std::vector<Item*>::reverse_iterator it;
    //for (layer = 0; layer <= 2; layer++)
    switch (layer) {
        case 0: // before creatures
            if (ground) {
                ground->AnimationAdvance(25./fps);
                ground->Render(&pos);
            }
            for (it = itemlayers[4].rbegin(); it != itemlayers[4].rend(); it++) {
                (*it)->Render(&pos);
                (*it)->AnimationAdvance(25./fps);
            }


            for (it = itemlayers[1].rbegin(); it != itemlayers[1].rend(); it++) {
                (*it)->Render(&pos);
                (*it)->AnimationAdvance(25./fps);
            }

            for (it = itemlayers[2].rbegin(); it != itemlayers[2].rend(); it++) if (items[(*it)->GetType()].splash) {
                (*it)->Render(&pos);
                (*it)->AnimationAdvance(25./fps);
            }

            for (it = itemlayers[0].rbegin(); it != itemlayers[0].rend(); it++) {
                (*it)->Render(&pos);
                (*it)->AnimationAdvance(25./fps);
            }
            break;

        case 1: { // creatures


            static unsigned int grndspeed = 500;// a safe default ...
            static unsigned int creaturespeed = 220; //  a safe default...
            if (ground) {
                grndspeed = ground->GetSpeedIndex();
                if (!grndspeed) grndspeed = 500; // a safe fallback, once again
            }

            if (pos.x < player->pos.x + 8
             && pos.x > player->pos.x - 8
             && pos.y < player->pos.y + 6
             && pos.y > player->pos.y - 6
             && creatures.size())
                for (std::vector<Creature*>::iterator it = creatures.begin(); it != creatures.end(); it++) {
                    creaturespeed = (*it)->GetSpeed();
                    creaturespeed = (creaturespeed ? creaturespeed : 220);



                    (*it)->Render(&pos);
                    if ((*it)->IsMoving()) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                        (*it)->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
                }
            break;
        }
        case 2: // after creatures

            for (it = itemlayers[2].rbegin(); it != itemlayers[2].rend(); it++) if (!items[(*it)->GetType()].splash) {
                (*it)->Render(&pos);
                (*it)->AnimationAdvance(25./fps);
            }

            for (it = itemlayers[3].rbegin(); it != itemlayers[3].rend(); it++) {
                (*it)->Render(&pos);
                (*it)->AnimationAdvance(25./fps);
            }

            break;
        case 3: // creature overlay
            for (std::vector<Creature*>::iterator it = creatures.begin(); it != creatures.end(); it++) {
                (*it)->RenderOverlay();
            }
    }


    ONThreadUnsafe(threadsafe);


}

void Tile::Empty () {
    ONThreadSafe(threadsafe);
    if (ground) {
        delete ground;
        ground = NULL;
    }

    for (int i = 3; i >= 0; i--) {
        for (std::vector<Item*>::iterator it = itemlayers[i].begin(); it != itemlayers[i].end(); ) {
            if (*it)
                delete (*it);
            else
                printf("@(@@@)@)@)(@)(@@( OMFG There's a NULL item on a tile!!\n");
            itemlayers[i].erase(it);
        }
    }

    for (std::vector<Creature*>::iterator it = creatures.begin(); it != creatures.end(); ) {
        // DONT DO delete x; ON CREATURES!!!
        // They need to be cleaned (freed) only on logoff and similar actions
        // That's because they're kept in std::map<>
//        if (*it)
//            delete (*it);
//        else
            printf("@(@@@)@)@)(@)(@@( OMFG There's a NULL creature on a tile!!\n");
        creatures.erase(it);
    }
    this->itemcount = 0;
    ONThreadUnsafe(threadsafe);
}

Creature *Tile::GetCreature() {
    if (creatures.size()) return *(creatures.begin()); else return NULL;
}

void Tile::ShowContents() {
    DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Contents of tile %d %d %d:\n", pos.x, pos.y, pos.z);
    if (ground) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Ground: %d\n", ground->GetType());
    }
    for (int i = 0; i <= 3 ; i++) {
        for (std::vector<Item*>::iterator it = itemlayers[i].begin(); it != itemlayers[i].end(); it++) {
            DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Layer %d: %d\n", i, (*it)->GetType());
        }
    }

}
