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

#include <unordered_map>
#include <vector>
#include "Pos.h"

using std::tr1::unordered_map;
using std::vector;

class Player;
class ReadPacket;

#define BEGINNER_SKILL(x) (x / 1000000 == 0)
#define FORTHJOB_SKILL(x) ((x/10000)%10 == 2)

struct SkillLevelInfo {
	int32_t time;
	int16_t mp;
	int16_t hp;
	int32_t item;
	int16_t itemcount;
	int16_t bulletcon;
	int16_t moneycon;
	int16_t x;
	int16_t y;
	int16_t speed;
	int16_t jump;
	int16_t watk;
	int16_t wdef;
	int16_t matk;
	int16_t mdef;
	int16_t acc;
	int16_t avo;
	uint16_t hpP;
	uint16_t prop;
	int16_t morph;
	Pos lt;
	Pos rb;
	int16_t cooltime;
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
	TYPE_5 = 0,
	TYPE_6,
	TYPE_7,
	TYPE_8,
	TYPE_1,
	TYPE_2,
	TYPE_3,
	TYPE_4
};

enum Act {
	ACT_HEAL,
	ACT_HURT
};

struct BuffInfo {
	uint8_t type;
	int8_t byte;
	int8_t value;
	bool hasmapval;
	bool val;
};

struct SkillAct {
	Act type;
	int8_t value;
	int32_t time;
};

struct SkillActiveInfo {
	uint8_t types[8];
	vector<int16_t> vals;
};

struct SkillMapActiveInfo {
	int8_t byte;
	int8_t type;
	int8_t value;
	bool isvalue;
	int32_t skill;
};

struct SkillMapEnterActiveInfo {
	SkillMapEnterActiveInfo() : val(0), isval(false) {
		for (size_t i = 0; i < 8; i++) {
			types[i] = 0;
		}
	}
	uint8_t types[8];
	int8_t val;
	bool isval;
};

struct SpecialSkillInfo { // Hurricane, Big Bang, Monster Magnet, Pierce, etc.
	SpecialSkillInfo() : skillid(0), level(0), w_speed(0), direction(0) { }
	uint8_t level;
	uint8_t w_speed;
	uint8_t direction;
	int32_t skillid;
};

struct PGMRInfo { // Power Guard/Mana Reflection
	PGMRInfo() : reduction(0), pos(0,0), damage(0), mapmobid(0), isphysical(true) { }
	uint8_t reduction;
	int32_t damage;
	int32_t mapmobid;
	bool isphysical;
	Pos pos;
};

struct MPEaterInfo { // MP Eater
	MPEaterInfo() : id(0), level(0), x(0), prop(0), onlyonce(false) { }
	int32_t id;
	uint8_t level;
	int16_t x;
	uint16_t prop;
	bool onlyonce;
};

struct SkillsInfo {
	vector<BuffInfo> player;
	SkillAct act;
	vector<bool> bact;
};

typedef unordered_map<uint8_t, SkillLevelInfo> SkillsLevelInfo;

namespace Skills {
	extern unordered_map<int32_t, SkillsLevelInfo> skills;
	extern unordered_map<int32_t, uint8_t> maxlevels;
	extern unordered_map<int32_t, SkillsInfo> skillsinfo;
	void init();
	void addSkillLevelInfo(int32_t skillid, uint8_t level, SkillLevelInfo levelinfo);
	void addSkill(Player *player, ReadPacket *packet);
	void cancelSkill(Player *player, ReadPacket *packet);
	void useSkill(Player *player, ReadPacket *packet);
	int16_t getValue(int8_t value, int32_t skillid, uint8_t level);
	SkillActiveInfo parseBuffInfo(Player *player, int32_t skillid, uint8_t level, int32_t &mountid);
	SkillActiveInfo parseBuffMapInfo(Player *player, int32_t skillid, uint8_t level, vector<SkillMapActiveInfo> &mapenterskill);
	void applySkillCosts(Player *player, int32_t skillid, uint8_t level, bool elementalamp = false);
	void useAttackSkill(Player *player, int32_t skillid);
	void useAttackSkillRanged(Player *player, int32_t skillid, int16_t pos, uint8_t display);
	void stopTimersPlayer(Player *player);
	void stopAllBuffs(Player *player);
	void heal(Player *player, int16_t value, int32_t skillid);
	void hurt(Player *player, int16_t value, int32_t skillid);
	void endBuff(Player *player, int32_t skill);
	void stopSkill(Player *player, int32_t skillid, bool fromTimer = false);
	void startCooldown(Player *player, int32_t skillid, int16_t cooltime);
	void stopCooldown(Player *player, int32_t skillid);
	bool isCooling(Player *player, int32_t skillid);
	bool isBuff(int32_t skillid);
};

#endif
