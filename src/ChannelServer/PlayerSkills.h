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

#include <unordered_map>

using std::tr1::unordered_map;

class Player;

class PlayerSkills {
public:
	PlayerSkills(Player *player) : player(player) { }

	bool addSkillLevel(int skillid, unsigned char amount, bool sendpacket = true);
	void setMaxSkillLevel(int skillid, unsigned char maxlevel) { // Set max level for 4th job skills
		maxlevels[skillid] = maxlevel;
	}
	unsigned char getSkillLevel(int skillid);
	unsigned char getMaxSkillLevel(int skillid);
	unordered_map<int, unsigned char> * getSkills() {
		return &playerskills;
	}
	void save();
private:
	unordered_map<int, unsigned char> playerskills;
	unordered_map<int, unsigned char> maxlevels;
	Player *player;
};

#endif
