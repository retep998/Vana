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
struct ActiveBuff;
struct ActiveMapBuff;

namespace BuffsPacket {
	SPLIT_PACKET(useSkill, int32_t playerId, int32_t skillId, const seconds_t &time, const ActiveBuff &playerSkill, const ActiveMapBuff &mapSkill, int16_t addedInfo);
	SPLIT_PACKET(endSkill, int32_t playerId, const ActiveBuff &playerSkill);
	SPLIT_PACKET(giveDebuff, int32_t playerId, uint8_t skillId, uint8_t level, const seconds_t &time, int16_t delay, const ActiveBuff &playerSkill, const ActiveMapBuff &mapSkill);
	SPLIT_PACKET(usePirateBuff, int32_t playerId, int32_t skillId, const seconds_t &time, const ActiveBuff &playerSkill, const ActiveMapBuff &mapSkill);
	SPLIT_PACKET(useSpeedInfusion, int32_t playerId, int32_t skillId, const seconds_t &time, const ActiveBuff &playerSkill, const ActiveMapBuff &mapSkill, int16_t addedInfo);
	SPLIT_PACKET(useMount, int32_t playerId, int32_t skillId, const seconds_t &time, const ActiveBuff &playerSkill, const ActiveMapBuff &mapSkill, int16_t addedInfo, int32_t mountId);
	PACKET(useHomingBeacon, int32_t skillId, const ActiveBuff &playerSkill, int32_t mapMobId);
}