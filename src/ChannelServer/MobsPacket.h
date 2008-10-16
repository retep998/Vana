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
#include <vector>

using std::vector;

class Player;
class Mob;
class PacketReader;
struct MobHPInfo;

namespace MobsPacket {
	void spawnMob(Player *player, Mob *mob, bool requestControl, bool spawn = false, bool show = false);
	void endControlMob(Player *player, Mob *mob);
	void moveMobResponse(Player *player, int32_t mobid, int16_t moveid, bool useskill, int32_t mp);
	void moveMob(Player *player, int32_t mobid, bool useskill, int32_t skill, unsigned char *buf, int32_t len);
	void damageMob(Player *player, PacketReader &pack);
	void damageMobRanged(Player *player, PacketReader &pack);
	void damageMobSpell(Player *player, PacketReader &pack);
	void damageMobSummon(Player *player, PacketReader &pack);
	void applyStatus(Mob *mob, int32_t status, const vector<StatusInfo> &info, int16_t delay);
	void removeStatus(Mob *mob, int32_t status);
	void showHP(Player *player, int32_t mobid, int8_t per, bool miniboss);
	void showBossHP(Player *player, int32_t mobid, int32_t hp, const MobInfo &info); // For major bosses
	void dieMob(Mob *mob);
};

#endif
