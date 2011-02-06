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
#ifndef MOBDATAPROVIDER_H
#define MOBDATAPROVIDER_H

#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <string>
#include <vector>

using std::string;
using std::tr1::unordered_map;
using std::vector;

struct MobAttackInfo {
	MobAttackInfo() : id(0), level(0), disease(0), mpconsume(0), mpburn(0), mobid(-1), deadlyattack(false) { }
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
	MobInfo() :
		boss(false),
		canfreeze(false),
		canpoison(false),
		undead(false),
		flying(false),
		friendly(false),
		publicreward(false),
		explosivereward(false),
		invincible(false),
		onlynormalattacks(false),
		keepcorpse(false),
		autoaggro(false),
		damageable(true),
		candamage(true)
		{ }
	int8_t cp;
	int8_t iceattr;
	int8_t fireattr;
	int8_t poisonattr;
	int8_t lightningattr;
	int8_t holyattr;
	int8_t nonelemattr;
	int8_t hpcolor;
	int8_t hpbgcolor;
	uint8_t skillcount;
	int16_t watk;
	int16_t wdef;
	int16_t matk;
	int16_t mdef;
	int16_t acc;
	int16_t avo;
	int16_t speed;
	int16_t chasespeed;
	int16_t summontype;
	uint16_t level;
	int32_t selfdestruction;
	int32_t buff;
	int32_t link;
	int32_t removeafter;
	int32_t knockback;
	int32_t fixeddamage;
	int32_t damageskill;
	int32_t damagemob;
	uint32_t hp;
	uint32_t mp;
	uint32_t exp;
	uint32_t hprecovery;
	uint32_t mprecovery;
	bool boss;
	bool canfreeze;
	bool canpoison;
	bool undead;
	bool flying;
	bool friendly;
	bool publicreward;
	bool explosivereward;
	bool invincible;
	bool damageable;
	bool candamage;
	bool autoaggro;
	bool keepcorpse;
	bool onlynormalattacks;
	double traction;
	vector<int32_t> summon;
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
	MobInfo * getMobInfo(int32_t mobid) { return &mobinfo[mobid]; }
	MobAttackInfo * getMobAttack(int32_t mobid, uint8_t index);
	MobSkillInfo * getMobSkill(int32_t mobid, uint8_t index);
	uint8_t getSkillCount(int32_t mobid);
private:
	MobDataProvider() {}
	static MobDataProvider *singleton;

	void loadMobs();
	void loadAttacks();
	void loadSkills();
	void loadSummons();
	int8_t getElemModifier(const string &elemattr);

	unordered_map<int32_t, MobInfo> mobinfo;
	unordered_map<int32_t, vector<MobAttackInfo> > attacks;
	unordered_map<int32_t, vector<MobSkillInfo> > skills;
};

#endif
