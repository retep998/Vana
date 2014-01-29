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

	auto getCardId(int32_t mobId) const -> int32_t;
	auto getMobId(int32_t cardId) const -> int32_t;
	auto scrollItem(int32_t scrollId, Item *equip, bool whiteScroll, bool gmScroller, int8_t &succeed, bool &cursed) const -> void;
	auto getItemInfo(int32_t itemId) const -> const ItemInfo * const;
	auto getConsumeInfo(int32_t itemId) const -> const ConsumeInfo * const;
	auto getPetInfo(int32_t itemId) const -> const PetInfo * const;
	auto getInteraction(int32_t itemId, int32_t action) const -> const PetInteractInfo * const;
	auto getItemSkills(int32_t itemId) const -> const vector_t<Skillbook> * const;
	auto getItemRewards(int32_t itemId) const -> const vector_t<ItemRewardInfo> * const;
	auto getItemSummons(int32_t itemId) const -> const vector_t<SummonBag> * const;
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