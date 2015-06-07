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

class Player;
struct ActiveBuff;
struct ActiveMapBuff;

namespace BuffsPacket {
	SPLIT_PACKET(useSkill, player_id_t playerId, skill_id_t skillId, const seconds_t &time, const ActiveBuff &playerSkill, const ActiveMapBuff &mapSkill, int16_t addedInfo);
	SPLIT_PACKET(endSkill, player_id_t playerId, const ActiveBuff &playerSkill);
	SPLIT_PACKET(giveDebuff, player_id_t playerId, mob_skill_id_t skillId, mob_skill_level_t level, const seconds_t &time, int16_t delay, const ActiveBuff &playerSkill, const ActiveMapBuff &mapSkill);
	SPLIT_PACKET(usePirateBuff, player_id_t playerId, skill_id_t skillId, const seconds_t &time, const ActiveBuff &playerSkill, const ActiveMapBuff &mapSkill);
	SPLIT_PACKET(useSpeedInfusion, player_id_t playerId, skill_id_t skillId, const seconds_t &time, const ActiveBuff &playerSkill, const ActiveMapBuff &mapSkill, int16_t addedInfo);
	SPLIT_PACKET(useMount, player_id_t playerId, skill_id_t skillId, const seconds_t &time, const ActiveBuff &playerSkill, const ActiveMapBuff &mapSkill, int16_t addedInfo, item_id_t mountId);
	PACKET(useHomingBeacon, skill_id_t skillId, const ActiveBuff &playerSkill, map_object_t mapMobId);
}