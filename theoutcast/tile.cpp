
#include "tile.h"
#include "thing.h"
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
    if (thing->IsGround()) {
        if (!ground) this->itemcount ++;
        ground = thing;
    } else {
        itemlayers[thing->GetTopIndex()].insert(itemlayers[thing->GetTopIndex()].begin(), thing);
        this->itemcount ++;
    }

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

//    printf("Tile::render - locking %d %d %d\n", pos.x, pos.y, pos.z);
    ONThreadSafe(threadsafe);

    if (ground)
        ground->Render(&pos);
    //printf("Ground rendered, now painting other stuff:\n");
    for (int i = 2; i >= 0; i--) {
        //printf("%d ", i);
        for (std::vector<Thing*>::iterator it = itemlayers[i].begin(); it != itemlayers[i].end(); it++) {
            (*it)->Render(&pos);
        }
    }
    //printf("\n");
    //printf("Tile::render - unlocking %d %d %d\n", pos.x, pos.y, pos.z);
    ONThreadUnsafe(threadsafe);


}
void Tile::empty () {
    printf("Tile::empty - locking\n");
    ONThreadSafe(threadsafe);
    if (ground) {
        delete ground;
        ground = NULL;
    }
    printf("Tile::empty - ground deleted\n");
    for (int i = 0; i < 3; i++) {
        for (std::vector<Thing*>::iterator it = itemlayers[i].begin(); it != itemlayers[i].end(); ) {
            if (*it)
                delete (*it);
            else
                printf("@(@@@)@)@)(@)(@@( OMFG There's a NULL item on a tile!!\n");
            itemlayers[i].erase(it);
        }
    }
    printf("Tile::empty - all items removed\n");
    this->itemcount = 0;
    printf("Tile::empty - unlocking\n");
    ONThreadUnsafe(threadsafe);
    printf("Tile::empty - unlocked\n");
}
