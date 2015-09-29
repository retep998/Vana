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
			auto addSkill(Player *player, PacketReader &reader) -> void;
			auto cancelSkill(Player *player, PacketReader &reader) -> void;
			auto useSkill(Player *player, PacketReader &reader) -> void;
			auto getAffectedPartyMembers(Party *party, int8_t affected, int8_t members) -> const vector_t<Player *>;
			auto applySkillCosts(Player *player, skill_id_t skillId, skill_level_t level, bool elementalAmp = false) -> Result;
			auto useAttackSkill(Player *player, skill_id_t skillId) -> Result;
			auto useAttackSkillRanged(Player *player, skill_id_t skillId, inventory_slot_t projectilePos, inventory_slot_t cashProjectilePos, item_id_t projectileId) -> Result;
			auto heal(Player *player, int64_t value, const BuffSource &source) -> void;
			auto hurt(Player *player, int64_t value, const BuffSource &source) -> void;
			auto stopSkill(Player *player, const BuffSource &source, bool fromTimer = false) -> void;
			auto startCooldown(Player *player, skill_id_t skillId, seconds_t coolTime, bool initialLoad = false) -> void;
			auto stopCooldown(Player *player, skill_id_t skillId) -> void;
			auto isCooling(Player *player, skill_id_t skillId) -> bool;
			auto getCooldownTimeLeft(Player *player, skill_id_t skillId) -> int16_t;
		}
	}
}