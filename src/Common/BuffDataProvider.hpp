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

#include "SkillConstants.hpp"
#include "Types.hpp"
#include <unordered_map>
#include <vector>

struct ConsumeInfo;

struct Buff {
	uint8_t type = 0;
	int8_t byte = 0;
	int8_t value = 0;
};

struct BuffInfo {
	bool hasMapVal = false;
	bool hasMapEntry = false;
	bool useVal = false;
	int16_t itemVal = 0;
	Buff buff;
};

struct BuffMapInfo {
	bool useVal = false;
	Buff buff;
};

struct BuffAct {
	int8_t value = 0;
	int32_t time = 0;
	Action type;
};

struct SkillInfo {
	bool hasAction = false;
	BuffAct act;
	vector_t<BuffInfo> player;
	vector_t<BuffMapInfo> map;
};

struct MobAilmentInfo {
	int16_t delay = 0;
	vector_t<BuffInfo> mob;
};

class BuffDataProvider {
	SINGLETON(BuffDataProvider);
public:
	auto loadData() -> void;
	auto addItemInfo(item_id_t itemId, const ConsumeInfo &cons) -> void;

	auto isBuff(skill_id_t skillId) const -> bool;
	auto isDebuff(mob_skill_id_t skillId) const -> bool;
	auto getSkillInfo(skill_id_t skillId) const -> const SkillInfo &;
	auto getMobSkillInfo(mob_skill_id_t skillId) const -> const MobAilmentInfo &;
private:
	hash_map_t<skill_id_t, SkillInfo> m_skillInfo;
	hash_map_t<mob_skill_id_t, MobAilmentInfo> m_mobSkillInfo;
};