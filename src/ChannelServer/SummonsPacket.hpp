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
	class Summon;
	struct Point;

	namespace Packets {
		SPLIT_PACKET(showSummon, player_id_t playerId, Summon *summon, bool isMapEntry = true);
		SPLIT_PACKET(moveSummon, player_id_t playerId, Summon *summon, const Point &startPos, unsigned char *buf, int32_t bufLen);
		SPLIT_PACKET(removeSummon, player_id_t playerId, Summon *summon, int8_t message);
		SPLIT_PACKET(damageSummon, player_id_t playerId, summon_id_t summonId, int8_t unk, damage_t damage, map_object_t mobId);
		PACKET(summonSkill, player_id_t playerId, skill_id_t skillId, uint8_t display, skill_level_t level);
		SPLIT_PACKET(summonSkillEffect, player_id_t playerId, skill_id_t skillId, uint8_t display, skill_level_t level);
	}
}