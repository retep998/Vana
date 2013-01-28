/*
Copyright (C) 2008-2013 Vana Development Team

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

#include "Types.h"

class Mob;
class PacketReader;
class Player;
struct MobSkillLevelInfo;

namespace MobHandler {
	int32_t handleMobStatus(int32_t playerId, Mob *mob, int32_t skillId, uint8_t level, int32_t weapon, int8_t hits, int32_t damage = 0);
	void handleMobSkill(Mob *mob, uint8_t skillId, uint8_t level, MobSkillLevelInfo *skillInfo);
	void handleBomb(Player *player, PacketReader &packet);
	void monsterControl(Player *player, PacketReader &packet);
	void friendlyDamaged(Player *player, PacketReader &packet);
	void handleTurncoats(Player *player, PacketReader &packet);
}