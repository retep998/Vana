/*
Copyright (C) 2008-2012 Vana Development Team

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

#include "MobDataObjects.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <string>
#include <vector>

using std::string;
using std::tr1::unordered_map;
using std::vector;

class MobDataProvider : boost::noncopyable {
public:
	static MobDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new MobDataProvider();
		return singleton;
	}
	void loadData();

	bool mobExists(int32_t mobId) { return m_mobInfo.find(mobId) != m_mobInfo.end(); }
	MobInfo getMobInfo(int32_t mobId) { return m_mobInfo[mobId]; }
	MobAttackInfo * getMobAttack(int32_t mobId, uint8_t index);
	MobSkillInfo * getMobSkill(int32_t mobId, uint8_t index);
	uint8_t getSkillCount(int32_t mobId);
private:
	MobDataProvider() {}
	static MobDataProvider *singleton;

	void loadMobs();
	void loadAttacks();
	void loadSkills();
	void loadSummons();
	int8_t getElemModifier(const string &elemAttr);

	unordered_map<int32_t, MobInfo> m_mobInfo;
	unordered_map<int32_t, vector<MobAttackInfo>> m_attacks;
	unordered_map<int32_t, vector<MobSkillInfo>> m_skills;
};