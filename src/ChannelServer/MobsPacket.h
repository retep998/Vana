/*
Copyright (C) 2008-2012 Vana Development Team

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
#include <vector>

using std::vector;

class Player;
class Mob;
struct Pos;
struct StatusInfo;

namespace MobsPacket {
	void spawnMob(Player *player, Mob *mob, int8_t summonEffect, Mob *owner = nullptr, bool spawn = false, bool show = false);
	void requestControl(Player *player, Mob *mob, bool spawn = false, Player *display = nullptr);
	void endControlMob(Player *player, Mob *mob);
	void moveMobResponse(Player *player, int32_t mobId, int16_t moveId, bool useSkill, int32_t mp, uint8_t skill = 0, uint8_t level = 0);
	void moveMob(Player *player, int32_t mobId, bool useSkill, int8_t skill, const Pos &projectileTarget, unsigned char *buf, int32_t len);
	void healMob(Mob *mob, int32_t amount);
	void hurtMob(Mob *mob, int32_t amount);
	void damageFriendlyMob(Mob *mob, int32_t damage);
	void applyStatus(Mob *mob, int32_t statusMask, const vector<StatusInfo> &info, int16_t delay, const vector<int32_t> &reflection);
	void removeStatus(Mob *mob, int32_t status);
	void showHp(Player *player, int32_t mobId, int8_t percentage);
	void showHp(int32_t mapId, int32_t mobId, int8_t percentage);
	void showBossHp(Mob *mob); // For major bosses
	void dieMob(Mob *mob, int8_t death = 1);
	void showSpawnEffect(int32_t mapId, int8_t summonEffect, const Pos &pos);
}