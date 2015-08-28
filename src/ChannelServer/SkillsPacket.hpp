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

#include "PacketBuilder.hpp"
#include "SplitPacketBuilder.hpp"
#include "Types.hpp"

namespace Vana {
	class Player;
	struct ChargeOrStationarySkillInfo;
	struct PlayerSkillInfo;

	namespace Packets {
		namespace Skills {
			PACKET(addSkill, skill_id_t skillId, const PlayerSkillInfo &skillInfo);
			SPLIT_PACKET(showSkill, player_id_t playerId, skill_id_t skillId, skill_level_t level, uint8_t direction, bool party = false, bool self = false);
			PACKET(healHp, health_t hp);
			SPLIT_PACKET(showSkillEffect, player_id_t playerId, skill_id_t skillId);
			SPLIT_PACKET(showChargeOrStationarySkill, player_id_t playerId, const ChargeOrStationarySkillInfo &info);
			SPLIT_PACKET(endChargeOrStationarySkill, player_id_t playerId, const ChargeOrStationarySkillInfo &info);
			SPLIT_PACKET(showMagnetSuccess, map_object_t mapMobId, uint8_t success);
			PACKET(sendCooldown, skill_id_t skillId, seconds_t time);
			SPLIT_PACKET(showBerserk, player_id_t playerId, skill_level_t level, bool on);
		}
	}
}