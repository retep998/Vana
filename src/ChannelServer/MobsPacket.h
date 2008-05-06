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

namespace MobsPacket {
	void controlMob(Player* player, Mob* mob);
	void endControlMob(Player* player, Mob* mob);
	void spawnMob(Player* player, Mob* mob, vector <Player*> players, bool isspawn);
	void showMob(Player* player, Mob* mob);
	void moveMob(Player* player, Mob* mob ,vector <Player*> players, unsigned char* pack, int pla);
	void damageMob(Player* player, vector <Player*> players, unsigned char* pack);
	void showHP(Player* player, int mobid, char per);
	void dieMob(Player* player, vector<Player*> players, Mob* mob, int mobid);
	void damageMobSkill(Player* player, vector <Player*> players, unsigned char* pack);
	void damageMobS(Player* player, vector <Player*> players, unsigned char* pack, int itemid);
};

#endif