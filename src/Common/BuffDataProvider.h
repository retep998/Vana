/*
Copyright (C) 2008-2013 Vana Development Team

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

#include "noncopyable.hpp"
#include "SkillConstants.h"
#include "Types.h"
#include <unordered_map>
#include <vector>

using std::unordered_map;
using std::vector;

struct ConsumeInfo;

struct Buff {
	Buff() : type(0), byte(0), value(0) { }
	uint8_t type;
	int8_t byte;
	int8_t value;
};

struct BuffInfo {
	BuffInfo() : itemVal(0), hasMapVal(false), hasmapentry(false), useVal(false) { }
	Buff buff;
	int16_t itemVal;
	bool hasMapVal;
	bool hasmapentry;
	bool useVal;
};

struct BuffMapInfo {
	BuffMapInfo() : useVal(false) { }
	Buff buff;
	bool useVal;
};

struct BuffAct {
	Action type;
	int8_t value;
	int32_t time;
};

struct SkillInfo {
	SkillInfo() : hasAction(false) { }
	vector<BuffInfo> player;
	vector<BuffMapInfo> map;
	BuffAct act;
	bool hasAction;
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
	void addItemInfo(int32_t itemId, const ConsumeInfo &cons);

	bool isBuff(int32_t skillId) { return (m_skillInfo.find(skillId) != m_skillInfo.end()); }
	bool isDebuff(uint8_t skillId) { return (m_mobSkillInfo.find(skillId) != m_mobSkillInfo.end()); }
	SkillInfo * getSkillInfo(int32_t skillId) { return &m_skillInfo[skillId]; }
	MobAilmentInfo * getMobSkillInfo(uint8_t skillId) { return (m_mobSkillInfo.find(skillId) != m_mobSkillInfo.end() ? &m_mobSkillInfo[skillId] : nullptr); }
private:
	BuffDataProvider() { }
	static BuffDataProvider *singleton;

	unordered_map<int32_t, SkillInfo> m_skillInfo;
	unordered_map<uint8_t, MobAilmentInfo> m_mobSkillInfo;
};