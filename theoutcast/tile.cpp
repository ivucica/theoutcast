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
void Tile::insert(Thing *thing) {
    ONThreadSafe(threadsafe);
    ASSERT(thing)
    if (!thing) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_ERROR, "Tile::insert - thing provided is NULL!!!!");
        system("pause");
        ONThreadUnsafe(threadsafe);
        return;
    }

    if (thing->IsGround()) {
        printf("Inserting ground to %d %d %d\n", pos.x, pos.y, pos.z );
        if (!ground) this->itemcount ++;
        ground = thing;
    } else if (dynamic_cast<Creature*>(thing)) {
        printf("Inserting a creature to %d %d %d.\n", pos.x, pos.y, pos.z);
        creatures.insert(creatures.begin(), (Creature*)thing);
        this->itemcount ++;
    } else {
        printf("Inserting item %d to %d %d %d to layer %d.\n", thing->GetType(), pos.x, pos.y, pos.z, thing->GetTopIndex());
        itemlayers[thing->GetTopIndex()].insert(itemlayers[thing->GetTopIndex()].begin(), (Item*)thing);
        this->itemcount ++;
    }

    ONThreadUnsafe(threadsafe);
}
void Tile::remove(unsigned char pos) {
    ONThreadSafe(threadsafe);

    printf("REMOVING %d\n", (int)pos);
    printf("Removing from tile %d %d %d\n", this->pos.x, this->pos.y, this->pos.z);
    ASSERT(pos < itemcount)

    // try to recover
    if (pos >= itemcount) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_ERROR, "Tile::remove pos - thing provided doesnt exist - removing %d, got %d!!!!", pos, itemcount);
        //system("pause");
        ONThreadUnsafe(threadsafe);
        return;
    }

    itemcount --;
    if (ground) {
        printf("Ground exists; are we desiring it?\n");
        if (pos==0) {
            ground=NULL;

            ONThreadUnsafe(threadsafe);
            return;
        }
        pos--;
    }
    printf("passed ground; position is %d\n", pos);

    for (int i = 3; i >=1 ; i-- ) {
        printf("now in layer %d with %d items\n", i, itemlayers[i].size());

        printf("position %d\n", pos);

        if (pos < itemlayers[i].size()) {
            printf("Positive!\n");
            std::vector<Item*>::iterator it=itemlayers[i].begin();
            it += pos;
            itemlayers[i].erase(it);
            ONThreadUnsafe(threadsafe);
            return;
        }
        pos -= itemlayers[i].size();
        printf("passed layer %d; position is %d\n", i, pos);
    }
    printf("now creatures!!! - size %d\n", creatures.size());
    if (pos < creatures.size()) {
        printf("Its a creature!!\n");
        std::vector<Creature*>::iterator it=creatures.begin();
        it += pos;
        creatures.erase(it);
        ONThreadUnsafe(threadsafe);
        return;
    }
    pos -= creatures.size();

    printf("checking bottom items - size %d\n", itemlayers[0].size());
    if (pos < itemlayers[0].size()) {
        std::vector<Item*>::iterator it=itemlayers[0].begin();
        it += pos;
        itemlayers[0].erase(it);

    ONThreadUnsafe(threadsafe);
        return;
    }
    pos -= itemlayers[0].size();
    printf("FAILED\n");


    itemcount ++;
    ONThreadUnsafe(threadsafe);
}
Thing *Tile::getstackpos(unsigned char pos) {
    printf("GETTING STACKPOS %d\n", pos);

    if (ground) {
        printf("Ground exists; are we desiring it?\n");
        if (pos==0)
            return ground;
        pos--;
    }
    printf("passed ground; position is %d\n", pos);

    for (int i = 3; i >=1 ; i-- ) {
        printf("now in layer %d with %d items\n", i, itemlayers[i].size());

        printf("position %d\n", pos);

        if (pos < itemlayers[i].size()) {
            printf("Positive!\n");
            return itemlayers[i][pos];
        }
        pos -= itemlayers[i].size();
        printf("passed layer %d; position is %d\n", i, pos);
    }
    printf("now creatures!!!\n");
    if (pos < creatures.size()) {
        printf("Its a creature!!\n");
        return creatures[pos];
    }
    pos -= creatures.size();

    printf("checking bottom items\n");
    if (pos < itemlayers[0].size()) {
        printf("it's a bottom item!\n");
        return itemlayers[0][pos];
    }
    pos -= itemlayers[0].size();
    printf("FAILED\n");

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

    if (!itemcount) {
        return;
    }

    ONThreadSafe(threadsafe);


    if (ground) {
        ground->AnimationAdvance(25./fps);
        ground->Render(&pos);
    }
    for (int i = 0; i <= 3; i++) {
        for (std::vector<Item*>::reverse_iterator it = itemlayers[i].rbegin(); it != itemlayers[i].rend(); it++) {
            (*it)->Render(&pos);
            (*it)->AnimationAdvance(25./fps);
        }
    }


    if (pos.x < player->GetPosX() + 8
     && pos.x > player->GetPosX() - 8
     && pos.y < player->GetPosY() + 6
     && pos.y > player->GetPosY() - 6)
        for (std::vector<Creature*>::iterator it = creatures.begin(); it != creatures.end(); it++) {
            (*it)->Render(&pos);
            (*it)->AnimationAdvance(25./fps);
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
