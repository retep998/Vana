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

#include "Common/Types.hpp"
#include <vector>

namespace Vana {
	class BuffSource;
	class PacketReader;
	struct MobSkillLevelInfo;
	struct SkillLevelInfo;

	namespace ChannelServer {
		class Party;
		class Player;

		namespace Skills {
			auto addSkill(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto cancelSkill(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto useSkill(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto getAffectedPartyMembers(Party *party, int8_t affected, int8_t members) -> const vector_t<ref_ptr_t<Player>>;
			auto applySkillCosts(ref_ptr_t<Player> player, skill_id_t skillId, skill_level_t level, bool elementalAmp = false) -> Result;
			auto useAttackSkill(ref_ptr_t<Player> player, skill_id_t skillId) -> Result;
			auto useAttackSkillRanged(ref_ptr_t<Player> player, skill_id_t skillId, inventory_slot_t projectilePos, inventory_slot_t cashProjectilePos, item_id_t projectileId) -> Result;
			auto heal(ref_ptr_t<Player> player, int64_t value, const BuffSource &source) -> void;
			auto hurt(ref_ptr_t<Player> player, int64_t value, const BuffSource &source) -> void;
			auto stopSkill(ref_ptr_t<Player> player, const BuffSource &source, bool fromTimer = false) -> void;
			auto startCooldown(ref_ptr_t<Player> player, skill_id_t skillId, seconds_t coolTime, bool initialLoad = false) -> void;
			auto stopCooldown(ref_ptr_t<Player> player, skill_id_t skillId) -> void;
			auto isCooling(ref_ptr_t<Player> player, skill_id_t skillId) -> bool;
			auto getCooldownTimeLeft(ref_ptr_t<Player> player, skill_id_t skillId) -> int16_t;
		}
	}
}