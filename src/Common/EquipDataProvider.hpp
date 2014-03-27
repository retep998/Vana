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
#include "ItemConstants.hpp"
#include "ItemDataObjects.hpp"
#include "Types.hpp"
#include <unordered_map>

class EquipDataProvider {
public:
	auto loadData() -> void;

	auto setEquipStats(Item *equip, Items::StatVariance variancePolicy, bool isGm, bool isItemInitialization) const -> void;
	auto canEquip(item_id_t itemId, gender_id_t gender, job_id_t job, int16_t str, int16_t dex, int16_t intt, int16_t luk, fame_t fame) const -> bool;
	auto isValidSlot(item_id_t equipId, inventory_slot_t target) const -> bool;
	auto getSlots(item_id_t equipId) const -> int8_t;
	auto getEquipInfo(item_id_t equipId) const -> const EquipInfo &;
private:
	auto loadEquips() -> void;

	hash_map_t<item_id_t, EquipInfo> m_equipInfo;
};