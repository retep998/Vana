/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "ItemDataObjects.hpp"
#include "Types.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class Item;

class ItemDataProvider {
	SINGLETON(ItemDataProvider);
public:
	auto loadData() -> void;

	auto getCardId(int32_t mobId) -> int32_t;
	auto getMobId(int32_t cardId) -> int32_t;
	auto itemExists(int32_t id) -> bool { return m_itemInfo.find(id) != std::end(m_itemInfo); }
	auto petExists(int32_t itemId) -> bool { return m_petInfo.find(itemId) != std::end(m_petInfo); }
	auto consumeInfoExists(int32_t itemId) -> bool { return m_consumeInfo.find(itemId) != std::end(m_consumeInfo); }
	auto skillItemExists(int32_t itemId) -> bool { return m_skillbooks.find(itemId) != std::end(m_skillbooks); }
	auto summonBagExists(int32_t itemId) -> bool { return m_summonBags.find(itemId) != std::end(m_summonBags); }
	auto isTradeable(int32_t itemId) -> bool { return !(m_itemInfo[itemId].noTrade || m_itemInfo[itemId].quest); }
	auto isCash(int32_t itemId) -> bool { return m_itemInfo[itemId].cash; }
	auto isQuest(int32_t itemId) -> bool { return m_itemInfo[itemId].quest; }
	auto canKarma(int32_t itemId) -> bool { return m_itemInfo[itemId].karmaScissors; }
	auto getMaxSlot(int32_t itemId) -> uint16_t { return itemExists(itemId) ? m_itemInfo[itemId].maxSlot : 0; }
	auto getPrice(int32_t itemId) -> int32_t { return itemExists(itemId) ? m_itemInfo[itemId].price : 0; }
	auto getMesoBonus(int32_t itemId) -> int32_t { return itemExists(itemId) ? m_itemInfo[itemId].mesos : 0; }
	auto getHunger(int32_t itemId) -> int32_t { return petExists(itemId) ? m_petInfo[itemId].hunger : 0; }
	auto getItemNpc(int32_t itemId) -> int32_t { return itemExists(itemId) ? m_itemInfo[itemId].npc : 0; }
	auto getItemName(int32_t itemId) -> string_t { return itemExists(itemId) ? m_itemInfo[itemId].name : ""; }
	auto getRandomReward(int32_t itemId) -> ItemRewardInfo *;

	auto scrollItem(int32_t scrollId, Item *equip, bool whiteScroll, bool gmScroller, int8_t &succeed, bool &cursed) -> void;
	auto getItemInfo(int32_t itemId) -> ItemInfo * { return &m_itemInfo[itemId]; }
	auto getConsumeInfo(int32_t itemId) -> ConsumeInfo * { return consumeInfoExists(itemId) ? &m_consumeInfo[itemId] : nullptr; }
	auto getPetInfo(int32_t itemId) -> PetInfo * { return &m_petInfo[itemId]; }
	auto getInteraction(int32_t itemId, int32_t action) -> PetInteractInfo *;
	auto getItemSkills(int32_t itemId) -> vector_t<Skillbook> * { return &m_skillbooks[itemId]; }
	auto getItemSummons(int32_t itemId) -> vector_t<SummonBag> * { return &m_summonBags[itemId]; }
private:
	auto loadItems() -> void;
	auto loadScrolls() -> void;
	auto loadConsumes() -> void;
	auto loadMapRanges() -> void;
	auto loadMultiMorphs() -> void;
	auto loadMonsterCardData() -> void;
	auto loadItemSkills() -> void;
	auto loadSummonBags() -> void;
	auto loadItemRewards() -> void;
	auto loadPets() -> void;
	auto loadPetInteractions() -> void;

	hash_map_t<int32_t, ItemInfo> m_itemInfo;
	hash_map_t<int32_t, ScrollInfo> m_scrollInfo;
	hash_map_t<int32_t, ConsumeInfo> m_consumeInfo;
	hash_map_t<int32_t, vector_t<SummonBag>> m_summonBags;
	hash_map_t<int32_t, vector_t<Skillbook>> m_skillbooks;
	hash_map_t<int32_t, vector_t<ItemRewardInfo>> m_itemRewards;
	hash_map_t<int32_t, PetInfo> m_petInfo;
	hash_map_t<int32_t, hash_map_t<int32_t, PetInteractInfo>> m_petInteractInfo;
	hash_map_t<int32_t, int32_t> m_cardsToMobs;
	hash_map_t<int32_t, int32_t> m_mobsToCards;
};