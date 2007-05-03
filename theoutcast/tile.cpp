#include "assert.h"
#include "tile.h"
#include "thing.h"
#include "player.h"
#include "debugprint.h"
#include "items.h"
#include "database.h"
#include "protocol.h"
#include "options.h"
#include "map.h"
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
            //delete ground; // we're allowed to delete ONLY if we're not >>moving<<
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
            //delete *it;
            itemlayers[i].erase(it);
            ONThreadUnsafe(threadsafe);
            return;
        }
        pos -= itemlayers[i].size();
    }
    if (pos < creatures.size()) {
        std::vector<Creature*>::iterator it=creatures.begin();
        it += pos;
        //delete *it;
        creatures.erase(it);
        ONThreadUnsafe(threadsafe);
        return;
    }
    pos -= creatures.size();
    if (pos < itemlayers[0].size()) {
        std::vector<Item*>::iterator it=itemlayers[0].begin();
        it += pos;
        //delete *it;
        itemlayers[0].erase(it);

    ONThreadUnsafe(threadsafe);
        return;
    }
    pos -= itemlayers[0].size();
    printf("Tile::remove(unsigned char pos): FAILED\n");
    itemcount ++;
    ONThreadUnsafe(threadsafe);
}


void Tile::Remove(Thing *obj) {
    ONThreadSafe(threadsafe);

    std::vector<Item*>::iterator it;
    std::vector<Creature*>::iterator ct;

    itemcount --;
    if (ground) {
        if (ground == obj) {
//            delete ground;
            ground=NULL;

            ONThreadUnsafe(threadsafe);
            return;
        }
    }
    for (int i = 3; i >=0 ; i-- ) {
        for (it=itemlayers[i].begin(); it != itemlayers[i].end(); it++) {
            if (*it == obj) {
//                delete *it;
                itemlayers[i].erase(it);
                ONThreadUnsafe(threadsafe);
                return;
            }
        }
    }
    for (ct=creatures.begin(); ct != creatures.end(); ct++) {
        if (*ct == obj) {
//            delete *ct;
            creatures.erase(ct);
            ONThreadUnsafe(threadsafe);
            return;
        }
    }

    printf("Tile::remove(Thing *obj): FAILED\n");
    itemcount++;
    ONThreadUnsafe(threadsafe);
}




