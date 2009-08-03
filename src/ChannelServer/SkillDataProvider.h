/*
Copyright (C) 2008-2009 Vana Development Team

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
#ifndef SKILLDATA_H
#define SKILLDATA_H

#include "Pos.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <vector>

using std::tr1::unordered_map;
using std::vector;

struct SkillLevelInfo {
	int8_t mobcount;
	int8_t hitcount;
	int16_t mp;
	int16_t hp;
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
	int16_t cooltime;
	int16_t morph;
	int16_t damage;
	uint16_t hpP;
	uint16_t mpP;
	uint16_t prop;
	int32_t item;
	int32_t time;
	Pos lt;
	Pos rb;
};
typedef unordered_map<uint8_t, SkillLevelInfo> SkillsLevelInfo;

struct SpecialSkillInfo { // Hurricane, Big Bang, Monster Magnet, Pierce, etc.
	SpecialSkillInfo() : level(0), w_speed(0), direction(0),  skillid(0) { }
	uint8_t level;
	uint8_t w_speed;
	uint8_t direction;
	int32_t skillid;
};

struct PGMRInfo { // Power Guard/Mana Reflection
	PGMRInfo() : reduction(0), damage(0), mapmobid(0), isphysical(true), pos(0,0) { }
	uint8_t reduction;
	int32_t damage;
	int32_t mapmobid;
	bool isphysical;
	Pos pos;
};

struct MpEaterInfo { // MP Eater
	MpEaterInfo() : id(0), level(0), x(0), prop(0), onlyonce(false) { }
	int32_t id;
	uint8_t level;
	int16_t x;
	uint16_t prop;
	bool onlyonce;
};

struct MobSkillLevelInfo {
	int8_t summoneffect;
	uint8_t mp;
	uint8_t hp;
	uint8_t count;
	int16_t interval;
	int16_t prop;
	int16_t limit;
	int16_t time;
	int32_t x;
	int32_t y;
	Pos lt;
	Pos rb;
	vector<int32_t> summons;
};

typedef unordered_map<uint8_t, MobSkillLevelInfo> MobSkillsLevelInfo;

class SkillDataProvider : boost::noncopyable {
public:
	static SkillDataProvider * Instance() {
		if (singleton == 0)
			singleton = new SkillDataProvider();
		return singleton;
	}
	void loadData();

	bool isSkill(int32_t skillid) { return (skills.find(skillid) != skills.end()); }
	uint8_t getMaxLevel(int32_t skillid) { return (maxlevels.find(skillid) != maxlevels.end() ? maxlevels[skillid] : 0); }
	SkillLevelInfo * getSkill(int32_t skill, uint8_t level);
	MobSkillLevelInfo * getMobSkill(uint8_t skill, uint8_t level);
private:
	SkillDataProvider() {}
	static SkillDataProvider *singleton;

	unordered_map<uint8_t, MobSkillsLevelInfo> mobskills;
	unordered_map<int32_t, SkillsLevelInfo> skills;
	unordered_map<int32_t, uint8_t> maxlevels;
};

#endif
