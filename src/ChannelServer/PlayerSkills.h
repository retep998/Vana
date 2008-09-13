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
#ifndef PLAYERSKILLS_H
#define PLAYERSKILLS_H

#include "Types.h"
#include <unordered_map>

using std::tr1::unordered_map;

class Player;

struct PlayerSkillInfo {
	PlayerSkillInfo() : level(0), maxlevel(0) {}
	unsigned char level;
	unsigned char maxlevel;
};

class PlayerSkills {
public:
	PlayerSkills(Player *player) : player(player) { load(); }

	bool addSkillLevel(int32_t skillid, unsigned char amount, bool sendpacket = true);
	unsigned char getSkillLevel(int32_t skillid);
	unsigned char getMaxSkillLevel(int32_t skillid);
	void setMaxSkillLevel(int32_t skillid, unsigned char maxlevel, bool sendpacket = true);
	unordered_map<int32_t, PlayerSkillInfo> * getSkills() {
		return &playerskills;
	}
	void load();
	void save();
private:
	unordered_map<int32_t, PlayerSkillInfo> playerskills;
	Player *player;
};

#endif
