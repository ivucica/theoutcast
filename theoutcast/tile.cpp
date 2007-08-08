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
#include "console.h"
void Tile::Insert(Creature *thing, bool begin) {
    ONThreadSafe(threadsafe);
    ASSERTFRIENDLY(thing, "Creature provided is null!")
    if (!thing) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_ERROR, "Tile::insert - creature provided is NULL!!!!");
        ONThreadUnsafe(threadsafe);
        return;
    }

	creatures.insert(begin ? creatures.begin() : creatures.end(), thing);
	this->itemcount ++;
	ONThreadUnsafe(threadsafe);
}

void Tile::Insert(Item *thing, bool begin) {
    ONThreadSafe(threadsafe);
    ASSERTFRIENDLY(thing, "Item provided is null!")
    if (!thing) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_ERROR, "Tile::insert - item provided is NULL!!!!");
        ONThreadUnsafe(threadsafe);
        return;
    }

	if (thing->IsGround()) {
        if (!ground) this->itemcount ++;
        ground = thing;
    } else {
        itemlayers[thing->GetTopIndex()].insert(begin ? itemlayers[thing->GetTopIndex()].begin() : itemlayers[thing->GetTopIndex()].end(), (Item*)thing);
        this->itemcount ++;
    }
	ONThreadUnsafe(threadsafe);
}

void Tile::Insert(Effect *thing, bool begin) {
    ONThreadSafe(threadsafe);
    ASSERTFRIENDLY(thing, "Effect provided is null!")
    if (!thing) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_ERROR, "Tile::insert - effect provided is NULL!!!!");
        ONThreadUnsafe(threadsafe);
        return;
    }

	effects.insert(begin ? effects.begin() : effects.end(), (Effect*)thing);
	ONThreadUnsafe(threadsafe);
}


void Tile::Insert(Thing *thing, bool begin) {

	ASSERTFRIENDLY(thing, "Thing provided is null!")
	/*printf("---inserting %p---\n", thing);
	printf("---testing for creature---\n");*/

    if (dynamic_cast<Creature*>(thing)) {
    	//printf("+++ack+++\n");
		Insert(dynamic_cast<Creature*>(thing), begin);
		return;
    }
    //printf("---testing for effect---\n");
    if (dynamic_cast<Effect*>(thing)) {
//    	printf("+++ack+++\n");
    	Insert(dynamic_cast<Effect*>(thing), begin);
    	return;
    }
//	printf("---surely it's item?---\n");
	Insert(dynamic_cast<Item*>(thing), begin);
//	printf("+++ack+++\n");

}

void Tile::Remove(unsigned char pos, bool moving) {
    ONThreadSafe(threadsafe);

    ASSERT(pos < itemcount)
    // try to recover
    if (pos >= itemcount) {
        DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_ERROR, "Tile::remove pos - thing provided doesnt exist - removing %d, got %d!!!!", pos, itemcount);
        ONThreadUnsafe(threadsafe);
        return;
    }
    ASSERTFRIENDLY(TextureIntegrityTest(), "Tile::Remove(unsigned char pos): Integrity test failed");
    itemcount --;
    if (ground) {
        if (pos==0) {
            if (!moving) delete ground;
            ground=NULL;

            ASSERTFRIENDLY(TextureIntegrityTest(), "Tile::Remove(unsigned char pos): Integrity test failed");

            ONThreadUnsafe(threadsafe);
            return;
        }
        pos--;
    }
    for (int i = 3; i >=1 ; i-- ) {
        if (pos < itemlayers[i].size()) {
            std::vector<Item*>::iterator it=itemlayers[i].end();
            it -= pos+1;
            if (!moving) delete *it;
            itemlayers[i].erase(it);

            ASSERTFRIENDLY(TextureIntegrityTest(), "Tile::Remove(unsigned char pos): Integrity test failed");
            ONThreadUnsafe(threadsafe);
            return;
        }
        pos -= itemlayers[i].size();
    }
    if (pos < creatures.size()) {
        std::vector<Creature*>::iterator it=creatures.end();
        it -= pos+1;

        //if (!moving) delete *it; // NEVER delete creatures since they're also kept in gamemap

        creatures.erase(it);

        ASSERTFRIENDLY(TextureIntegrityTest(), "Tile::Remove(unsigned char pos): Integrity test failed");
        ONThreadUnsafe(threadsafe);
        return;
    }
    pos -= creatures.size();
    if (pos < itemlayers[0].size()) {
        std::vector<Item*>::iterator it=itemlayers[0].end();
        it -= pos+1;
        if (!moving) delete *it;
        itemlayers[0].erase(it);

		ASSERTFRIENDLY(TextureIntegrityTest(), "Tile::Remove(unsigned char pos): Integrity test failed");
		ONThreadUnsafe(threadsafe);
        return;
    }
    pos -= itemlayers[0].size();
    printf("Tile::remove(unsigned char pos): FAILED\n");
    itemcount ++;

    ONThreadUnsafe(threadsafe);
}


