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

#include <vector>
#include <hash_map>

using std::vector;
using stdext::hash_map;

class Player;
struct SkillMapEnterActiveInfo;
struct SkillMapActiveInfo;
struct SkillActiveInfo;

class PlayerSkills {
public:
	PlayerSkills(Player *player) : player(player) { }

	void addSkillLevel(int skillid, short amount, bool sendpacket = true);

	void deleteSkillMapEnterInfo(int skillid);

	short getSkillLevel(int skillid);
	short getMaxSkillLevel(int skillid);
	int getSkillsNum() {
		return playerskills.size();
	}
	int getSkillID(size_t i);
	SkillMapEnterActiveInfo getSkillMapEnterInfo();
	SkillActiveInfo getSkillPlayerInfo(int skillid);
	SkillActiveInfo getSkillMapInfo(int skillid);
	short getActiveSkillLevel(int skillid);

	void setMaxSkillLevel(int skillid, int maxlevel) { // Set max level for 4th job skills
		maxlevels[skillid] = maxlevel;
	}
	void setSkillPlayerInfo(int skillid, SkillActiveInfo skill);
	void setSkillMapInfo(int skillid, SkillActiveInfo skill);
	void setSkillMapEnterInfo(int skillid, vector <SkillMapActiveInfo> skill);
	void setActiveSkillLevel(int skillid, int level);
private:
	hash_map<int, short> playerskills;
	hash_map<int, short> maxlevels;
	hash_map<int, SkillActiveInfo> activeplayerskill;
	hash_map<int, SkillActiveInfo> activemapskill;
	vector<SkillMapActiveInfo> activemapenterskill;
	hash_map<int, short> activelevels;
	Player *player;
};

#endif