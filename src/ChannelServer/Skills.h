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
	short itemcount;
	short bulletcon;
	short moneycon;
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
	short morph;
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
	SKILL_PROP,
	SKILL_MORPH,
	SKILL_LV
};

enum {
	TYPE_5 = 1,
	TYPE_6,
	TYPE_7,
	TYPE_8,
	TYPE_1,
	TYPE_2,
	TYPE_3,
	TYPE_4
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

struct PGMRInfo { // Power Guard/Mana Reflection
	PGMRInfo() : reduction(0), pos_x(0), pos_y(0), damage(0), mapmobid(0), isphysical(true) { }
	unsigned char reduction;
	short pos_x;
	short pos_y;
	int damage;
	int mapmobid;
	bool isphysical;
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
	extern hash_map <int, short> maxlevels;
	extern hash_map <int, SkillsInfo> skillsinfo;
	void addSkillLevelInfo(int skillid, short level, SkillLevelInfo levelinfo);
	void addSkill(Player *player, ReadPacket *packet);
	void cancelSkill(Player *player, ReadPacket *packet);
	void useSkill(Player *player, ReadPacket *packet);
	void useAttackSkill(Player *player, int skillid);
	void stopTimersPlayer(Player *player);
	void stopAllBuffs(Player *player);
	void stopCooldownTimersPlayer(Player *player);
	void heal(Player *player, short value, int skillid);
	void hurt(Player *player, short value, int skillid);
	void endSkill(Player *player, int skillid);
	void endBuff(Player *player, int skill);
	void stopSkill(Player *player, int skillid);
	void addCombo(Player *player); // Combo Attack 
	void clearCombo(Player *player); // Combo Attack
	void startCooldown(Player *player, int skillid, int cooltime);
	void stopCooldown(Player *player, int skillid);
	bool isCooling(Player *player, int skillid);
};

#endif