void Tile::Remove(Thing *obj, bool moving) {
    ONThreadSafe(threadsafe);

    std::vector<Item*>::iterator it;
    std::vector<Creature*>::iterator ct;
    std::vector<Effect*>::iterator et;
	printf("Removing...");
    for (et=effects.begin(); et != effects.end(); et++) {
        if (*et==obj) {
			printf("effect\n");
            delete *et; // effects are the only thing that is verified to be DELETEable
            effects.erase(et);
            ONThreadUnsafe(threadsafe);
            return;
        }
    }
    printf("Is it an effect?");
    printf(dynamic_cast<Effect*>(obj) ? "yes" : "no");
	if (dynamic_cast<Effect*>(obj)) ASSERT(false);

    itemcount --;
    if (ground) {
        if (ground == obj) {
            if (!moving) delete ground;
            ground=NULL;

            ONThreadUnsafe(threadsafe);
            return;
        }
    }
    for (int i = 3; i >=0 ; i-- ) {
        for (it=itemlayers[i].begin(); it != itemlayers[i].end(); it++) {
            if (*it == obj) {
                if (!moving) delete *it;
                itemlayers[i].erase(it);
                ONThreadUnsafe(threadsafe);
                return;
            }
        }
    }
    for (ct=creatures.begin(); ct != creatures.end(); ct++) {
        if (*ct == obj) {
            //if (!moving) delete *ct; // NEVER delete creatures since they're also kept in gamemap
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
    #ifdef WIN32
    ONThreadSafe(threadsafe);
    #endif
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
        return creatures[creatures.size() - pos - 1];
    }
    pos -= creatures.size();

    if (pos < itemlayers[0].size()) {
        ONThreadUnsafe(threadsafe);
        return itemlayers[0][pos];
    }
    pos -= itemlayers[0].size();
    printf("Tile::getstackpos(unsigned char pos): FAILED (argument: %d, total size: %d, remaining: %d)\n", initialpos, itemcount, pos );
    #ifdef WIN32
    ONThreadUnsafe(threadsafe);
    #endif
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
            delete itemlayers[i][pos];
            if (newthing->GetTopIndex()==i) {
                itemlayers[i][pos] = (Item*)newthing;
                ONThreadUnsafe(threadsafe);
                return;
            } else {
                itemlayers[i].erase(itemlayers[i].begin() + pos);
                itemcount --;
                ONThreadUnsafe(threadsafe);
                this->Insert(newthing, true);
                return;
            }
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
        if (newthing->GetTopIndex()==0) {
            itemlayers[0][pos] = (Item*)newthing;
            ONThreadUnsafe(threadsafe);
            return;
        } else {
            itemlayers[0].erase(itemlayers[0].begin() + pos);
            itemcount --;
            ONThreadUnsafe(threadsafe);
            this->Insert(newthing, true);
            return;
        }
    }
    pos -= itemlayers[0].size();
    printf("Tile::replace(): FAILED\n");

    ONThreadUnsafe(threadsafe);
    return;
}




