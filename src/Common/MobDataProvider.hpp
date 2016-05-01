/*
Copyright (C) 2008-2016 Vana Development Team

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

#include "GameConstants.hpp"
#include "MobAttackInfo.hpp"
#include "MobInfo.hpp"
#include "MobSkillInfo.hpp"
#include "Types.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Vana {
	class MobDataProvider {
	public:
		auto loadData() -> void;

		auto mobExists(mob_id_t mobId) const -> bool;
		auto getMobInfo(mob_id_t mobId) const -> ref_ptr_t<MobInfo>;
		auto getMobAttack(mob_id_t mobId, uint8_t index) const -> const MobAttackInfo * const;
		auto getMobSkill(mob_id_t mobId, uint8_t index) const -> const MobSkillInfo * const;
		auto getSkills(mob_id_t mobId) const -> const vector_t<MobSkillInfo> &;
	private:
		auto loadMobs() -> void;
		auto loadAttacks() -> void;
		auto loadSkills() -> void;
		auto loadSummons() -> void;

		hash_map_t<mob_id_t, ref_ptr_t<MobInfo>> m_mobInfo;
		hash_map_t<mob_id_t, vector_t<MobAttackInfo>> m_attacks;
		hash_map_t<mob_id_t, vector_t<MobSkillInfo>> m_skills;
	};
}