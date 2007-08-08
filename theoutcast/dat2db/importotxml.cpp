#include <sqlite3.h>

#include <iostream>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <string>
#include <string.h>

#ifdef XML_GCC_FREE
        #define xmlFreeOTSERV(s)        free(s)
#else
        #define xmlFreeOTSERV(s)        xmlFree(s)
#endif

#ifdef WIN32
	#define strcasecmp strcmp
#endif

#define ATOI64 atoll


extern "C" {
	int dbexecprintf(sqlite3* db, sqlite3_callback cb, void *arg, char **errmsg, const char *sql, ...);
	int dbexec(sqlite3* db, const char*sql, sqlite3_callback cb, void *arg, char **errmsg);
	void show_progress(int currentid, int dat_items);
	int datversion;
	int dat_items;
	extern sqlite3* fo;
	extern char lastpercentage;
}


typedef enum {
	ITEM_GROUP_KEY, ITEM_GROUP_MAGICFIELD
} grouptype;
typedef enum {
	WEAPON_SWORD, WEAPON_CLUB, WEAPON_AXE, WEAPON_SHIELD, WEAPON_DIST, WEAPON_WAND, WEAPON_AMMO
} weapontypetype;
typedef enum {
	AMMO_ARROW, AMMO_BOLT
} amutypetype;
typedef enum {
	SHOOT_SPEAR, SHOOT_BOLT, SHOOT_ARROW, SHOOT_FIRE, SHOOT_ENERGY, SHOOT_POISONARROW, SHOOT_BURSTARROW, SHOOT_THROWINGSTAR,
	SHOOT_THROWINGKNIFE, SHOOT_SMALLSTONE, SHOOT_SUDDENDEATH, SHOOT_LARGEROCK, SHOOT_SNOWBALL, SHOOT_POWERBOLT, SHOOT_POISONFIELD,
	SHOOT_INFERNALBOLT
} shoottypetype;
typedef enum {
	SKILL_SWORD, SKILL_AXE, SKILL_CLUB, SKILL_DIST, SKILL_FISH, SKILL_SHIELD, SKILL_FIST
} skillstype;
#define SLOTP_HEAD 1
#define SLOTP_ARMOR 2
#define SLOTP_LEGS 4
#define SLOTP_FEET 8
#define SLOTP_BACKPACK (8*2)
#define SLOTP_TWO_HAND 32
#define SLOTP_NECKLACE (32*2)
#define SLOTP_RING 128

typedef enum {
	CONDITION_DRUNK=0
} conditiontypes;
typedef enum {
	COMBAT_NONE
} CombatType_t;
typedef struct {
} ConditionDamage;
struct items_s {
	int id;
	grouptype group;
	weapontypetype weaponType;
	int slot_position;
	std::string name, description, runeSpellName;
	int armor, defence,attack, rotateTo, maxItems, maxTextLen, writeOnceItemId;
	float weight;
	bool canWriteText, canReadText;
	amutypetype amuType;
	shoottypetype shootType;
	int stopTime, decayTo, transformEquipTo, transformDeEquipTo, charges, showCharges;
	float decayTime, showDuration;
	struct {
		int invisible, speed, regeneration, healthGain, healthTicks;
		int manaGain, manaTicks, manaShield;
		int skills[20];
		int absorbPercentAll, absorbPercentEnergy, absorbPercentFire, absorbPercentLifeDrain, absorbPercentManaDrain, absorbPercentPhysical;
		int absorbPercentPoison;
		int conditionSuppressions;
	} abilities;

};




bool readXMLInteger(xmlNodePtr node, const char* tag, int& value)
{
        char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
        if(nodeValue){
                value = atoi(nodeValue);
                xmlFreeOTSERV(nodeValue);
                return true;
        }

        return false;
}

bool readXMLInteger64(xmlNodePtr node, const char* tag, uint64_t& value)
{
        char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
        if(nodeValue){
                value = ATOI64(nodeValue);
                xmlFreeOTSERV(nodeValue);
                return true;
        }

        return false;
}

bool readXMLFloat(xmlNodePtr node, const char* tag, float& value)
{
        char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
        if(nodeValue){
                value = atof(nodeValue);
                xmlFreeOTSERV(nodeValue);
                return true;
        }

        return false;
}

