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
#include <vector>

class Mob;
class Player;
struct Pos;
struct StatusInfo;

namespace MobsPacket {
	auto spawnMob(Player *player, ref_ptr_t<Mob> mob, int8_t summonEffect, ref_ptr_t<Mob> owner = nullptr, bool spawn = false, bool show = false) -> void;
	auto requestControl(Player *player, ref_ptr_t<Mob> mob, bool spawn = false, Player *display = nullptr) -> void;
	auto endControlMob(Player *player, int32_t mapId, int32_t mapMobId) -> void;
	auto moveMobResponse(Player *player, int32_t mapMobId, int16_t moveId, bool skillPossible, int32_t mp, uint8_t skill, uint8_t level) -> void;
	auto moveMob(Player *player, int32_t mapMobId, bool skillPossible, int8_t rawAction, uint8_t skill, uint8_t level, int16_t option, unsigned char *buf, int32_t len) -> void;
	auto healMob(int32_t mapId, int32_t mapMobId, int32_t amount) -> void;
	auto hurtMob(int32_t mapId, int32_t mapMobId, int32_t amount) -> void;
	auto damageFriendlyMob(ref_ptr_t<Mob> mob, int32_t damage) -> void;
	auto applyStatus(int32_t mapId, int32_t mapMobId, int32_t statusMask, const vector_t<StatusInfo> &info, int16_t delay, const vector_t<int32_t> &reflection) -> void;
	auto removeStatus(int32_t mapId, int32_t mapMobId, int32_t status) -> void;
	auto showHp(Player *player, int32_t mapMobId, int8_t percentage) -> void;
	auto showHp(int32_t mapId, int32_t mapMobId, int8_t percentage) -> void;
	auto showBossHp(ref_ptr_t<Mob> mob) -> void; // For major bosses
	auto dieMob(int32_t mapId, int32_t mapMobId, int8_t death = 1) -> void;
	auto showSpawnEffect(int32_t mapId, int8_t summonEffect, const Pos &pos) -> void;
}