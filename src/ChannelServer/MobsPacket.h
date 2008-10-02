/*
Copyright (C) 2008 Vana Development Team

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
#ifndef MOBSPACK_H
#define MOBSPACK_H

#include "Types.h"

class Player;
class Mob;
class ReadPacket;
struct MobHPInfo;

namespace MobsPacket {
	void spawnMob(Player *player, Mob *mob, bool requestControl, bool spawn = false, bool show = false);
	void endControlMob(Player *player, Mob *mob);
	void moveMobResponse(Player *player, int32_t mobid, int16_t moveid, bool useskill, int32_t mp);
	void moveMob(Player *player, int32_t mobid, bool useskill, int32_t skill, unsigned char *buf, int32_t len);
	void showHP(Player *player, int32_t mobid, int8_t per, bool miniboss);
	void showBossHP(Player *player, const MobHPInfo &mob); // For major bosses
	void dieMob(Mob *mob);
	void damageMob(Player *player, ReadPacket *pack);
	void damageMobRanged(Player *player, ReadPacket *pack);
	void damageMobSpell(Player *player, ReadPacket *pack);
	void damageMobSummon(Player *player, ReadPacket *pack);
};

#endif
