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
#ifndef MOBDATA_H
#define MOBDATA_H

#include "GameConstants.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <string>
#include <vector>

using std::string;
using std::tr1::unordered_map;
using std::vector;

struct MobAttackInfo {
	MobAttackInfo() : id(0), level(0), mpconsume(0), mpburn(0), mobid(-1), disease(0), deadlyattack(false) { }
	int8_t id;
	uint8_t level;
	uint8_t disease;
	uint8_t mpconsume;
	uint16_t mpburn;
	int32_t mobid;
	bool deadlyattack;
};

struct MobSkillInfo {
	MobSkillInfo() : skillid(0), level(0), effectAfter(0) { }
	uint8_t skillid;
	uint8_t level;
	int16_t effectAfter;
};

struct MobInfo {
	uint8_t level;
	int32_t hp;
	int32_t mp;
	int32_t hprecovery;
	int32_t mprecovery;
	int32_t exp;
	int32_t selfdestruction;
	int32_t buff;
	int32_t link;
	int32_t removeafter;
	bool boss;
	bool canfreeze;
	bool canpoison;
	bool undead;
	bool flying;
	bool friendly;
	bool publicreward;
	bool explosivereward;
	int8_t hpcolor;
	int8_t hpbgcolor;
	vector<int32_t> summon;
	vector<MobAttackInfo> attacks;
	vector<MobSkillInfo> skills;
};

struct BanishField {
	BanishField() : message(""), portal(""), field(Maps::NoMap) { }
	string message;
	string portal;
	int32_t field;
};

class MobDataProvider : boost::noncopyable {
public:
	static MobDataProvider * Instance() {
		if (singleton == 0)
			singleton = new MobDataProvider();
		return singleton;
	}
	void loadData();

	bool mobExists(int32_t mobid) { return mobinfo.find(mobid) != mobinfo.end(); }
	MobInfo const getMobInfo(int32_t mobid) { return mobinfo[mobid]; }
	MobAttackInfo * getMobAttack(int32_t mobid, uint8_t type);
	MobSkillInfo * getMobSkill(int32_t mobid, uint8_t index);
	bool hasBanishData(int32_t mobid) { return banishinfo.find(mobid) != banishinfo.end(); }
	string getBanishMessage(int32_t mobid) { return banishinfo[mobid].message; }
	string getBanishPortal(int32_t mobid) { return banishinfo[mobid].portal; }
	int32_t getBanishMap(int32_t mobid) { return banishinfo[mobid].field; }
private:
	MobDataProvider() {}
	static MobDataProvider *singleton;

	unordered_map<int32_t, MobInfo> mobinfo;
	unordered_map<int32_t, BanishField> banishinfo;
};

#endif
