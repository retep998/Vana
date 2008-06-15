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

class Player;
class Mob;

struct MobHPInfo;

namespace MobsPacket {
	void controlMob(Player *player, Mob *mob);
	void endControlMob(Player *player, Mob *mob);
	void spawnMob(vector <Player*> players, Mob *mob);
	void showMob(Player *player, Mob *mob);
	void moveMobResponse(Player *player, int mobid, short moveid, bool useskill, int mp);
	void moveMob(Player *player, vector <Player*> players, int mobid, bool useskill, int skill, unsigned char *buf, int len);
	void showHP(Player *player, int mobid, char per);
	void showMinibossHP(Player *player, vector <Player*> players, int mobid, char per); // For minor bosses
	void showBossHP(Player *player, vector <Player*> players, const MobHPInfo &mob); // For major bosses
	void dieMob(Player *player, vector<Player*> players, Mob *mob, int mobid);
	void damageMob(Player *player, vector <Player*> players, ReadPacket *pack);
	void damageMobSpell(Player *player, vector <Player*> players, ReadPacket *pack);
	void damageMobRanged(Player *player, vector <Player*> players, ReadPacket *pack);
};

#endif