void Tile::SetPos(const position_t *p) {
    ONThreadSafe(threadsafe);
    pos.x = p->x;
    pos.y = p->y;
    pos.z = p->z;
    ONThreadUnsafe(threadsafe);
}
void Tile::RenderStrayCreatures(const position_t *p) {
    ONThreadSafe(threadsafe);
    unsigned int grndspeed = 500;// a safe default ...
    unsigned int creaturespeed = 220; //  a safe default...
    if (ground) {
        grndspeed = ground->GetSpeedIndex();
        if (!grndspeed) grndspeed = 500; // a safe fallback, once again
    }
    if (!this)  return;
    for (std::vector<Creature*>::iterator it = this->creatures.begin(); it != this->creatures.end(); it++) {
        Creature *cr = (*it);
        if (!cr) {
            DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_WARNING, "A null item encountered while trying to render stray creatures");
            continue;
        } else {
            creaturespeed = cr->GetSpeed();
            creaturespeed = (creaturespeed ? creaturespeed : 220);

            if (cr->IsMoving() &&
                ((p->x > pos.x && p->y == pos.y && cr->GetDirection() == WEST) ||
                 (p->y > pos.y && p->x == pos.x && cr->GetDirection() == NORTH) ||
                 (p->y <= pos.y && p->x == pos.x && cr->GetDirection() == SOUTH) ||
                 (p->x > pos.x && p->y == pos.y && cr->GetDirection() == EAST))
            ) {

/*                printf("Direction is ");
                switch(cr->GetDirection()) {
                    case NORTH:
                        printf("north\n");
                        break;
                    case WEST:
                        printf("west\n");
                        break;
                    case EAST:
                        printf("east\n");
                        break;
                    case SOUTH:
                        printf("south\n");
                        break;
					default:
						printf("some other direction!\n");
                }

                printf("p->x compared to pos.x is ");
                if (p->x > pos.x) printf("greater\n");
                if (p->x == pos.x) printf("equal\n");
                if (p->x < pos.x) printf("smaller\n");

                printf("p->y compared to pos.y is ");
                if (p->y > pos.y) printf("greater\n");
                if (p->y == pos.y) printf("equal\n");
                if (p->y < pos.y) printf("smaller\n");
*/

                //glColor4f(1., 0, 0, 1.);
                glTranslatef(-(p->x - pos.x) * 32, (p->y - pos.y) * 32, 0);
                cr->Render(p);
                glTranslatef((p->x - pos.x) * 32, -(p->y - pos.y) * 32, 0);
                //glColor4f(1.,1.,1.,1.);
                //printf("Creature rendered ok\n");
            }
//            if (cr->IsMoving()) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                //cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);

        }
    }
    ONThreadUnsafe(threadsafe);
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

                if (!cr->IsMoving() )
                    cr->Render(&pos);
                //if (cr->IsMoving()) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                //    cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
            } else {
                th->AnimationAdvance(100./fps);
                th->Render(&pos);
            }
        }



        for (std::vector<Item*>::iterator it = this->itemlayers[2].begin(); it != this->itemlayers[2].end(); it++) if (items[(*it)->GetType()]->splash) {
            Thing *th = (*it);

            if (Creature * cr = dynamic_cast<Creature*>(th)) {
                creaturespeed = cr->GetSpeed();
                creaturespeed = (creaturespeed ? creaturespeed : 220);

                if (!cr->IsMoving() )
                    cr->Render(&pos);
                //if (cr->IsMoving()) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                //    cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
            } else {
                th->AnimationAdvance(100./fps);
                th->Render(&pos);
            }
        }








        for (std::vector<Item*>::iterator it = this->itemlayers[1].begin(); it != this->itemlayers[1].end(); it++) {
            Thing *th = (*it);

            if (Creature * cr = dynamic_cast<Creature*>(th)) {
                creaturespeed = cr->GetSpeed();
                creaturespeed = (creaturespeed ? creaturespeed : 220);

                if (!cr->IsMoving() )
                    cr->Render(&pos);
                //if (cr->IsMoving()) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                //    cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
            } else {
                th->AnimationAdvance(100./fps);
                th->Render(&pos);
            }
        }


        for (std::vector<Item*>::iterator it = this->itemlayers[2].begin(); it != this->itemlayers[2].end(); it++) if (!items[(*it)->GetType()]->splash) {
            Thing *th = (*it);

            if (Creature * cr = dynamic_cast<Creature*>(th)) {
                creaturespeed = cr->GetSpeed();
                creaturespeed = (creaturespeed ? creaturespeed : 220);

                if (!cr->IsMoving() )
                    cr->Render(&pos);
                //if (cr->IsMoving()) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                //    cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
            } else {
                th->AnimationAdvance(100./fps);
                th->Render(&pos);
            }
        }




        for (std::vector<Item*>::iterator it = this->itemlayers[0].begin(); it != this->itemlayers[0].end(); it++) {
            Thing *th = (*it);

            if (Creature * cr = dynamic_cast<Creature*>(th)) {
                creaturespeed = cr->GetSpeed();
                creaturespeed = (creaturespeed ? creaturespeed : 220);

                if (!cr->IsMoving() )
                    cr->Render(&pos);
                //if (cr->IsMoving()) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                //    cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
            } else {
                th->AnimationAdvance(100./fps);
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

                if (!cr->IsMoving() )
                    cr->Render(&pos);
                //if (cr->IsMoving()) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                //    cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
            } else {
                th->AnimationAdvance(100./fps);
                th->Render(&pos);
            }
        }



        for (std::vector<Item*>::iterator it = this->itemlayers[3].begin(); it != this->itemlayers[3].end(); it++) {
            Thing *th = (*it);

            if (Creature * cr = dynamic_cast<Creature*>(th)) {
                creaturespeed = cr->GetSpeed();
                creaturespeed = (creaturespeed ? creaturespeed : 220);

                if (!cr->IsMoving() )
                    cr->Render(&pos);
                //if (cr->IsMoving()) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                //    cr->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
            } else {
                th->AnimationAdvance(100./fps);
                th->Render(&pos);
            }
        }




        for (std::vector<Effect*>::iterator it = this->effects.begin(); it != this->effects.end(); it++) {
            (*it)->Render(&pos, false);
            if (!(*it)->AnimationAdvance(2000./fps, false)) break;
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

        if (pos.x < player->pos.x + 8 + 1
         && pos.x > player->pos.x - 8 - 1
         && pos.y < player->pos.y + 6 + 1
         && pos.y > player->pos.y - 6 - 1
         && pos.z == player->pos.z
         && creatures.size()) {
             for (std::vector<Creature*>::iterator it = creatures.begin(); it != creatures.end(); it++) {
                 creaturespeed = (*it)->GetSpeed();
                 creaturespeed = (creaturespeed ? creaturespeed : 220);

                 (*it)->RenderOverlay();
                 if ((*it)->IsMoving()) // maybe the below function call should be changed into MoveAdvance() which would be passed only the grndspeed?
                     (*it)->AnimationAdvance( (100. * creaturespeed / grndspeed) / fps);
             }
         }



        for (std::vector<Effect*>::iterator it = this->effects.begin(); it != this->effects.end(); it++) {
            (*it)->Render(&pos, true);
            if (!(*it)->AnimationAdvance(2000./fps, true)) break;
        }

		CommitDelayedRemove();
        ONThreadUnsafe(threadsafe);
    }





}

