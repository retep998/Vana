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

#include "Item.hpp"
#include "ItemDataObjects.hpp"
#include "Types.hpp"
#include <unordered_map>

class EquipDataProvider {
	SINGLETON(EquipDataProvider);
public:
	auto loadData() -> void;

	auto setEquipStats(Item *equip, bool random, bool isGm) -> void;
	auto canEquip(int32_t itemId, int16_t job, int16_t str, int16_t dex, int16_t intt, int16_t luk, int16_t fame) -> bool;
	auto validSlot(int32_t equipId, int16_t target) -> bool;
	auto getSlots(int32_t equipId) -> int8_t { return getEquipInfo(equipId).slots; }
	auto getEquipInfo(int32_t equipId) -> const EquipInfo & { return m_equipInfo[equipId]; }
private:
	auto loadEquips() -> void;

	hash_map_t<int32_t, EquipInfo> m_equipInfo;
};