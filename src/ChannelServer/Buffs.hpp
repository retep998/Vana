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

#include "Types.hpp"
#include "BuffDataProvider.hpp"
#include <array>
#include <vector>

class Player;

struct ActiveBuff {
	ActiveBuff()
	{
		for (size_t i = 0; i < BuffBytes::ByteQuantity; i++) {
			types[i] = 0;
		}
	}

	bool hasMapBuff = false;
	array_t<uint8_t, BuffBytes::ByteQuantity> types;
	vector_t<int16_t> vals;
};

struct ActiveMapBuff {
	ActiveMapBuff()
	{
		for (int8_t i = 0; i < BuffBytes::ByteQuantity; i++) {
			typeList[i] = 0;
		}
	}

	bool debuff = false;
	array_t<uint8_t, BuffBytes::ByteQuantity> typeList;
	vector_t<uint8_t> bytes;
	vector_t<int8_t> types;
	vector_t<int16_t> values;
	vector_t<bool> useVals;
};

struct MapEntryVals {
	bool use = false;
	bool debuff = false;
	int16_t val = 0;
	int16_t skill = 0;
};

namespace Buffs {
	auto addBuff(Player *player, item_id_t itemId, const seconds_t &time) -> void;
	auto addBuff(Player *player, skill_id_t skillId, skill_level_t level, int16_t addedInfo, map_object_t mapMobId = 0) -> Result;
	auto endBuff(Player *player, skill_id_t skill) -> void;
	auto doAction(Player *player, skill_id_t skillId, skill_level_t level) -> void;
	auto addDebuff(Player *player, mob_skill_id_t skillId, mob_skill_level_t level) -> void;
	auto endDebuff(Player *player, mob_skill_id_t skillId) -> void;
	auto buffMayApply(skill_id_t skillId, skill_level_t level, int8_t buffValue) -> bool;

	auto parseBuffInfo(Player *player, skill_id_t skillId, skill_level_t level) -> ActiveBuff;
	auto parseBuffMapInfo(Player *player, skill_id_t skillId, skill_level_t level) -> ActiveMapBuff;
	auto parseBuffs(int32_t skillId, mob_skill_id_t level) -> vector_t<Buff>;
	auto parseMobBuffInfo(Player *player, mob_skill_id_t skillId, mob_skill_level_t level) -> ActiveBuff;
	auto parseMobBuffMapInfo(Player *player, mob_skill_id_t skillId, mob_skill_level_t level) -> ActiveMapBuff;
	auto parseMobBuffs(uint8_t skillId) -> vector_t<Buff>;
	auto parseBuffMapEntryInfo(Player *player, skill_id_t skillId, skill_level_t level) -> ActiveMapBuff;
	auto parseMobBuffMapEntryInfo(Player *player, mob_skill_id_t skillId, mob_skill_level_t level) -> ActiveMapBuff;
	auto getValue(int8_t value, skill_id_t skillId, skill_level_t level) -> int16_t;
	auto getMobSkillValue(int8_t value, mob_skill_id_t skillId, mob_skill_level_t level) -> int16_t;
	auto parseMountInfo(Player *player, skill_id_t skillId, skill_level_t level) -> skill_id_t;
}