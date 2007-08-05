#include "spcreature.h"
#include "map.h"
#include "protocol.h"
#include "protocolsp.h"
#include "console.h"
#include "player.h"
static int lastspcid=3;

SPCreature::SPCreature(std::string name, short hp, short maxhp, int x, int y, int z, short type, char head, char body, char legs, char feet, short extended, short addons) {
	pos.x = x; pos.y = y; pos.z = z;
	look.type = type;
	look.head = head;
	look.body = body;
	look.legs = legs;
	look.feet = feet;
	look.addons = addons;
	look.extendedlook = extended;
	this->name = name;
	this->hp = hp;
	this->maxhp = maxhp;
	this->cid = (lastspcid ++);
}


SPCreature::SPCreature(std::string name, short hp, short maxhp, const position_t &p, const creaturelook_t &crl) {
	pos = p;
	look = crl;
	this->hp = hp;
	this->maxhp = maxhp;
	this->cid = (lastspcid ++);
	this->name = name;
}


Creature* SPCreature::MakeCreature() {
	Creature *c = new Creature;
	c->SetCreatureID(cid);
	c->SetType(look.type, &look);
	c->SetName(name);
	c->SetDirection(EAST);
	c->SetHP((char)((float)hp*100./(float)maxhp));
	return c;
}

void SPCreature::Run(NetworkMessage &nm) {
	direction_t dir = (direction_t)(rand() % 4);
	position_t pos2=pos;


	switch (dir) {
		case NORTH:
			pos2.y--;
			break;
		case SOUTH:
			pos2.y++;
			break;
		case EAST:
			pos2.x++;
			break;
		case WEST:
			pos2.x--;
			break;
	}

	// FIXME (Khaos#2#) implement real passability check here

	Tile dest;
	std::vector<Thing*> remover;
	((ProtocolSP*)protocol)->SPFillTile(dest, remover, pos2.x, pos2.y, pos2.z);

	if (dest.GetItemCount()!=1 || (pos2.x == player->GetPosX()  && pos2.y == player->GetPosY() && pos2.z == player->GetPosZ()) ) {
		printf("There's something more than ground on destination!\n");
		return; // if there's something more than ground, guess it's unpassable
	}

	for (std::vector<Thing*>::iterator it = remover.begin(); it != remover.end(); it++) {
		delete *it;
	}


	printf("Moving %s from %d %d %d to %d %d %d\n", name.c_str(), pos.x, pos.y, pos.z, pos2.x, pos2.y, pos2.z);

	if (gamemap.IsVisible(pos)) {
		printf("Source visible, ");
		if (gamemap.IsVisible(pos2)) {
			printf("destination visible\n");
			nm.AddU8(0x6D); // move item
			protocol->AddPosition(&nm, &pos);
			nm.AddU8(1); // FIXME should figure out which stackpos is this creature... here we guess it's on stackpos 2, but it is possible it's on 3 or more
			protocol->AddPosition(&nm, &pos2);
			console.insert("Creature walked into screen!\n");
			pos = pos2;
		} else {
			printf("destination not visible\n");
			nm.AddU8(0x6C); // remove item
			protocol->AddPosition(&nm, &pos);
			nm.AddU8(1); // FIXME should figure out which stackpos is this creature... here we guess it's on stackpos 2, but it is possible it's on 3 or more
			console.insert("Creature walked out of screen!\n");
			pos = pos2;
		}
	} else {
		printf("Source not visible, ");
		if (gamemap.IsVisible(pos2)) {
			printf("destination visible\n");
			Creature* c=MakeCreature();
			nm.AddU8(0x6A); // add item
			protocol->AddPosition(&nm, &pos2);
			((ProtocolSP*)protocol)->SPAddCreature(&nm, c);

			pos = pos2;
			delete c;
		} else { // client doesnt know about moving if thing is not visible; dont send anything
			printf("destination not visible\n");
			pos = pos2;
		}
	}


	switch (look.type) {
		case 35:{ // demon

			position_t p;
			p.z = pos2.z;
			for (int i = pos2.x-1; i <= pos2.x+1; i++) {
				for (int j=pos2.y-1; j <= pos2.y+1; j++) {
					p.x = i; p.y = j;
					if (gamemap.IsVisible(p)) {
						nm.AddU8(0x83);
						protocol->AddPosition(&nm, &p);
						if (rand() % 100 > 75)
							nm.AddU8(rand()%3 + 5); // explosions
						else
							nm.AddU8(15+1); // fire
					}
				}
			}
		}
	}
}
