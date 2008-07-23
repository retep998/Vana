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
#ifndef SKILLS_H
#define SKILLS_H

#include <hash_map>
#include "SkillsPacket.h"
#include "Pos.h"

#define FORTHJOB_SKILL(x) ((x/10000)%10 == 2)

using namespace std;
using namespace stdext;

class Player;
class SkillTimer;
class ReadPacket;

struct SkillLevelInfo {
	int time;
	short mp;
	short hp;
	int item;
	int itemcount;
	short x;
	short y;
	short speed;
	short jump;
	short watk;
	short wdef;
	short matk;
	short mdef;
	short acc;
	short avo;
	short hpP;
	short prop;
	Pos lt;
	Pos rb;
	int cooltime;
};

enum {
	SKILL_X,
	SKILL_Y,
	SKILL_SPEED,
	SKILL_JUMP,
	SKILL_WATK,
	SKILL_WDEF,
	SKILL_MATK,
	SKILL_MDEF,
	SKILL_ACC,
	SKILL_AVO,
	SKILL_PROP
};

struct SkillPlayerInfo {
	unsigned char type;
	char byte;
	char value;
};

struct SkillMapInfo {
	unsigned char type;
	char byte;
	char value;
	bool val;
};

struct SkillAct {
	char name[50];
	char value;
	int time;
};

struct SkillActiveInfo {
	unsigned char types[8];
	vector <short> vals;
};

struct SkillMapActiveInfo {
	char byte;
	char type;
	char value;
	bool isvalue;
	int skill;
};

struct SkillMapEnterActiveInfo {
	SkillMapEnterActiveInfo() : val(0), isval(false) {
		for (size_t i = 0; i < 8; i++) {
			types[i] = 0;
		}
	}
	unsigned char types[8];
	char val;
	bool isval;
};

struct SpecialSkillInfo {
	SpecialSkillInfo() : skillid(0), level(0), w_speed(0), direction(0) { }
	unsigned char level;
	unsigned char w_speed;
	unsigned char direction;
	int skillid;
};

struct PowerGuardInfo {
	PowerGuardInfo() : reduction(0), pos_x(0), pos_y(0), damage(0), mapmobid(0) { }
	unsigned char reduction;
	short pos_x;
	short pos_y;
	int damage;
	int mapmobid;
};
struct SkillsInfo {
	vector <SkillPlayerInfo> player;
	vector <SkillMapInfo> map;
	SkillAct act;
	vector <bool> bact;
};
typedef hash_map <int, SkillLevelInfo> SkillsLevelInfo;

namespace Skills {
	void init();
	extern hash_map <int, SkillsLevelInfo> skills;
	extern hash_map <int, SkillsInfo> skillsinfo;
	void addSkill(int id, SkillsLevelInfo skill);
	void addSkill(Player *player, ReadPacket *packet);
	void cancelSkill(Player *player, ReadPacket *packet);
	void useSkill(Player *player, ReadPacket *packet);
	void useAttackSkill(Player *player, int skillid);
	void stopTimerPlayer(Player *player);
	void heal(Player *player, short value, int skillid);
	void endSkill(Player *player, int skillid);
	void stopSkill(Player *player, int skillid);
	void addCombo(Player *player); // Combo Attack 
	void clearCombo(Player *player); // Combo Attack
};

class PlayerSkills {
public:
	PlayerSkills(Player *player) : player(player) { }
	void addSkillLevel(int skillid, int level, bool sendpacket = true) {
		if (playerskills.find(skillid) != playerskills.end())
			playerskills[skillid] += level;
		else
			playerskills[skillid] = level;
		
		if (sendpacket) {
			int maxlevel = 0;
			if (FORTHJOB_SKILL(skillid)) {
				maxlevel = getMaxSkillLevel(skillid);
			}
			SkillsPacket::addSkill(player, skillid, getSkillLevel(skillid), maxlevel);
		}
	}
	int getSkillLevel(int skillid) {
		if (playerskills.find(skillid) != playerskills.end())
			return playerskills[skillid];
		else
			return 0;
	}
	void setMaxSkillLevel(int skillid, int maxlevel) { // Set max level for 4th job skills
		maxlevels[skillid] = maxlevel;
	}
	int getMaxSkillLevel(int skillid) { // Get max level for 4th job skills
		if (maxlevels.find(skillid) != maxlevels.end())
			return maxlevels[skillid];
		else
			return 0;
	}
	int getSkillsNum() {
		return playerskills.size();
	}
	int getSkillID(int i) {
		int j = 0;
		for (hash_map <int,int>::iterator iter = playerskills.begin(); iter != playerskills.end(); iter++) {
			if (j == i) {
				return iter->first;
			} 
			j++;
		}
		return 0;
	}
	void setSkillPlayerInfo(int skillid, SkillActiveInfo skill) {
		activeplayerskill[skillid] = skill;
	}
	void setSkillMapInfo(int skillid, SkillActiveInfo skill) {
		activemapskill[skillid] = skill;
	}
	void setSkillMapEnterInfo(int skillid, vector <SkillMapActiveInfo> skill) {
		// TEMP //
		for (unsigned int i=0; i<activemapenterskill.size(); i++) { 
			if (activemapenterskill[i].isvalue) {
				activemapenterskill.erase(activemapenterskill.begin()+i);
				break;
			}
		}
		//////////
		for (unsigned int i=0; i<skill.size(); i++) {
			activemapenterskill.push_back(skill[i]);
		}
	}
	void deleteSkillMapEnterInfo(int skillid) {
		for (unsigned int i=0; i<activemapenterskill.size(); i++) {
			if (activemapenterskill[i].skill == skillid) {
				activemapenterskill.erase(activemapenterskill.begin()+i);
			}
		}
	}
	SkillMapEnterActiveInfo getSkillMapEnterInfo() {
		SkillMapEnterActiveInfo skill;
		for (unsigned int i=0; i<activemapenterskill.size(); i++) {
			 skill.types[activemapenterskill[i].byte-1] += activemapenterskill[i].type;
			if (activemapenterskill[i].isvalue) {
				skill.val = activemapenterskill[i].value;
				skill.isval = true;
			}
		}
		return skill;
	}
	SkillActiveInfo getSkillPlayerInfo(int skillid) {
		return activeplayerskill[skillid];
	}
	SkillActiveInfo getSkillMapInfo(int skillid) {
		return activemapskill[skillid];
	}
	void setActiveSkillLevel(int skillid, int level) {
		activelevels[skillid] = level;
	}
	int getActiveSkillLevel(int skillid) {
		if (activelevels.find(skillid) == activelevels.end())
			return 0;
		return activelevels[skillid];
	}
	int getActiveSkillsNum() {
		int j = 0;
		for (hash_map <int,int>::iterator iter = activelevels.begin(); iter != activelevels.end(); iter++) {
			j++;
		}
		return j;
	}
	int getActiveSkillsID(int i) {
		int j = 0;
		for (hash_map <int,int>::iterator iter = activelevels.begin(); iter != activelevels.end(); iter++) {
			if (j == i) {
				return iter->first;
			} 
			j++;
		}
		return 0;
	}
private:
	hash_map <int, int> playerskills;
	hash_map <int, int> maxlevels;
	hash_map <int, SkillActiveInfo> activeplayerskill;
	hash_map <int, SkillActiveInfo> activemapskill;
	vector <SkillMapActiveInfo> activemapenterskill;
	hash_map <int, int> activelevels;
	Player *player;
};

#endif
