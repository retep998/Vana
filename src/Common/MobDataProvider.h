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

#include "Types.h"
#include <boost/tr1/memory.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <string>
#include <vector>

using std::string;
using std::tr1::unordered_map;
using std::vector;

struct MobAttackInfo {
	MobAttackInfo() : id(0), level(0), disease(0), mpConsume(0), mpBurn(0), deadlyAttack(false) { }
	int8_t id;
	uint8_t level;
	uint8_t disease;
	uint8_t mpConsume;
	uint16_t mpBurn;
	bool deadlyAttack;
};

struct MobSkillInfo {
	MobSkillInfo() : skillId(0), level(0), effectAfter(0) { }
	uint8_t skillId;
	uint8_t level;
	int16_t effectAfter;
};

struct MobInfoRaw {
	MobInfoRaw() :
		boss(false),
		canFreeze(false),
		canPoison(false),
		undead(false),
		flying(false),
		friendly(false),
		publicReward(false),
		explosiveReward(false),
		invincible(false),
		onlyNormalAttacks(false),
		keepCorpse(false),
		autoAggro(false),
		damageable(true),
		canDamage(true)
		{ }
	int8_t carnivalPoints;
	int8_t iceAttr;
	int8_t fireAttr;
	int8_t poisonAttr;
	int8_t lightningAttr;
	int8_t holyAttr;
	int8_t nonElemAttr;
	int8_t hpColor;
	int8_t hpBackgroundColor;
	uint8_t skillCount;
	int16_t wAtk;
	int16_t wDef;
	int16_t mAtk;
	int16_t mDef;
	int16_t acc;
	int16_t avo;
	int16_t speed;
	int16_t chaseSpeed;
	int16_t summonType;
	uint16_t level;
	int32_t selfDestruction;
	int32_t buff;
	int32_t link;
	int32_t removeAfter;
	int32_t knockback;
	int32_t fixedDamage;
	int32_t damageSkill;
	int32_t damageMob;
	uint32_t hp;
	uint32_t mp;
	uint32_t exp;
	uint32_t hpRecovery;
	uint32_t mpRecovery;
	bool boss;
	bool canFreeze;
	bool canPoison;
	bool undead;
	bool flying;
	bool friendly;
	bool publicReward;
	bool explosiveReward;
	bool invincible;
	bool damageable;
	bool canDamage;
	bool autoAggro;
	bool keepCorpse;
	bool onlyNormalAttacks;
	double traction;
	vector<int32_t> summon;
};
typedef std::tr1::shared_ptr<MobInfoRaw> MobInfo;

class MobDataProvider : boost::noncopyable {
public:
	static MobDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new MobDataProvider();
		return singleton;
	}
	void loadData();

	bool mobExists(int32_t mobid) { return mobinfo.find(mobid) != mobinfo.end(); }
	MobInfo getMobInfo(int32_t mobid) { return mobinfo[mobid]; }
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
