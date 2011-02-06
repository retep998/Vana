/*
Copyright (C) 2008-2011 Vana Development Team

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
#pragma once

#include "SkillConstants.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <vector>

using std::tr1::unordered_map;
using std::vector;

struct ConsumeInfo;

struct Buff {
	Buff() : type(0), byte(0), value(0) { }
	uint8_t type;
	int8_t byte;
	int8_t value;
};

struct BuffInfo {
	BuffInfo() : itemval(0), hasmapval(false), hasmapentry(false), useval(false) { }
	Buff buff;
	int16_t itemval;
	bool hasmapval;
	bool hasmapentry;
	bool useval;
};

struct BuffMapInfo {
	BuffMapInfo() : useval(false) { }
	Buff buff;
	bool useval;
};

struct BuffAct {
	Act type;
	int8_t value;
	int32_t time;
};

struct SkillInfo {
	SkillInfo() : bact(false) { }
	vector<BuffInfo> player;
	vector<BuffMapInfo> map;
	BuffAct act;
	bool bact;
};

struct MobAilmentInfo {
	MobAilmentInfo() : delay(0) { }
	vector<BuffInfo> mob;
	int16_t delay;
};

class BuffDataProvider {
public:
	static BuffDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new BuffDataProvider;
		return singleton;
	}
	void loadData();
	void addItemInfo(int32_t itemid, const ConsumeInfo &cons);

	bool isBuff(int32_t skillid) { return (skillsinfo.find(skillid) != skillsinfo.end()); }
	bool isDebuff(uint8_t skillid) { return (mobskillsinfo.find(skillid) != mobskillsinfo.end()); }
	SkillInfo * getSkillInfo(int32_t skillid) { return &skillsinfo[skillid]; }
	MobAilmentInfo * getMobSkillInfo(uint8_t skillid) { return (mobskillsinfo.find(skillid) != mobskillsinfo.end() ? &mobskillsinfo[skillid] : nullptr); }
private:
	BuffDataProvider() { }
	static BuffDataProvider *singleton;

	unordered_map<int32_t, SkillInfo> skillsinfo;
	unordered_map<uint8_t, MobAilmentInfo> mobskillsinfo;
};
