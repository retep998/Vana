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
#ifndef BUFFS_H
#define BUFFS_H

#include "Types.h"
#include <tr1/unordered_map>
#include <vector>

using std::tr1::unordered_map;
using std::vector;

class Player;

enum Act {
	ACT_HEAL,
	ACT_HURT
};

enum SkillValues {
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

enum ByteTypes {
	TYPE_5,
	TYPE_6,
	TYPE_7,
	TYPE_8,
	TYPE_1,
	TYPE_2,
	TYPE_3,
	TYPE_4
};

struct BuffInfo {
	BuffInfo() : type(0), byte(0), value(0), hasmapval(false), showmaponly(false), mapval(false) { }
	uint8_t type;
	int8_t byte;
	int8_t value;
	bool hasmapval;
	bool showmaponly;
	bool mapval;
};

struct SkillAct {
	Act type;
	int8_t value;
	int32_t time;
};

struct SkillsInfo {
	vector<BuffInfo> player;
	SkillAct act;
	vector<bool> bact;
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

class Buffs {
public:
	static Buffs * Instance() {
		if (singleton == 0)
			singleton = new Buffs();
		return singleton;
	}
	bool addBuff(Player *player, int32_t skillid, uint8_t level, int16_t addedinfo);
	void addBuff(Player *player, int32_t itemid, int32_t time, SkillActiveInfo &iteminfo, bool morph);
	void endBuff(Player *player, int32_t skill);

private:
	Buffs();
	Buffs(const Buffs&);
	Buffs& operator=(const Buffs&);
	static Buffs *singleton;

	unordered_map<int32_t, SkillsInfo> skillsinfo;

	int16_t getValue(int8_t value, int32_t skillid, uint8_t level);
	SkillActiveInfo parseBuffInfo(Player *player, int32_t skillid, uint8_t level, int32_t &mountid);
	SkillActiveInfo parseBuffMapInfo(Player *player, int32_t skillid, uint8_t level, vector<SkillMapActiveInfo> &mapenterskill);
};

#endif