Thing *Tile::GetStackPos(unsigned char pos) {
    ONThreadSafe(threadsafe);
    char initialpos = pos; // just for debugggin purposes, remove me
    if (ground) {
        if (pos==0) {
            ONThreadUnsafe(threadsafe);
            return ground;
        }
        pos--;
    }

    for (int i = 1; i <=3 ; i++ ) {

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
    printf("Tile::getstackpos(unsigned char pos): FAILED (argument: %d, total size: %d, remaining: %d)\n", initialpos, itemcount, pos );

    ONThreadUnsafe(threadsafe);
    return NULL;
}


void Tile::Replace(unsigned char pos, Thing* newthing) {
    ONThreadSafe(threadsafe);

    //printf("Replacing %d with %d\n", GetStackPos
    if (ground) {
        if (pos==0) {
            ground = newthing;
            ONThreadUnsafe(threadsafe);
            return;
        }
        pos--;
    }

    for (int i = 1; i <=3 ; i++ ) {

        if (pos < itemlayers[i].size()) {
            itemlayers[i][pos] = (Item*)newthing;
            ONThreadUnsafe(threadsafe);
            return;
        }
        pos -= itemlayers[i].size();
    }
    if (pos < creatures.size()) {
        creatures[pos] = (Creature*)newthing;
        ONThreadUnsafe(threadsafe);
        return;
    }
    pos -= creatures.size();

    if (pos < itemlayers[0].size()) {
        itemlayers[0][pos] = (Item*)newthing;
        ONThreadUnsafe(threadsafe);
        return;
    }
    pos -= itemlayers[0].size();
    printf("Tile::replace(): FAILED\n");

    ONThreadUnsafe(threadsafe);
    return;
}




void Tile::SetPos(position_t *p) {
    ONThreadSafe(threadsafe);
    pos.x = p->x;
    pos.y = p->y;
    pos.z = p->z;
    ONThreadUnsafe(threadsafe);
}
void Tile::RenderStrayCreatures(position_t *p) {

    static unsigned int grndspeed = 500;// a safe default ...
    static unsigned int creaturespeed = 220; //  a safe default...
    if (ground) {
        grndspeed = ground->GetSpeedIndex();
        if (!grndspeed) grndspeed = 500; // a safe fallback, once again
    }
    for (std::vector<Creature*>::iterator it = this->creatures.begin(); it != this->creatures.end(); it++) {
        Creature *cr = (*it);
        if (!cr) {
            DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_WARNING, "A null item encountered while trying to render stray creatures");
            continue;
        } else {
            creaturespeed = cr->GetSpeed();
            creaturespeed = (creaturespeed ? creaturespeed : 220);

            if (cr->IsMoving()  &&
                ((p->x > pos.x && p->y == pos.y && cr->GetDirection() == WEST) ||
                 (p->y > pos.y && p->x == pos.x && cr->GetDirection() == NORTH))
            ) {
                //glColor4f(1., 0, 0, 1.);
                glTranslatef(-(p->x - pos.x) * 32, (p->y - pos.y) * 32, 0);
                cr->Render(p);
                glTranslatef((p->x - pos.x) * 32, -(p->y - pos.y) * 32, 0);
                //glColor4f(1.,1.,1.,1.);
            }
            if (cr->IsMoving() && pos.z != player->pos.z) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
        }
    }

}
void Tile::Render(int layer) {

    if (!itemcount) {
        //ONThreadUnsafe(threadsafe);
        return;
    }



    if (layer == 0) {

        ONThreadSafe(threadsafe);

        static unsigned int grndspeed = 500;// a safe default ...
        static unsigned int creaturespeed = 220; //  a safe default...
        if (ground) {
            grndspeed = ground->GetSpeedIndex();
            if (!grndspeed) grndspeed = 500; // a safe fallback, once again
        }

        if (ground) {
            Thing *th = (ground);

            if (Creature * cr = dynamic_cast<Creature*>(th)) {
                creaturespeed = cr->GetSpeed();
                creaturespeed = (creaturespeed ? creaturespeed : 220);

                if (!cr->IsMoving() || cr->IsApproved() || cr->GetDirection() == SOUTH || cr->GetDirection() == EAST)
                    cr->Render(&pos);
                if (cr->IsMoving() && pos.z != player->pos.z) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                    cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
            } else {
                th->AnimationAdvance(25./fps);
                th->Render(&pos);
            }
        }



        for (std::vector<Item*>::iterator it = this->itemlayers[2].begin(); it != this->itemlayers[2].end(); it++) if (items[(*it)->GetType()]->splash) {
            Thing *th = (*it);

            if (Creature * cr = dynamic_cast<Creature*>(th)) {
                creaturespeed = cr->GetSpeed();
                creaturespeed = (creaturespeed ? creaturespeed : 220);

                if (!cr->IsMoving() || cr->IsApproved() || cr->GetDirection() == SOUTH || cr->GetDirection() == EAST)
                    cr->Render(&pos);
                if (cr->IsMoving() && pos.z != player->pos.z) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                    cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
            } else {
                th->AnimationAdvance(25./fps);
                th->Render(&pos);
            }
        }





        for (std::vector<Item*>::iterator it = this->itemlayers[0].begin(); it != this->itemlayers[0].end(); it++) {
            Thing *th = (*it);

            if (Creature * cr = dynamic_cast<Creature*>(th)) {
                creaturespeed = cr->GetSpeed();
                creaturespeed = (creaturespeed ? creaturespeed : 220);

                if (!cr->IsMoving() || !cr->IsApproved())
                    cr->Render(&pos);
                if (cr->IsMoving() && pos.z != player->pos.z) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                    cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
            } else {
                th->AnimationAdvance(25./fps);
                th->Render(&pos);
            }
        }

        for (std::vector<Item*>::iterator it = this->itemlayers[1].begin(); it != this->itemlayers[1].end(); it++) {
            Thing *th = (*it);

            if (Creature * cr = dynamic_cast<Creature*>(th)) {
                creaturespeed = cr->GetSpeed();
                creaturespeed = (creaturespeed ? creaturespeed : 220);

                if (!cr->IsMoving() || cr->IsApproved() || cr->GetDirection() == SOUTH || cr->GetDirection() == EAST)
                    cr->Render(&pos);
                if (cr->IsMoving() && pos.z != player->pos.z) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                    cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
            } else {
                th->AnimationAdvance(25./fps);
                th->Render(&pos);
            }
        }



        for (std::vector<Item*>::iterator it = this->itemlayers[2].begin(); it != this->itemlayers[2].end(); it++) if (!items[(*it)->GetType()]->splash) {
            Thing *th = (*it);

            if (Creature * cr = dynamic_cast<Creature*>(th)) {
                creaturespeed = cr->GetSpeed();
                creaturespeed = (creaturespeed ? creaturespeed : 220);

                if (!cr->IsMoving() || cr->IsApproved() || cr->GetDirection() == SOUTH || cr->GetDirection() == EAST)
                    cr->Render(&pos);
                if (cr->IsMoving() && pos.z != player->pos.z) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                    cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
            } else {
                th->AnimationAdvance(25./fps);
                th->Render(&pos);
            }
        }

        position_t p;
        p.z = pos.z;
        for (int i = -1; i <= 1; i++)
            for (int j = -1; j <= 1; j++) {
                p.x = pos.x + i; p.y = pos.y + j;
                Tile *neighbor = gamemap.GetTile(&p);
                neighbor->RenderStrayCreatures(&pos);
            }

        for (std::vector<Creature*>::iterator it = this->creatures.begin(); it != this->creatures.end(); it++) {
            Thing *th = (*it);

            if (Creature * cr = dynamic_cast<Creature*>(th)) {
                creaturespeed = cr->GetSpeed();
                creaturespeed = (creaturespeed ? creaturespeed : 220);

                if (!cr->IsMoving() || cr->IsApproved() || cr->GetDirection() == SOUTH || cr->GetDirection() == EAST)
                    cr->Render(&pos);
                if (cr->IsMoving() && pos.z != player->pos.z) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                    cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
            } else {
                th->AnimationAdvance(25./fps);
                th->Render(&pos);
            }
        }



        for (std::vector<Item*>::iterator it = this->itemlayers[3].begin(); it != this->itemlayers[3].end(); it++) {
            Thing *th = (*it);

            if (Creature * cr = dynamic_cast<Creature*>(th)) {
                creaturespeed = cr->GetSpeed();
                creaturespeed = (creaturespeed ? creaturespeed : 220);

                if (!cr->IsMoving() || cr->IsApproved() || cr->GetDirection() == SOUTH || cr->GetDirection() == EAST)
                    cr->Render(&pos);
                if (cr->IsMoving() && pos.z != player->pos.z) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                    cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
            } else {
                th->AnimationAdvance(25./fps);
                th->Render(&pos);
            }
        }








        ONThreadUnsafe(threadsafe);

    }
    if (layer==3) {// creature overlay
        ONThreadSafe(threadsafe);
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

                (*it)->RenderOverlay();
                if ((*it)->IsMoving() && pos.z == player->pos.z) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                    (*it)->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
        }
        ONThreadUnsafe(threadsafe);
    }





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
//            printf("@(@@@)@)@)(@)(@@( OMFG There's a NULL creature on a tile!!\n");
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
void Tile::StoreToDatabase() {
    static std::string *serv;
    static unsigned short *port;
    static int stackpos;
    static std::vector<Item*>::iterator it;
    static int i;
    if (!options.maptrack) return;
    ONThreadSafe(threadsafe);
    stackpos = 0;
    if (protocol->CipSoft()) {
        serv = &(protocol->charlistserver);
        port = &(protocol->charlistport);
    } else {
        serv = &(protocol->gameworldserver);
        port = &(protocol->gameworldport);
    }
    dbExecPrintf(dbUser, NULL, NULL, NULL, "delete from map where server='%s' and port='%d' and x=%d and y=%d and z=%d;",
                                                                  serv->c_str(),  *port,      pos.x,  pos.y,     pos.z);



    if (ground) {
        dbExecPrintf(dbUser, NULL, NULL, NULL, "insert into map (server, port, x, y, z, stackpos, itemid) values ('%s', %d, %d, %d, %d, %d, %d);",
                                                    serv->c_str(), *port, pos.x, pos.y, pos.z, stackpos, ground->GetType());
        stackpos ++;
    }

    for (i = 3; i >=0 ; i-- ) {
        for (it = itemlayers[i].begin(); it != itemlayers[i].end(); it++) {
            dbExecPrintf(dbUser, NULL, NULL, NULL, "insert into map (server, port, x, y, z, stackpos, itemid) values ('%s', %d, %d, %d, %d, %d, %d);",
                                                    serv->c_str(), *port, pos.x, pos.y, pos.z, stackpos, (*it)->GetType());
            stackpos ++;
        }
    }

    ONThreadUnsafe(threadsafe);

}
unsigned char Tile::GetTopLookAt() {
    static unsigned char stackpos;
    static unsigned char topusable;
    static int i;

    static std::vector<Item*>::iterator it;

    stackpos = 0;
    topusable = 255;
    if (ground) {

        if (items[ground->GetType()]->usable) {
            topusable = stackpos;

        }
        stackpos ++;
    }

    for (i = 1; i <= 3 ; i++ ) {
        for (it = itemlayers[i].begin(); it != itemlayers[i].end(); it++) {
            if (items[(*it)->GetType()]->usable) {
                topusable = stackpos;
            }
            stackpos ++;
        }
    }


    if (creatures.size())
        return stackpos + creatures.size() - 1;

    for (it = itemlayers[0].begin(); it != itemlayers[0].end(); it++) {
        if (items[(*it)->GetType()]->usable) {
            topusable = stackpos;
        }
        stackpos ++;
    }

    if (topusable == 255) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_WARNING, "Nothing usable there, defaulting.");
        topusable = stackpos-1;
    }


    return topusable;
}
#include "console.h"
unsigned char Tile::GetTopUsableStackpos() {
    static unsigned char stackpos;
    static unsigned char topusable;
    static int i;

    static std::vector<Item*>::iterator it;



    if (ground) {

        if (items[ground->GetType()]->extraproperty==78) {
 //           console.insert("Using on ground, extra property is 78", CONRED);
            return 0; // use the ground
        } else {
 //           char tmp[256];
//            sprintf(tmp, "Ground's extra property %d", items[ground->GetType()].extraproperty);
//            console.insert(tmp, CONRED);
        }
    }

    return GetTopLookAt();



    stackpos = 0;
    topusable = 255;
    if (ground) {

        if (items[ground->GetType()]->usable) {
            topusable = stackpos;

        }
        stackpos ++;
    }

    for (i = 1; i <=3 ; i++ ) {
        for (it = itemlayers[i].begin(); it != itemlayers[i].end(); it++) {
            if (items[(*it)->GetType()]->usable) {
                topusable = stackpos;
            }
            stackpos ++;
        }
    }

    if (!itemlayers[0].size()) {
        if (topusable == 255) {
            DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_WARNING, "Nothing usable there, defaulting.");
            topusable = stackpos-1;
        }

        return topusable;
    }

    stackpos += creatures.size();

    for (it = itemlayers[0].begin(); it != itemlayers[0].end(); it++) {
        if (items[(*it)->GetType()]->usable) {
            topusable = stackpos;
        }
        stackpos ++;
    }

    if (topusable == 255) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_WARNING, "Nothing usable there, defaulting.");
        topusable = stackpos-1;
    }

    return topusable;
}
