#ifdef INCLUDE_SP

#ifndef __SPCREATURE_H
#define __SPCREATURE_H

#include <string>
#include "types.h"
#include "creature.h"
#include "networkmessage.h"

class SPCreature {
	public:
		SPCreature(std::string name, short hp, short maxhp, int x, int y, int z, short type, char head, char body, char legs, char feet, short extended, short addons);
		SPCreature(std::string name, short hp, short maxhp, const position_t &pos, const creaturelook_t &look);
		Creature* MakeCreature();
		void Run(NetworkMessage&);

		position_t pos;
		creaturelook_t look;
		int cid;
		std::string name;
		short hp, maxhp;

		friend class ProtocolSP;
};



#endif

#endif
