/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Buff.hpp"
#include "BuffInfo.hpp"
#include "BuffInfoByEffect.hpp"
#include "Types.hpp"
#include <unordered_map>
#include <vector>

class BuffSource;
struct ConsumeInfo;

class BuffDataProvider {
public:
	auto loadData() -> void;
	auto addItemInfo(item_id_t itemId, const ConsumeInfo &cons) -> void;

	auto isBuff(const BuffSource &source) const -> bool;
	auto isDebuff(const BuffSource &source) const -> bool;
	auto getInfo(const BuffSource &source) const -> const Buff &;
	auto getBuffsByEffect() const -> const BuffInfoByEffect &;
private:
	auto processSkills(Buff value, const init_list_t<skill_id_t> &skills) -> void;
	hash_map_t<skill_id_t, Buff> m_buffs;
	hash_map_t<item_id_t, Buff> m_items;
	hash_map_t<mob_skill_id_t, Buff> m_mobSkillInfo;
	BuffInfoByEffect m_basics;
};