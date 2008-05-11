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

using namespace std;
using namespace stdext;

class Player;
class SkillTimer;

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
};

#define SKILL_X       1
#define SKILL_Y       2
#define SKILL_SPEED   3
#define SKILL_JUMP    4
#define SKILL_WATK    5
#define SKILL_WDEF    6
#define SKILL_MATK    7
#define SKILL_MDEF    8
#define SKILL_ACC     9
#define SKILL_AVO    10

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
	unsigned char types[8];
	char val;
	bool isval;
};

struct SkillsInfo {
	vector <SkillPlayerInfo> player;
	vector <SkillMapInfo> map;
	SkillAct act;
	vector <bool> bact;
};
typedef hash_map <int, SkillLevelInfo> SkillsLevelInfo;

namespace Skills {
	extern SkillTimer* timer;
	void startTimer();
	extern hash_map <int, SkillsLevelInfo> skills;
	extern hash_map <int, SkillsInfo> skillsinfo;
	void addSkill(int id, SkillsLevelInfo skill);
	void addSkill(Player* player, unsigned char* packet);
	void cancelSkill(Player* player, unsigned char* packet);
	void useSkill(Player* player, unsigned char* packet);
	void useAttackSkill(Player* player, int skillid);
	void stopTimerPlayer(Player* player);
	void heal(Player* player, short value, int skillid);
	void endSkill(Player* player, int skillid);
	void stopSkill(Player* player, int skillid);
};

class PlayerSkills {
public:
	void addSkillLevel(int skillid, int level){
		for (hash_map<int,int>::iterator iter = playerskills.begin(); iter != playerskills.end(); iter++){
			if(iter->first == skillid){
				iter->second += level;
				return;
			}
		}
		playerskills[skillid] = level;
	}
	int getSkillLevel(int skillid){
		for (hash_map<int,int>::iterator iter = playerskills.begin(); iter != playerskills.end(); iter++){
			if(iter->first == skillid){
				return iter->second;
			} 
		}
		return 0;
	}
	int getSkillsNum(){
		return playerskills.size();
	}
	int getSkillID(int i){
		int j=0;
		for (hash_map<int,int>::iterator iter = playerskills.begin(); iter != playerskills.end(); iter++){
			if(j == i){
				return iter->first;
			} 
			j++;
		}
		return 0;
	}
	void setSkillPlayerInfo(int skillid, SkillActiveInfo skill){
		activeplayerskill[skillid] = skill;
	}
	void setSkillMapInfo(int skillid, SkillActiveInfo skill){
		activemapskill[skillid] = skill;
	}
	void setSkillMapEnterInfo(int skillid, vector <SkillMapActiveInfo> skill){
		// TEMP //
		for (unsigned int i=0; i<activemapenterskill.size(); i++){ 
			if(activemapenterskill[i].isvalue){
				activemapenterskill.erase(activemapenterskill.begin()+i);
				break;
			}
		}
		//////////
		for (unsigned int i=0; i<skill.size(); i++){
			activemapenterskill.push_back(skill[i]);
		}
	}
	void deleteSkillMapEnterInfo(int skillid){
		for (unsigned int i=0; i<activemapenterskill.size(); i++){
			if(activemapenterskill[i].skill == skillid){
				activemapenterskill.erase(activemapenterskill.begin()+i);
			}
		}
	}
	SkillMapEnterActiveInfo getSkillMapEnterInfo(){
		SkillMapEnterActiveInfo skill;
		skill.types[0] = 0;
		skill.types[1] = 0;
		skill.types[2] = 0;
		skill.types[3] = 0;
		skill.types[4] = 0;
		skill.types[5] = 0;
		skill.types[6] = 0;
		skill.types[7] = 0;
		skill.val = 0;
		skill.isval = false;
		for (unsigned int i=0; i<activemapenterskill.size(); i++){
			 skill.types[activemapenterskill[i].byte-1] += activemapenterskill[i].type;
			if(activemapenterskill[i].isvalue){
				skill.val = activemapenterskill[i].value;
				skill.isval = true;
			}
		}
		return skill;
	}
	SkillActiveInfo getSkillPlayerInfo(int skillid){
		return activeplayerskill[skillid];
	}
	SkillActiveInfo getSkillMapInfo(int skillid){
		return activemapskill[skillid];
	}
	void setActiveSkillLevel(int skillid, int level){
		activelevels[skillid] = level;
	}
	int getActiveSkillLevel(int skillid){
		if(activelevels.find(skillid) == activelevels.end())
			return 0;
		return activelevels[skillid];
	}
private:
	hash_map <int, int> playerskills;
	hash_map <int, SkillActiveInfo> activeplayerskill;
	hash_map <int, SkillActiveInfo> activemapskill;
	vector <SkillMapActiveInfo> activemapenterskill;
	hash_map <int, int> activelevels;
};

#endif
