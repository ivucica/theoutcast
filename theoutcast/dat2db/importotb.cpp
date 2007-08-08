#include <string>
#include <sqlite3.h>
#include "itemloader.h"
#include "items.h"

struct items_s {
	int cid,sid;
};
extern "C" {
	int dbexecprintf(sqlite3* db, sqlite3_callback cb, void *arg, char **errmsg, const char *sql, ...);
	int dbexec(sqlite3* db, const char*sql, sqlite3_callback cb, void *arg, char **errmsg);
	void show_progress(int currentid, int dat_items);
	extern int datversion;
	extern int dat_items;
	extern sqlite3* fo;
	extern char lastpercentage;
}

inline subfight_t translateOTBsubfight_t (subfightOTB_t sf) {
    switch (sf) {
    case OTB_DIST_NONE:
        return DIST_NONE;
        break;
    case OTB_DIST_BOLT:
        return DIST_BOLT;
        break;
    case OTB_DIST_ARROW:
        return DIST_ARROW;
        break;
    case OTB_DIST_FIRE:
        return DIST_FIRE;
        break;
    case OTB_DIST_ENERGY:
        return DIST_ENERGY;
        break;
    case OTB_DIST_POISONARROW:
        return DIST_POISONARROW;
        break;
    case OTB_DIST_BURSTARROW:
        return DIST_BURSTARROW;
        break;
    case OTB_DIST_THROWINGSTAR:
        return DIST_THROWINGSTAR;
        break;
    case OTB_DIST_THROWINGKNIFE:
        return DIST_THROWINGKNIFE;
        break;
    case OTB_DIST_SMALLSTONE:
        return DIST_SMALLSTONE;
        break;
    case OTB_DIST_SUDDENDEATH:
        return DIST_SUDDENDEATH;
        break;
    case OTB_DIST_LARGEROCK:
        return DIST_LARGEROCK;
        break;
    case OTB_DIST_SNOWBALL:
        return DIST_SNOWBALL;
        break;
    case OTB_DIST_POWERBOLT:
        return DIST_POWERBOLT;
        break;
    case OTB_DIST_SPEAR:
        return DIST_SPEAR;
        break;
    case OTB_DIST_POISONFIELD:
        return DIST_POISONFIELD;
        break;
    }
    return DIST_NONE;
}




