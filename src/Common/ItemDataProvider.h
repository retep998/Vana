/*
Copyright (C) 2008-2011 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#pragma once

#include "Types.h"
#include <boost/bimap.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <vector>
#include <string>

using boost::bimap;
using std::string;
using std::tr1::unordered_map;
using std::vector;

class Item;

struct SummonBag {
	int32_t mobid;
	uint32_t chance;
};

struct Skillbook {
	int32_t skillid;
	uint8_t reqlevel;
	uint8_t maxlevel;
	int8_t chance;
};

struct ScrollInfo {
	ScrollInfo() : randstat(false), recover(false), warmsupport(false), preventslip(false), ihand(0) { }
	bool warmsupport;
	bool randstat;
	bool recover;
	bool preventslip;
	int16_t istr;
	int16_t idex;
	int16_t iint;
	int16_t iluk;
	int16_t ihp;
	int16_t imp;
	int16_t iwatk;
	int16_t imatk;
	int16_t iwdef;
	int16_t imdef;
	int16_t iacc;
	int16_t iavo;
	int16_t ihand;
	int16_t ijump;
	int16_t ispeed;
	uint16_t success;
	uint16_t cursed;
};

struct Morph {
	Morph() : chance(100) { }
	int8_t chance;
	int16_t morph;
};

struct CardMapRange {
	int32_t startmap;
	int32_t endmap;
};

struct AilmentInfo {
	AilmentInfo() : darkness(false), weakness(false), poison(false), seal(false), curse(false) { }
	bool darkness;
	bool weakness;
	bool poison;
	bool seal;
	bool curse;
};

struct ConsumeInfo {
	ConsumeInfo() :
		fireresist(0), iceresist(0), lightningresist(0), poisonresist(0),
		cursedef(0), stundef(0), weaknessdef(0), darknessdef(0), sealdef(0),
		dropupitem(0), dropupitemrange(0),
		ailment(0),
		autoconsume(false), ignorewdef(false), ignoremdef(false), party(false), mousecancel(true),
		ignorecontinent(false), ghost(false), barrier(false), overridetraction(false), preventdrown(false),
		preventfreeze(false), mesoup(false), partydropup(false), dropup(false)
		{ }

	bool autoconsume;
	bool ignorewdef;
	bool ignoremdef;
	bool party;
	bool mousecancel;
	bool ignorecontinent;
	bool ghost;
	bool barrier;
	bool overridetraction;
	bool preventdrown;
	bool preventfreeze;
	bool mesoup;
	bool dropup;
	bool partydropup;
	uint8_t effect;
	uint8_t dechunger;
	uint8_t decfatigue;
	uint8_t cp;
	int16_t hp;
	int16_t mp;
	int16_t hpr;
	int16_t mpr;
	int16_t watk;
	int16_t matk;
	int16_t avo;
	int16_t acc;
	int16_t wdef;
	int16_t mdef;
	int16_t speed;
	int16_t jump;
	int16_t fireresist;
	int16_t iceresist;
	int16_t lightningresist;
	int16_t poisonresist;
	int16_t cursedef;
	int16_t stundef;
	int16_t weaknessdef;
	int16_t darknessdef;
	int16_t sealdef;
	int16_t dropupitemrange;
	uint16_t mcprob;
	int32_t dropupitem;
	int32_t moveTo;
	int32_t time;
	int32_t ailment;
	vector<Morph> morphs;
	vector<CardMapRange> mapranges;
};

struct ItemInfo {
	ItemInfo() :
		timelimited(false),
		nosale(false),
		karmascissors(false),
		expireonlogout(false),
		blockpickup(false),
		notrade(false),
		cash(false),
		quest(false)
		{ }

	string name;
	uint8_t minlevel;
	uint8_t maxlevel;
	uint8_t makerlevel;
	uint16_t maxslot;
	int32_t price;
	int32_t maxobtainable;
	int32_t exp;
	int32_t npc;
	int32_t mesos;
	bool timelimited;
	bool nosale;
	bool karmascissors;
	bool expireonlogout;
	bool blockpickup;
	bool notrade;
	bool quest;
	bool cash;
};

struct PetInfo {
	PetInfo() : norevive(false), nostoreincashshop(false), autoreact(false) { }
	bool norevive;
	bool nostoreincashshop;
	bool autoreact;
	int8_t evolevel;
	int32_t hunger;
	int32_t life;
	int32_t limitedlife;
	int32_t evoitem;
	string name;
};

struct PetInteractInfo {
	uint32_t prob;
	int16_t increase;
};

struct ItemRewardInfo {
	int32_t rewardid;
	uint16_t prob;
	int16_t quantity;
	string effect;
};

class ItemDataProvider {
public:
	static ItemDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new ItemDataProvider;
		return singleton;
	}
	void loadData();

	int32_t getCardId(int32_t mobid);
	int32_t getMobId(int32_t cardid);
	bool itemExists(int32_t id) { return (items.find(id) != items.end()); }
	bool petExists(int32_t itemid) { return (petsInfo.find(itemid) != petsInfo.end()); }
	bool consumeInfoExists(int32_t itemid) { return (consumes.find(itemid) != consumes.end()); }
	bool skillItemExists(int32_t itemid) { return (skills.find(itemid) != skills.end()); }
	bool summonBagExists(int32_t itemid) { return (mobs.find(itemid) != mobs.end()); }
	bool isTradeable(int32_t itemid) { return (!(items[itemid].notrade || items[itemid].quest)); }
	bool isCash(int32_t itemid) { return items[itemid].cash; }
	bool isQuest(int32_t itemid) { return items[itemid].quest; }
	uint16_t getMaxSlot(int32_t itemid) { return (itemExists(itemid) ? items[itemid].maxslot : 0); }
	int32_t getPrice(int32_t itemid) { return (itemExists(itemid) ? items[itemid].price : 0); }
	int32_t getMesoBonus(int32_t itemid) { return (itemExists(itemid) ? items[itemid].mesos : 0); }
	int32_t getHunger(int32_t itemid) { return (petExists(itemid) ? petsInfo[itemid].hunger : 0); }
	int32_t getItemNpc(int32_t itemid) { return (itemExists(itemid) ? items[itemid].npc : 0); }
	string getItemName(int32_t itemid) { return (itemExists(itemid) ? items[itemid].name : ""); }
	ItemRewardInfo * getRandomReward(int32_t itemid);

	void scrollItem(int32_t scrollid, Item *equip, int8_t &succeed, bool &cursed, bool wscroll);
	ConsumeInfo * getConsumeInfo(int32_t itemid) { return (consumeInfoExists(itemid) ? &consumes[itemid] : nullptr); }
	ItemInfo * getItemInfo(int32_t itemid) { return (itemExists(itemid) ? &items[itemid] : nullptr); }
	PetInfo * getPetInfo(int32_t itemid) { return &petsInfo[itemid]; }
	PetInteractInfo * getInteraction(int32_t itemid, int32_t action);
	vector<Skillbook> * getItemSkills(int32_t itemid) { return &skills[itemid]; }
	vector<SummonBag> * getItemSummons(int32_t itemid) { return &mobs[itemid]; }
private:
	ItemDataProvider() { }
	static ItemDataProvider *singleton;

	void loadItems();
	void loadScrolls();
	void loadConsumes();
	void loadMapRanges();
	void loadMultiMorphs();
	void loadMonsterCardData();
	void loadItemSkills();
	void loadSummonBags();
	void loadItemRewards();
	void loadPets();
	void loadPetInteractions();
	int16_t getStatVariance(int8_t mod);

	typedef bimap<int32_t, int32_t> card_map;
	typedef card_map::value_type card_info;

	unordered_map<int32_t, ItemInfo> items;
	unordered_map<int32_t, ScrollInfo> scrolls;
	unordered_map<int32_t, ConsumeInfo> consumes;
	unordered_map<int32_t, vector<SummonBag> > mobs;
	unordered_map<int32_t, vector<Skillbook> > skills;
	unordered_map<int32_t, vector<ItemRewardInfo> > itemRewards;
	unordered_map<int32_t, PetInfo> petsInfo;
	unordered_map<int32_t, unordered_map<int32_t, PetInteractInfo> > petsInteractInfo;
	card_map cards; // Left, cardid; right, mobid
};
