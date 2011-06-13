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

#include "ItemDataObjects.h"
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

	typedef bimap<int32_t, int32_t> CardMap;
	typedef CardMap::value_type CardInfo;

	unordered_map<int32_t, ItemInfo> m_itemInfo;
	unordered_map<int32_t, ScrollInfo> m_scrollInfo;
	unordered_map<int32_t, ConsumeInfo> m_consumeInfo;
	unordered_map<int32_t, vector<SummonBag>> m_summonBags;
	unordered_map<int32_t, vector<Skillbook>> m_skillbooks;
	unordered_map<int32_t, vector<ItemRewardInfo>> m_itemRewards;
	unordered_map<int32_t, PetInfo> m_petInfo;
	unordered_map<int32_t, unordered_map<int32_t, PetInteractInfo>> m_petInteractInfo;
	CardMap m_cards; // Left, card ID; right, mob ID
};