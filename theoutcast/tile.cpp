#include "assert.h"
#include "tile.h"
#include "thing.h"

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
void Tile::insert(Thing *thing) {
    ONThreadSafe(threadsafe);
    ASSERT(thing)
    if (thing->IsGround()) {
        if (!ground) this->itemcount ++;
        ground = thing;
    } else if (dynamic_cast<Creature*>(thing)) {
        creatures.insert(creatures.begin(), (Creature*)thing);
    } else {
        itemlayers[thing->GetTopIndex()].insert(itemlayers[thing->GetTopIndex()].begin(), (Item*)thing);
        this->itemcount ++;
    }

    ONThreadUnsafe(threadsafe);
}
void Tile::remove(unsigned char stackpos) {
    ONThreadSafe(threadsafe);

    ONThreadUnsafe(threadsafe);
}
void Tile::setpos(position_t *p) {
    ONThreadSafe(threadsafe);
    pos.x = p->x;
    pos.y = p->y;
    pos.z = p->z;
    ONThreadUnsafe(threadsafe);
}
void Tile::render() {

    if (!itemcount) {
        return;
    }

    ONThreadSafe(threadsafe);

    if (ground) {
        //ground->AnimationAdvance(100. / fps);
        ground->Render(&pos);
    }
    for (int i = 2; i >= 0; i--) {
        for (std::vector<Item*>::iterator it = itemlayers[i].begin(); it != itemlayers[i].end(); it++) {
            (*it)->Render(&pos);
            //(*it)->AnimationAdvance(100. / fps);
        }
    }


    for (std::vector<Creature*>::iterator it = creatures.begin(); it != creatures.end(); it++) {
        (*it)->Render(&pos);
        //(*it)->AnimationAdvance(100. / fps);
    }

    ONThreadUnsafe(threadsafe);


}
void Tile::empty () {
    //printf("Tile::empty - locking\n");
    ONThreadSafe(threadsafe);
    if (ground) {
        delete ground;
        ground = NULL;
    }
    //printf("Tile::empty - ground deleted\n");
    for (int i = 0; i < 3; i++) {
        for (std::vector<Item*>::iterator it = itemlayers[i].begin(); it != itemlayers[i].end(); ) {
            if (*it)
                delete (*it);
            else
                printf("@(@@@)@)@)(@)(@@( OMFG There's a NULL item on a tile!!\n");
            itemlayers[i].erase(it);
        }
    }
    for (std::vector<Creature*>::iterator it = creatures.begin(); it != creatures.end(); ) {
        if (*it)
            delete (*it);
        else
            printf("@(@@@)@)@)(@)(@@( OMFG There's a NULL creature on a tile!!\n");
        creatures.erase(it);
    }
    //printf("Tile::empty - all items removed\n");
    this->itemcount = 0;
    //printf("Tile::empty - unlocking\n");
    ONThreadUnsafe(threadsafe);
    //printf("Tile::empty - unlocked\n");
}
