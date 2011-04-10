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
using std::tr1::unordered_map;
using std::vector;
using std::string;

class Item;

struct SummonBag {
	int32_t mobId;
	uint32_t chance;
};

struct Skillbook {
	int32_t skillId;
	uint8_t reqlevel;
	uint8_t maxLevel;
	int8_t chance;
};

struct ScrollInfo {
	ScrollInfo() : randStat(false), recover(false), warmSupport(false), preventSlip(false), ihand(0) { }
	bool warmSupport;
	bool randStat;
	bool recover;
	bool preventSlip;
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
	int32_t startMap;
	int32_t endMap;
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
		fireResist(0), iceResist(0), lightningResist(0), poisonResist(0),
		curseDef(0), stunDef(0), weaknessDef(0), darknessDef(0), sealDef(0),
		dropUpitem(0), dropUpItemRange(0),
		ailment(0),
		autoConsume(false), ignoreWdef(false), ignoreMdef(false), party(false), mouseCancel(true),
		ignoreContinent(false), ghost(false), barrier(false), overrideTraction(false), preventDrown(false),
		preventFreeze(false), mesoUp(false), partyDropUp(false), dropUp(false)
		{ }

	bool autoConsume;
	bool ignoreWdef;
	bool ignoreMdef;
	bool party;
	bool mouseCancel;
	bool ignoreContinent;
	bool ghost;
	bool barrier;
	bool overrideTraction;
	bool preventDrown;
	bool preventFreeze;
	bool mesoUp;
	bool dropUp;
	bool partyDropUp;
	uint8_t effect;
	uint8_t decHunger;
	uint8_t decFatigue;
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
	int16_t fireResist;
	int16_t iceResist;
	int16_t lightningResist;
	int16_t poisonResist;
	int16_t curseDef;
	int16_t stunDef;
	int16_t weaknessDef;
	int16_t darknessDef;
	int16_t sealDef;
	int16_t dropUpItemRange;
	uint16_t mcProb;
	int32_t dropUpitem;
	int32_t moveTo;
	int32_t time;
	int32_t ailment;
	vector<Morph> morphs;
	vector<CardMapRange> mapRanges;
};

struct ItemInfo {
	ItemInfo() :
		timeLimited(false),
		noSale(false),
		karmaScissors(false),
		expireOnLogout(false),
		blockPickup(false),
		noTrade(false),
		cash(false),
		quest(false)
		{ }

	string name;
	uint8_t minLevel;
	uint8_t maxLevel;
	uint8_t makerLevel;
	uint16_t maxSlot;
	int32_t price;
	int32_t maxObtainable;
	int32_t exp;
	int32_t npc;
	int32_t mesos;
	bool timeLimited;
	bool noSale;
	bool karmaScissors;
	bool expireOnLogout;
	bool blockPickup;
	bool noTrade;
	bool quest;
	bool cash;
};

struct PetInfo {
	PetInfo() : noRevive(false), noStoringInCashShop(false), autoReact(false) { }
	bool noRevive;
	bool noStoringInCashShop;
	bool autoReact;
	int8_t evolveLevel;
	int32_t hunger;
	int32_t life;
	int32_t limitedLife;
	int32_t evolveItem;
	string name;
};

struct PetInteractInfo {
	uint32_t prob;
	int16_t increase;
};

struct ItemRewardInfo {
	int32_t rewardId;
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

	int32_t getCardId(int32_t mobId);
	int32_t getMobId(int32_t cardId);
	bool itemExists(int32_t id) { return (m_itemInfo.find(id) != m_itemInfo.end()); }
	bool petExists(int32_t itemId) { return (m_petInfo.find(itemId) != m_petInfo.end()); }
	bool consumeInfoExists(int32_t itemId) { return (m_consumeInfo.find(itemId) != m_consumeInfo.end()); }
	bool skillItemExists(int32_t itemId) { return (m_skillbooks.find(itemId) != m_skillbooks.end()); }
	bool summonBagExists(int32_t itemId) { return (m_summonBags.find(itemId) != m_summonBags.end()); }
	bool isTradeable(int32_t itemId) { return (!(m_itemInfo[itemId].noTrade || m_itemInfo[itemId].quest)); }
	bool isCash(int32_t itemId) { return m_itemInfo[itemId].cash; }
	bool isQuest(int32_t itemId) { return m_itemInfo[itemId].quest; }
	bool canKarma(int32_t itemId) { return m_itemInfo[itemId].karmaScissors; }
	uint16_t getMaxSlot(int32_t itemId) { return (itemExists(itemId) ? m_itemInfo[itemId].maxSlot : 0); }
	int32_t getPrice(int32_t itemId) { return (itemExists(itemId) ? m_itemInfo[itemId].price : 0); }
	int32_t getMesoBonus(int32_t itemId) { return (itemExists(itemId) ? m_itemInfo[itemId].mesos : 0); }
	int32_t getHunger(int32_t itemId) { return (petExists(itemId) ? m_petInfo[itemId].hunger : 0); }
	int32_t getItemNpc(int32_t itemId) { return (itemExists(itemId) ? m_itemInfo[itemId].npc : 0); }
	string getItemName(int32_t itemId) { return (itemExists(itemId) ? m_itemInfo[itemId].name : ""); }
	ItemRewardInfo * getRandomReward(int32_t itemId);

	void scrollItem(int32_t scrollId, Item *equip, int8_t &succeed, bool &cursed, bool wscroll);
	ItemInfo * getItemInfo(int32_t itemId) { return &m_itemInfo[itemId]; }
	ConsumeInfo * getConsumeInfo(int32_t itemId) { return (consumeInfoExists(itemId) ? &m_consumeInfo[itemId] : nullptr); }
	PetInfo * getPetInfo(int32_t itemId) { return &m_petInfo[itemId]; }
	PetInteractInfo * getInteraction(int32_t itemId, int32_t action);
	vector<Skillbook> * getItemSkills(int32_t itemId) { return &m_skillbooks[itemId]; }
	vector<SummonBag> * getItemSummons(int32_t itemId) { return &m_summonBags[itemId]; }
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

	typedef bimap<int32_t, int32_t> cardMap;
	typedef cardMap::value_type cardInfo;

	unordered_map<int32_t, ItemInfo> m_itemInfo;
	unordered_map<int32_t, ScrollInfo> m_scrollInfo;
	unordered_map<int32_t, ConsumeInfo> m_consumeInfo;
	unordered_map<int32_t, vector<SummonBag>> m_summonBags;
	unordered_map<int32_t, vector<Skillbook>> m_skillbooks;
	unordered_map<int32_t, vector<ItemRewardInfo>> m_itemRewards;
	unordered_map<int32_t, PetInfo> m_petInfo;
	unordered_map<int32_t, unordered_map<int32_t, PetInteractInfo>> m_petInteractInfo;
	cardMap m_cards; // Left, card ID; right, mob ID
};