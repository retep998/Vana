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
#include <unordered_map>

namespace Vana {
	class PacketBuilder;
	enum class MysticDoorResult;
	struct SkillLevelInfo;

	namespace ChannelServer {
		class MysticDoor;
		class Party;
		class Player;

		struct PlayerSkillInfo {
			skill_level_t level = 0;
			skill_level_t maxSkillLevel = 0;
			skill_level_t playerMaxSkillLevel = 0;
		};

		class PlayerSkills {
			NONCOPYABLE(PlayerSkills);
			NO_DEFAULT_CONSTRUCTOR(PlayerSkills);
		public:
			PlayerSkills(Player *player);

			auto load() -> void;
			auto save(bool saveCooldowns = false) -> void;
			auto connectPacket(PacketBuilder &builder) const -> void;
			auto connectPacketForBlessing(PacketBuilder &builder) const -> void;

			auto addSkillLevel(skill_id_t skillId, skill_level_t amount, bool sendPacket = true) -> bool;
			auto getSkillLevel(skill_id_t skillId) const -> skill_level_t;
			auto getMaxSkillLevel(skill_id_t skillId) const -> skill_level_t;
			auto setMaxSkillLevel(skill_id_t skillId, skill_level_t maxLevel, bool sendPacket = true) -> void;
			auto getSkillInfo(skill_id_t skillId) const -> const SkillLevelInfo * const;

			auto hasElementalAmp() const -> bool;
			auto hasEnergyCharge() const -> bool;
			auto hasHpIncrease() const -> bool;
			auto hasMpIncrease() const -> bool;
			auto hasVenomousWeapon() const -> bool;
			auto hasAchilles() const -> bool;
			auto hasDarkSightInterruptionSkill() const -> bool;
			auto hasNoDamageSkill() const -> bool;
			auto hasFollowTheLead() const -> bool;
			auto hasLegendarySpirit() const -> bool;
			auto hasMaker() const -> bool;
			auto hasBlessingOfTheFairy() const -> bool;
			auto getElementalAmp() const -> skill_id_t;
			auto getEnergyCharge() const -> skill_id_t;
			auto getAdvancedCombo() const -> skill_id_t;
			auto getAlchemist() const -> skill_id_t;
			auto getHpIncrease() const -> skill_id_t;
			auto getMpIncrease() const -> skill_id_t;
			auto getMastery() const -> skill_id_t;
			auto getMpEater() const -> skill_id_t;
			auto getVenomousWeapon() const -> skill_id_t;
			auto getAchilles() const -> skill_id_t;
			auto getDarkSightInterruptionSkill() const -> skill_id_t;
			auto getNoDamageSkill() const -> skill_id_t;
			auto getFollowTheLead() const -> skill_id_t;
			auto getLegendarySpirit() const -> skill_id_t;
			auto getMaker() const -> skill_id_t;
			auto getBlessingOfTheFairy() const -> skill_id_t;
			auto getRechargeableBonus() const -> slot_qty_t;

			auto addCooldown(skill_id_t skillId, seconds_t time) -> void;
			auto removeCooldown(skill_id_t skillId) -> void;
			auto removeAllCooldowns() -> void;

			auto openMysticDoor(const Point &pos, seconds_t doorTime) -> MysticDoorResult;
			auto closeMysticDoor(bool fromTimer) -> void;
			auto getMysticDoor() const -> ref_ptr_t<MysticDoor>;
			auto onJoinParty(Party *party, Player *player) -> void;
			auto onLeaveParty(Party *party, Player *player, bool kicked) -> void;
			auto onPartyDisband(Party *party) -> void;
			auto onMapChange() const -> void;
			auto onDisconnect() -> void;
		private:
			auto hasSkill(skill_id_t skillId) const -> bool;

			Player *m_player = nullptr;
			hash_map_t<skill_id_t, PlayerSkillInfo> m_skills;
			hash_map_t<skill_id_t, seconds_t> m_cooldowns;
			ref_ptr_t<MysticDoor> m_mysticDoor;
			string_t m_blessingPlayer;
		};
	}
}