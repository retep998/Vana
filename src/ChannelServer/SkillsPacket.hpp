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

#include "PacketBuilder.hpp"
#include "SplitPacketBuilder.hpp"
#include "Types.hpp"

class Player;
struct ChargeOrStationarySkillInfo;
struct PlayerSkillInfo;

namespace SkillsPacket {
	PACKET(addSkill, int32_t skillId, const PlayerSkillInfo &skillInfo);
	SPLIT_PACKET(showSkill, int32_t playerId, int32_t skillId, uint8_t level, uint8_t direction, bool party = false, bool self = false);
	PACKET(healHp, int16_t hp);
	SPLIT_PACKET(showSkillEffect, int32_t playerId, int32_t skillId);
	SPLIT_PACKET(showChargeOrStationarySkill, int32_t playerId, const ChargeOrStationarySkillInfo &info);
	SPLIT_PACKET(endChargeOrStationarySkill, int32_t playerId, const ChargeOrStationarySkillInfo &info);
	SPLIT_PACKET(showMagnetSuccess, int32_t mapMobId, uint8_t success);
	PACKET(sendCooldown, int32_t skillId, int16_t time);
	SPLIT_PACKET(showBerserk, int32_t playerId, uint8_t level, bool on);
}