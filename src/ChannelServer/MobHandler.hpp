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

#include "Types.hpp"

class Mob;
class PacketReader;
class Player;
struct MobSkillLevelInfo;

namespace MobHandler {
	auto handleMobStatus(int32_t playerId, ref_ptr_t<Mob> mob, int32_t skillId, uint8_t level, int32_t weapon, int8_t hits, int32_t damage = 0) -> int32_t;
	auto handleBomb(Player *player, PacketReader &reader) -> void;
	auto monsterControl(Player *player, PacketReader &reader) -> void;
	auto friendlyDamaged(Player *player, PacketReader &reader) -> void;
	auto handleTurncoats(Player *player, PacketReader &reader) -> void;
}