void Tile::Empty () {
    ONThreadSafe(threadsafe);
    if (ground) {
        delete ground;
        ground = NULL;
    }

    for (std::vector<Effect*>::iterator it = effects.begin(); it != effects.end(); ) {
        if (*it)
            delete (*it);
        else
            printf("@(@@@)@)@)(@)(@@( OMFG There's a NULL effect on a tile!!\n");
        effects.erase(it);
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

void Tile::DelayedRemove(Thing *th) {
	delayedremove.insert(delayedremove.end(), th);
}
void Tile::CommitDelayedRemove() {
	ONThreadSafe(threadsafe);
/*	if (delayedremove.size()) {
		DEBUGPRINT(DEBUGPRINT_LEVEL_DEBUGGING, DEBUGPRINT_NORMAL, "Committing delayed remove of %d things\n", delayedremove.size());
	}*/
	for (std::vector<Thing*>::iterator it = delayedremove.begin(); it != delayedremove.end(); it++) {
		Remove(*it);
	}
	delayedremove.clear();
	if (delayedremove.size()) {
		DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_ERROR, "Tile::CommitDelayedRemove(): delayedremove not empty even after all has been processed\n");
	}
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
            DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Layer %d: %d[%d]\n", i, (*it)->GetType(), (*it)->GetSubType());
        }
    }

    for (std::vector<Creature*>::iterator ct=creatures.begin(); ct != creatures.end(); ct++) {
		DEBUGPRINT(DEBUGPRINT_LEVEL_USEFUL, DEBUGPRINT_NORMAL, "Creature: %s\n", (*ct)->GetName().c_str());
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

//    return GetTopLookAt();



    stackpos = 0;
    topusable = 255;
    if (ground) {

        if (items[ground->GetType()]->ladder) {
            topusable = stackpos;

        }
        stackpos ++;
    }

    for (i = 1; i <=3 ; i++ ) {
        for (it = itemlayers[i].begin(); it != itemlayers[i].end(); it++) {
            if (items[(*it)->GetType()]->ladder) {
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
        if (items[(*it)->GetType()]->ladder) {
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