bool readXMLString(xmlNodePtr node, const char* tag, std::string& value)
{
        char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
        if(nodeValue){
                value = nodeValue;
                xmlFreeOTSERV(nodeValue);
                return true;
        }

        return false;
}








bool loadFromXml(const std::string& filename)
{

	xmlDocPtr doc = xmlParseFile(filename.c_str());

	int intValue;
	std::string strValue;
	uint32_t id = 0;

	if(doc){
		xmlNodePtr root = xmlDocGetRootElement(doc);

		if(xmlStrcmp(root->name,(const xmlChar*)"items") != 0){

			xmlFreeDoc(doc);
			return false;
		}

		xmlNodePtr itemNode = root->children;
		while(itemNode){
			if(xmlStrcmp(itemNode->name,(const xmlChar*)"item") == 0){
				if(readXMLInteger(itemNode, "id", intValue)){
					id = intValue;
					items_s* iType=NULL;
					if(id > 20000 && id < 20100){
						id = id - 20000;

						iType = new items_s();
						iType->id = id;
//						items.addElement(iType, iType->id);
					}
					if (!iType) {
						iType = new items_s();
						iType->id = id;
					}

//					ItemType& it = Item::items.getItemType(id);
					items_s &it = *iType;

					if(readXMLString(itemNode, "name", strValue)){
						it.name = strValue;
					}

					xmlNodePtr itemAttributesNode = itemNode->children;

					while(itemAttributesNode){
						if(readXMLString(itemAttributesNode, "key", strValue)){
							if(strcasecmp(strValue.c_str(), "type") == 0){
								if(readXMLString(itemAttributesNode, "value", strValue)){
									if(strcasecmp(strValue.c_str(), "key") == 0){
										it.group = ITEM_GROUP_KEY;
									}
									else if(strcasecmp(strValue.c_str(), "magicfield") == 0){
										it.group = ITEM_GROUP_MAGICFIELD;
									}
									else if(strcasecmp(strValue.c_str(), "depot") == 0){
										//it.group = ITEM_GROUP_DEPOT;
									}
									else{
										std::cout << "Warning: [Items::loadFromXml] " << "Unknown type " << strValue  << std::endl;
									}
								}
							}
							else if(strcasecmp(strValue.c_str(), "name") == 0){
								if(readXMLString(itemAttributesNode, "value", strValue)){
									it.name = strValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "description") == 0){
								if(readXMLString(itemAttributesNode, "value", strValue)){
									it.description = strValue;
								}
							}


							#if 0

							else if(strcasecmp(strValue.c_str(), "runeSpellName") == 0){
								if(readXMLString(itemAttributesNode, "value", strValue)){
									it.runeSpellName = strValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "weight") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.weight = intValue / 100.f;
								}
							}
							else if(strcasecmp(strValue.c_str(), "armor") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.armor = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "defense") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.defence = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "attack") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.attack = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "rotateTo") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.rotateTo = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "containerSize") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.maxItems = intValue;
								}
							}
							/*
							else if(strcasecmp(strValue.c_str(), "readable") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.canReadText = true;
								}
							}
							*/
							else if(strcasecmp(strValue.c_str(), "writeable") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.canWriteText = true;
									it.canReadText = true;
								}
							}
							else if(strcasecmp(strValue.c_str(), "maxTextLen") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.maxTextLen = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "readOnceItemId") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.writeOnceItemId = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "weaponType") == 0){
								if(readXMLString(itemAttributesNode, "value", strValue)){
									if(strcasecmp(strValue.c_str(), "sword") == 0){
										it.weaponType = WEAPON_SWORD;
									}
									else if(strcasecmp(strValue.c_str(), "club") == 0){
										it.weaponType = WEAPON_CLUB;
									}
									else if(strcasecmp(strValue.c_str(), "axe") == 0){
										it.weaponType = WEAPON_AXE;
									}
									else if(strcasecmp(strValue.c_str(), "shield") == 0){
										it.weaponType = WEAPON_SHIELD;
									}
									else if(strcasecmp(strValue.c_str(), "distance") == 0){
										it.weaponType = WEAPON_DIST;
									}
									else if(strcasecmp(strValue.c_str(), "wand") == 0){
										it.weaponType = WEAPON_WAND;
									}
									else if(strcasecmp(strValue.c_str(), "ammunition") == 0){
										it.weaponType = WEAPON_AMMO;
									}
									else{
										std::cout << "Warning: [Items::loadFromXml] " << "Unknown weaponType " << strValue  << std::endl;
									}
								}
							}
							else if(strcasecmp(strValue.c_str(), "slotType") == 0){
								if(readXMLString(itemAttributesNode, "value", strValue)){
									if(strcasecmp(strValue.c_str(), "head") == 0){
										it.slot_position |= SLOTP_HEAD;
									}
									else if(strcasecmp(strValue.c_str(), "body") == 0){
										it.slot_position |= SLOTP_ARMOR;
									}
									else if(strcasecmp(strValue.c_str(), "legs") == 0){
										it.slot_position |= SLOTP_LEGS;
									}
									else if(strcasecmp(strValue.c_str(), "feet") == 0){
										it.slot_position |= SLOTP_FEET;
									}
									else if(strcasecmp(strValue.c_str(), "backpack") == 0){
										it.slot_position |= SLOTP_BACKPACK;
									}
									else if(strcasecmp(strValue.c_str(), "two-handed") == 0){
										it.slot_position |= SLOTP_TWO_HAND;
									}
									else if(strcasecmp(strValue.c_str(), "necklace") == 0){
										it.slot_position |= SLOTP_NECKLACE;
									}
									else if(strcasecmp(strValue.c_str(), "ring") == 0){
										it.slot_position |= SLOTP_RING;
									}
									else{
										std::cout << "Warning: [Items::loadFromXml] " << "Unknown slotType " << strValue  << std::endl;
									}
								}
							}
							else if(strcasecmp(strValue.c_str(), "ammoType") == 0){
								if(readXMLString(itemAttributesNode, "value", strValue)){
									if(strcasecmp(strValue.c_str(), "arrow") == 0){
										it.amuType = AMMO_ARROW;
									}
									else if(strcasecmp(strValue.c_str(), "bolt") == 0){
										it.amuType = AMMO_BOLT;
									}
									else{
										std::cout << "Warning: [Items::loadFromXml] " << "Unknown ammoType " << strValue  << std::endl;
									}
								}
							}
							else if(strcasecmp(strValue.c_str(), "shootType") == 0){
								if(readXMLString(itemAttributesNode, "value", strValue)){
									if(strcasecmp(strValue.c_str(), "spear") == 0){
										it.shootType = SHOOT_SPEAR;
									}
									else if(strcasecmp(strValue.c_str(), "bolt") == 0){
										it.shootType = SHOOT_BOLT;
									}
									else if(strcasecmp(strValue.c_str(), "arrow") == 0){
										it.shootType = SHOOT_ARROW;
									}
									else if(strcasecmp(strValue.c_str(), "fire") == 0){
										it.shootType = SHOOT_FIRE;
									}
									else if(strcasecmp(strValue.c_str(), "energy") == 0){
										it.shootType = SHOOT_ENERGY;
									}
									else if(strcasecmp(strValue.c_str(), "poisonarrow") == 0){
										it.shootType = SHOOT_POISONARROW;
									}
									else if(strcasecmp(strValue.c_str(), "burstarrow") == 0){
										it.shootType = SHOOT_BURSTARROW;
									}
									else if(strcasecmp(strValue.c_str(), "throwingstar") == 0){
										it.shootType = SHOOT_THROWINGSTAR;
									}
									else if(strcasecmp(strValue.c_str(), "throwingknife") == 0){
										it.shootType = SHOOT_THROWINGKNIFE;
									}
									else if(strcasecmp(strValue.c_str(), "smallstone") == 0){
										it.shootType = SHOOT_SMALLSTONE;
									}
									else if(strcasecmp(strValue.c_str(), "suddendeath") == 0){
										it.shootType = SHOOT_SUDDENDEATH;
									}
									else if(strcasecmp(strValue.c_str(), "largerock") == 0){
										it.shootType = SHOOT_LARGEROCK;
									}
									else if(strcasecmp(strValue.c_str(), "snowball") == 0){
										it.shootType = SHOOT_SNOWBALL;
									}
									else if(strcasecmp(strValue.c_str(), "powerbolt") == 0){
										it.shootType = SHOOT_POWERBOLT;
									}
									else if(strcasecmp(strValue.c_str(), "poison") == 0){
										it.shootType = SHOOT_POISONFIELD;
									}
									else if(strcasecmp(strValue.c_str(), "infernalbolt") == 0){
										it.shootType = SHOOT_INFERNALBOLT;
									}
									else{
										std::cout << "Warning: [Items::loadFromXml] " << "Unknown shootType " << strValue  << std::endl;
									}
								}
							}
							else if(strcasecmp(strValue.c_str(), "stopduration") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.stopTime = (intValue != 0);
								}
							}
							else if(strcasecmp(strValue.c_str(), "decayTo") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.decayTo = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "transformEquipTo") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.transformEquipTo = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "transformDeEquipTo") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.transformDeEquipTo = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "duration") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.decayTime = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "showduration") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.showDuration = true;
								}
							}
							else if(strcasecmp(strValue.c_str(), "charges") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.charges = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "showcharges") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.showCharges = true;
								}
							}
							else if(strcasecmp(strValue.c_str(), "invisible") == 0){
								it.abilities.invisible = true;
							}
							else if(strcasecmp(strValue.c_str(), "speed") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.speed = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "healthGain") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.regeneration = true;
									it.abilities.healthGain = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "healthTicks") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.regeneration = true;
									it.abilities.healthTicks = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "manaGain") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.regeneration = true;
									it.abilities.manaGain = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "manaTicks") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.regeneration = true;
									it.abilities.manaTicks = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "manaShield") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.manaShield = true;
								}
							}
							else if(strcasecmp(strValue.c_str(), "skillSword") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.skills[SKILL_SWORD] = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "skillAxe") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.skills[SKILL_AXE] = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "skillClub") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.skills[SKILL_CLUB] = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "skillDist") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.skills[SKILL_DIST] = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "skillFish") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.skills[SKILL_FISH] = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "skillShield") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.skills[SKILL_SHIELD] = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "skillFist") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.skills[SKILL_FIST] = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "absorbPercentAll") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.absorbPercentAll = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "absorbPercentEnergy") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.absorbPercentEnergy = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "absorbPercentFire") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.absorbPercentFire = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "absorbPercentPoison") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.absorbPercentPoison = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "absorbPercentLifeDrain") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.absorbPercentLifeDrain = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "absorbPercentManaDrain") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.absorbPercentManaDrain = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "absorbPercentPhysical") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.absorbPercentPhysical = intValue;
								}
							}
							else if(strcasecmp(strValue.c_str(), "suppressDrunk") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.conditionSuppressions |= CONDITION_DRUNK;
								}
							}
							/*
							else if(strcasecmp(strValue.c_str(), "suppressEnergy") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.conditionSuppressions |= CONDITION_ENERGY;
								}
							}
							else if(strcasecmp(strValue.c_str(), "suppressFire") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.conditionSuppressions |= CONDITION_FIRE;
								}
							}
							else if(strcasecmp(strValue.c_str(), "suppressPoison") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.conditionSuppressions |= CONDITION_POISON;
								}
							}
							else if(strcasecmp(strValue.c_str(), "suppressLifeDrain") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.conditionSuppressions |= CONDITION_LIFEDRAIN;
								}
							}
							else if(strcasecmp(strValue.c_str(), "suppressManaDrain") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.conditionSuppressions |= CONDITION_MANADRAIN;
								}
							}
							else if(strcasecmp(strValue.c_str(), "suppressPhysical") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
									it.abilities.conditionSuppressions |= CONDITION_PHYSICAL;
								}
							}
							*/
							else if(strcasecmp(strValue.c_str(), "field") == 0){
								it.group = ITEM_GROUP_MAGICFIELD;
								CombatType_t combatType = COMBAT_NONE;
								ConditionDamage* conditionDamage = NULL;

								if(readXMLString(itemAttributesNode, "value", strValue)){
									/*if(strcasecmp(strValue.c_str(), "fire") == 0){
										conditionDamage = new ConditionDamage(CONDITIONID_COMBAT, CONDITION_FIRE);
										combatType = COMBAT_FIREDAMAGE;
									}
									else if(strcasecmp(strValue.c_str(), "energy") == 0){
										conditionDamage = new ConditionDamage(CONDITIONID_COMBAT, CONDITION_ENERGY);
										combatType = COMBAT_ENERGYDAMAGE;
									}
									else if(strcasecmp(strValue.c_str(), "poison") == 0){
										conditionDamage = new ConditionDamage(CONDITIONID_COMBAT, CONDITION_POISON);
										combatType = COMBAT_POISONDAMAGE;
									}
									//else if(strcasecmp(strValue.c_str(), "physical") == 0){
									//	damageCondition = new ConditionDamage(CONDITIONID_COMBAT, CONDITION_PHYSICAL);
									//	combatType = COMBAT_PHYSICALDAMAGE;
									//}
									*/
									if(combatType != COMBAT_NONE){
/*										it.combatType = combatType;
										it.condition = conditionDamage;
										uint32_t ticks = 0;
										int32_t damage = 0;
										int32_t start = 0;
										int32_t count = 1;
*/
										xmlNodePtr fieldAttributesNode = itemAttributesNode->children;
										while(fieldAttributesNode){
											if(readXMLString(fieldAttributesNode, "key", strValue)){
												if(strcasecmp(strValue.c_str(), "ticks") == 0){
													if(readXMLInteger(fieldAttributesNode, "value", intValue)){
//														ticks = std::max(0, intValue);
													}
												}

												if(strcasecmp(strValue.c_str(), "count") == 0){
													if(readXMLInteger(fieldAttributesNode, "value", intValue)){
														if(intValue > 0){
//															count = intValue;
														}
														else{
//															count = 1;
														}
													}
												}

												if(strcasecmp(strValue.c_str(), "start") == 0){
													if(readXMLInteger(fieldAttributesNode, "value", intValue)){
														if(intValue > 0){
//															start = intValue;
														}
														else{
//															start = 0;
														}
													}
												}

												if(strcasecmp(strValue.c_str(), "damage") == 0){
													if(readXMLInteger(fieldAttributesNode, "value", intValue)){

/*														damage = -intValue;

														if(start > 0){
															std::list<int32_t> damageList;
															ConditionDamage::generateDamageList(damage, start, damageList);

															for(std::list<int32_t>::iterator it = damageList.begin(); it != damageList.end(); ++it){
																conditionDamage->addDamage(1, ticks, -*it);
															}

															start = 0;
														}
														else{
															conditionDamage->addDamage(count, ticks, damage);
														}*/
													}
												}
											}

											fieldAttributesNode = fieldAttributesNode->next;
										}
									}
								}
							}
							else if(strcasecmp(strValue.c_str(), "replaceable") == 0){
								if(readXMLInteger(itemAttributesNode, "value", intValue)){
//									it.replaceable = (intValue != 0);
								}
							}
							#endif
						}

						itemAttributesNode = itemAttributesNode->next;
					}
					show_progress(iType->id , dat_items);
					//printf(".");

					dbexecprintf(fo, NULL, 0, NULL, "update items%d set otname='%q' where otid=%d;", datversion, it.name.c_str(),  it.id );
					//printf("_");
					fflush(stdout);
				}
				else{
					std::cout << "Warning: [Spells::loadFromXml] - No itemid found" << std::endl;
				}
			}



			itemNode = itemNode->next;
		}

		xmlFreeDoc(doc);
	}

	return true;
}


//////////////////////////////////////////////////
extern "C" {

	void import_otxml_do(const char* fn) {
		printf("%s\n", fn);
		FILE* f = fopen(fn, "r");
		if (!f) {
			printf("XML file %s doesn't exist.\n", fn);
			return;
		}

		lastpercentage = -100;
		dbexecprintf(fo, NULL,NULL,NULL, "begin transaction;");
		if (loadFromXml(fn)) printf("OTXML imported\n"); else printf("Failed to import OTXML\n");
		dbexecprintf(fo, NULL,NULL,NULL, "end transaction;");

		fclose(f);
	}

}
