#include "assert.h"
#include "tile.h"
#include "thing.h"
#include "player.h"
#include "debugprint.h"
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
unsigned int Tile::getitemcount() {
    return this->itemcount;
}
void Tile::insert(Thing *thing) {
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
void Tile::remove(unsigned char pos) {
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
Thing *Tile::getstackpos(unsigned char pos) {
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
void Tile::setpos(position_t *p) {
    ONThreadSafe(threadsafe);
    pos.x = p->x;
    pos.y = p->y;
    pos.z = p->z;
    ONThreadUnsafe(threadsafe);
}
void Tile::render() {


    ONThreadSafe(threadsafe);


    if (!itemcount) {
        ONThreadUnsafe(threadsafe);
        return;
    }

    if (ground) {
        ground->AnimationAdvance(25./fps);
        ground->Render(&pos);
    }
    for (int i = 0; i <= 3; i++) {
        for (std::vector<Item*>::reverse_iterator it = itemlayers[3-i].rbegin(); it != itemlayers[3-i].rend(); it++) {
            (*it)->Render(&pos);
            (*it)->AnimationAdvance(25./fps);
        }
    }




    ONThreadUnsafe(threadsafe);


}
void Tile::rendercreatures() {

    ONThreadSafe(threadsafe);


    if (!itemcount) {
        ONThreadUnsafe(threadsafe);
        return;
    }

    unsigned int grndspeed = 500;// a safe default ...
    if (ground) {
        grndspeed = ground->GetSpeedIndex();
        if (!grndspeed) grndspeed = 500; // a safe fallback, once again
    }

    if (pos.x < player->GetPosX() + 8
     && pos.x > player->GetPosX() - 8
     && pos.y < player->GetPosY() + 6
     && pos.y > player->GetPosY() - 6)
        for (std::vector<Creature*>::iterator it = creatures.begin(); it != creatures.end(); it++) {
            (*it)->Render(&pos);
            if ((*it)->IsMoving()) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                (*it)->AnimationAdvance((((float)grndspeed*1000.) / ((*it)->GetSpeed() ? (*it)->GetSpeed() : 220))/fps);
        }


    ONThreadUnsafe(threadsafe);

}
void Tile::empty () {
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