int loadFromOtb (std::string file) {
    ItemLoader f;
    if (!f.openFile (file.c_str(), false, true) ) {
        return f.getError();
    }

    unsigned long type,len;
    const unsigned char* data;
    NODE node = f.getChildNode (NO_NODE, type);

    PropStream props;
    if (f.getProps (node,props) ) {
        //4 byte flags
        //attributes
        //0x01 = version data
        unsigned long flags;
        if (!props.GET_ULONG (flags) ) {
            return ERROR_INVALID_FORMAT;
        }
        attribute_t attr;
        datasize_t datalen = 0;
        if (!props.GET_VALUE (attr) ) {
            return ERROR_INVALID_FORMAT;
        }
        if (attr == ROOT_ATTR_VERSION) {
            if (!props.GET_VALUE (datalen) ) {
                return ERROR_INVALID_FORMAT;
            }
            if (datalen != sizeof (VERSIONINFO) ) {
                return ERROR_INVALID_FORMAT;
            }
            VERSIONINFO *vi;
            if (!props.GET_STRUCT (vi) ) {
                return ERROR_INVALID_FORMAT;
            }
            //Items::dwMajorVersion = vi->dwMajorVersion;
            //Items::dwMinorVersion = vi->dwMinorVersion;
            //Items::dwBuildNumber = vi->dwBuildNumber;
        }
    }
//	if(Items::dwMinorVersion != CLIENT_VERSION_760){
//		std::cout << "Not supported items.otb client version." << std::endl;
//		return ERROR_INVALID_FORMAT;
//	}

    node = f.getChildNode (node, type);

    while (node != NO_NODE) {
        data = f.getProps (node, len);
        if (data == NULL && f.getError() != ERROR_NONE)
            return f.getError();

        flags_t flags;
        if (data != NULL) {
            const unsigned char* p = &data[0];
            //ItemType* iType = new ItemType();

            items_s* newitem = new items_s;//(items_s*) malloc (sizeof (items_s) );



            bool loadedFlags = false;

            while (p < data + len) {
                //iType->group = (itemgroup_t)type;

                switch (type) {
                case ITEM_GROUP_NONE:
                case ITEM_GROUP_GROUND:
                case ITEM_GROUP_CONTAINER:
                case ITEM_GROUP_WEAPON:
                case ITEM_GROUP_AMMUNITION:
                case ITEM_GROUP_ARMOR:
                case ITEM_GROUP_RUNE:
                case ITEM_GROUP_TELEPORT:
                case ITEM_GROUP_MAGICFIELD:
                case ITEM_GROUP_WRITEABLE:
                case ITEM_GROUP_KEY:
                case ITEM_GROUP_SPLASH:
                case ITEM_GROUP_FLUID:
                case ITEM_GROUP_DOOR: {
                    if (!loadedFlags) {
                        //read 4 byte flags
                        memcpy ( (void*) &flags, p, sizeof (flags_t) );
                        p+= sizeof (flags_t);

                        //iType->blockSolid = ((flags & FLAG_BLOCK_SOLID) == FLAG_BLOCK_SOLID);
                        //iType->blockProjectile = ((flags & FLAG_BLOCK_PROJECTILE) == FLAG_BLOCK_PROJECTILE);
                        //iType->blockPathFind = ((flags & FLAG_BLOCK_PATHFIND) == FLAG_BLOCK_PATHFIND);
                        //iType->hasHeight = ((flags & FLAG_HAS_HEIGHT) == FLAG_HAS_HEIGHT);
                        //iType->useable = ((flags & FLAG_USEABLE) == FLAG_USEABLE);
                        //iType->pickupable = ((flags & FLAG_PICKUPABLE) == FLAG_PICKUPABLE);
                        //iType->moveable = ((flags & FLAG_MOVEABLE) == FLAG_MOVEABLE);
                        //iType->stackable = ((flags & FLAG_STACKABLE) == FLAG_STACKABLE);
                        //iType->floorChangeDown = ((flags & FLAG_FLOORCHANGEDOWN) == FLAG_FLOORCHANGEDOWN);
                        //iType->floorChangeNorth = ((flags & FLAG_FLOORCHANGENORTH) == FLAG_FLOORCHANGENORTH);
                        //iType->floorChangeEast = ((flags & FLAG_FLOORCHANGEEAST) == FLAG_FLOORCHANGEEAST);
                        //iType->floorChangeSouth = ((flags & FLAG_FLOORCHANGESOUTH) == FLAG_FLOORCHANGESOUTH);
                        //iType->floorChangeWest = ((flags & FLAG_FLOORCHANGEWEST) == FLAG_FLOORCHANGEWEST);
                        //iType->alwaysOnTop = ((flags & FLAG_ALWAYSONTOP) == FLAG_ALWAYSONTOP);
                        //iType->canDecay = !((flags & FLAG_CANNOTDECAY) == FLAG_CANNOTDECAY);
                        //iType->isVertical = ((flags & FLAG_VERTICAL) == FLAG_VERTICAL);
                        //iType->isHorizontal = ((flags & FLAG_HORIZONTAL) == FLAG_HORIZONTAL);
                        //iType->isHangable = ((flags & FLAG_HANGABLE) == FLAG_HANGABLE);
                        //iType->allowDistRead = ((flags & FLAG_ALLOWDISTREAD) == FLAG_ALLOWDISTREAD);
                        /*if (type==ITEM_GROUP_GROUND)
                            newitem->ground=true;*/
                        /*if(type == ITEM_GROUP_WRITEABLE) {
                        	iType->RWInfo |= CAN_BE_WRITTEN;
                        }

                        if((flags & FLAG_READABLE) == FLAG_READABLE)
                        	iType->RWInfo |= CAN_BE_READ;

                        iType->rotable = ((flags & FLAG_ROTABLE) == FLAG_ROTABLE);
                        */
                        if (p >= data + len) //no attributes
                            break;
                        loadedFlags = true;
                    }

                    //attribute
                    attribute_t attrib = *p;
                    p+= sizeof (attribute_t);
                    if (p >= data + len) {
//							delete iType;
                        return ERROR_INVALID_FORMAT;
                    }

                    datasize_t datalen = 0;
                    //size of data
                    memcpy (&datalen, p, sizeof (datasize_t) );
                    p+= sizeof (datalen);
                    if (p >= data + len) {
//							delete iType;
                        return ERROR_INVALID_FORMAT;
                    }

                    switch (attrib) {
                    case ITEM_ATTR_SERVERID: {
                        if (datalen != sizeof (unsigned short) )
                            return ERROR_INVALID_FORMAT;

                        unsigned short serverid = * (unsigned short*) p;
                        if (serverid > 20000 && serverid < 20100)
                            serverid = serverid - 20000;


//								iType->id = serverid;
                        newitem->sid = serverid;
                        break;
                    }

                    case ITEM_ATTR_CLIENTID: {
                        if (datalen != sizeof (unsigned short) )
                            return ERROR_INVALID_FORMAT;

//								memcpy(&iType->clientId, p, sizeof(unsigned short));
                        newitem->cid = * ( (unsigned short*) p);
                        break;
                    }
                    case ITEM_ATTR_NAME: {
                        char name[128];
                        if (datalen >= sizeof (name) )
                            return ERROR_INVALID_FORMAT;

                        memcpy (name, p, datalen);
                        name[datalen] = 0;



//								iType->name = name;
                        break;
                    }
                    case ITEM_ATTR_DESCR: {
                        char descr[128];
                        if (datalen >= sizeof (descr) )
                            return ERROR_INVALID_FORMAT;

                        memcpy (descr, p, datalen);
                        descr[datalen] = 0;
//								iType->description = descr;

                        break;
                    }
                    case ITEM_ATTR_SPEED: {
                        if (datalen != sizeof (unsigned short) )
                            return ERROR_INVALID_FORMAT;

//								memcpy(&iType->speed, p, sizeof(unsigned short));
                        break;
                    }
                    case ITEM_ATTR_SLOT: {
                        if (datalen != sizeof (unsigned short) )
                            return ERROR_INVALID_FORMAT;

                        unsigned short otb_slot = * (unsigned short*) p;

                        switch (otb_slot) {
                        case OTB_SLOT_DEFAULT:
                        case OTB_SLOT_WEAPON:
                        case OTB_SLOT_HAND:
                            //default
                            break;
                        case OTB_SLOT_HEAD:
//									iType->slot_position = SLOTP_HEAD;
                            break;
                        case OTB_SLOT_BODY:
//									iType->slot_position = SLOTP_ARMOR;
                            break;
                        case OTB_SLOT_LEGS:
//									iType->slot_position = SLOTP_LEGS;
                            break;
                        case OTB_SLOT_BACKPACK:
//									iType->slot_position = SLOTP_BACKPACK;
                            break;
                        case OTB_SLOT_2HAND:
//									iType->slot_position  = SLOTP_TWO_HAND;
                            break;
                        case OTB_SLOT_FEET:
//									iType->slot_position = SLOTP_FEET;
                            break;
                        case OTB_SLOT_AMULET:
//									iType->slot_position = SLOTP_NECKLACE;
                            break;
                        case OTB_SLOT_RING:
//									iType->slot_position = SLOTP_RING;
                            break;
                        }
//								iType->slot_position = iType->slot_position | SLOTP_LEFT | SLOTP_RIGHT | SLOTP_AMMO;
                        break;
                    }
                    case ITEM_ATTR_MAXITEMS: {
                        if (datalen != sizeof (unsigned short) )
                            return ERROR_INVALID_FORMAT;

//								memcpy(&iType->maxItems, p, sizeof(unsigned short));
                        break;
                    }
                    case ITEM_ATTR_WEIGHT: {
                        if (datalen != sizeof (double) )
                            return ERROR_INVALID_FORMAT;

//								memcpy(&iType->weight, p, sizeof(double));
                        break;
                    }
                    case ITEM_ATTR_MAGLEVEL: {
                        if (datalen != sizeof (unsigned short) )
                            return ERROR_INVALID_FORMAT;

//								memcpy(&iType->runeMagLevel, p, sizeof(unsigned short));

                        break;
                    }
                    case ITEM_ATTR_MAGFIELDTYPE: {
                        if (datalen != sizeof (unsigned char) )
                            return ERROR_INVALID_FORMAT;

//								memcpy(&iType->magicfieldtype, p, sizeof(unsigned char));

                        break;
                    }
                    case ITEM_ATTR_ROTATETO: {
                        if (datalen != sizeof (unsigned short) )
                            return ERROR_INVALID_FORMAT;

//								iType->rotateTo = *(unsigned short*)p;

                        break;
                    }
                    case ITEM_ATTR_SPRITEHASH: {
                        if (datalen != 16)
                            return ERROR_INVALID_FORMAT;
                        break;
                    }

                    case ITEM_ATTR_MINIMAPCOLOR: {
                        if (datalen != sizeof (unsigned short) )
                            return ERROR_INVALID_FORMAT;
                        break;
                    }
                    case ITEM_ATTR_07: {
                        if (datalen != sizeof (unsigned short) )
                            return ERROR_INVALID_FORMAT;
                        break;
                    }

                    case ITEM_ATTR_08: {
                        if (datalen != sizeof (unsigned short) )
                            return ERROR_INVALID_FORMAT;
                        break;
                    }
                    case ITEM_ATTR_DECAY2: {
                        if (datalen != sizeof (decayBlock2) )
                            return ERROR_INVALID_FORMAT;

                        decayBlock2 db2;
                        memcpy (&db2, p, sizeof (decayBlock2) );
//								iType->decayTime = db2.decayTime;
//								iType->decayTo = db2.decayTo;
                        break;
                    }
                    case ITEM_ATTR_WEAPON2: {
                        if (datalen != sizeof (weaponBlock2) )
                            return ERROR_INVALID_FORMAT;

                        weaponBlock2 wb2;
                        memcpy (&wb2, p, sizeof (weaponBlock2) );
                        /*								iType->weaponType = (WeaponType)wb2.weaponType;
                        								iType->shootType = translateOTBsubfight_t((subfightOTB_t)wb2.shootType);
                        								iType->amuType = (amu_t)wb2.amuType;
                        								iType->attack = wb2.attack;
                        								iType->defence = wb2.defence;
                        */
                        break;
                    }
                    case ITEM_ATTR_AMU2: {
                        if (datalen != sizeof (amuBlock2) )
                            return ERROR_INVALID_FORMAT;

                        amuBlock2 ab2;
                        memcpy (&ab2, p, sizeof (amuBlock2) );
                        /*								iType->weaponType = AMO;
                        								iType->shootType = translateOTBsubfight_t((subfightOTB_t)ab2.shootType);
                        								iType->amuType = (amu_t)ab2.amuType;
                        								iType->attack = ab2.attack;
                        */
                        break;
                    }
                    case ITEM_ATTR_ARMOR2: {
                        if (datalen != sizeof (armorBlock2) )
                            return ERROR_INVALID_FORMAT;

                        armorBlock2 ab2;
                        memcpy (&ab2, p, sizeof (armorBlock2) );
                        /*								iType->armor = ab2.armor;
                        								iType->weight = ab2.weight;
                        */								//ignore this value
                        //iType->slot_position = (slots_t)ab2.slot_position;

                        break;
                    }
                    case ITEM_ATTR_WRITEABLE3: {
                        if (datalen != sizeof (writeableBlock3) )
                            return ERROR_INVALID_FORMAT;

                        struct writeableBlock3 wb3;
                        memcpy (&wb3, p, sizeof (writeableBlock3) );
                        /*								iType->readOnlyId = wb3.readOnlyId;
                        								iType->maxTextLen = wb3.maxTextLen;
                        */
                        break;
                    }
                    case ITEM_ATTR_LIGHT2: {
                        if (datalen != sizeof (lightBlock2) )
                            return ERROR_INVALID_FORMAT;

                        lightBlock2 lb2;
                        memcpy (&lb2, p, sizeof (lightBlock2) );
                        /*								iType->lightLevel = lb2.lightLevel;
                        								iType->lightColor = lb2.lightColor;
                        */
                        break;
                    }
                    case ITEM_ATTR_TOPORDER: {
                        if (datalen != sizeof (unsigned char) )
                            return ERROR_INVALID_FORMAT;

//								memcpy(&iType->alwaysOnTopOrder, p, sizeof(unsigned char));
                        break;
                    }
                    default:
                        //ignore unknown attributes
                        //delete iType;
                        //return ERROR_INVALID_FORMAT;
                        break;
                    }


                    p+= datalen;
                    break;
                }

                default:
                    return ERROR_INVALID_FORMAT;
                    break;
                }
            }

            //get rune mag level from spells.xml
//			if(iType->group == ITEM_GROUP_RUNE){
            /*				std::map<unsigned short, Spell*>::iterator it = spells.getAllRuneSpells()->find(iType->id);
            				if(it != spells.getAllRuneSpells()->end()){
            					iType->runeMagLevel = it->second->getMagLv();
            				}
            				else{
            					iType->runeMagLevel = 0;
            				}*/

            //	}
            // store the found item
            //items[iType->id] = iType;
            //revItems[iType->clientId] = iType->id;

			show_progress(newitem->cid , dat_items);
            dbexecprintf(fo, NULL, 0, NULL, "update items%d set otid=%d where itemid=%d;", datversion, newitem->sid, newitem->cid );

        }

        node = f.getNextNode (node, type);
    }

    return ERROR_NONE;


}











//////////////////////////////////////////////////
extern "C" {

	void import_otb_do(const char* fn) {
		FILE* f = fopen(fn, "r");
		if (!f) {
			printf("OTB file %s doesn't exist.\n", fn);
			return;
		}

		lastpercentage = -100;
		dbexec(fo, "begin transaction",NULL,NULL,NULL);
		loadFromOtb(fn);
		dbexec(fo, "end transaction",NULL,NULL,NULL);
		printf("OTB imported\n");
		fclose(f);
	}

}
