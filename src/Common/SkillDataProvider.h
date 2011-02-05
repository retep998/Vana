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

#include "MapConstants.h"
#include "Pos.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <string>
#include <vector>

using std::string;
using std::tr1::unordered_map;
using std::vector;

struct SkillLevelInfo {
	int8_t mobCount;
	int8_t hitCount;
	int8_t mastery;
	uint8_t criticalDamage;
	int16_t mp;
	int16_t hp;
	int16_t itemCount;
	int16_t bulletConsume;
	int16_t moneyConsume;
	int16_t x;
	int16_t y;
	int16_t speed;
	int16_t jump;
	int16_t str;
	int16_t wAtk;
	int16_t wDef;
	int16_t mAtk;
	int16_t mDef;
	int16_t acc;
	int16_t avo;
	int16_t cooltime;
	int16_t morph;
	int16_t damage;
	int16_t range;
	uint16_t hpProp;
	uint16_t mpProp;
	uint16_t prop;
	int32_t fixedDamage;
	int32_t item;
	int32_t time;
	int32_t optionalItem;
	Pos lt;
	Pos rb;
};
typedef unordered_map<uint8_t, SkillLevelInfo> SkillsLevelInfo;

struct SpecialSkillInfo { // Hurricane, Big Bang, Monster Magnet, Pierce, etc.
	SpecialSkillInfo() : level(0), weaponSpeed(0), direction(0),  skillId(0) { }
	uint8_t level;
	uint8_t weaponSpeed;
	uint8_t direction;
	int32_t skillId;
};

struct ReturnDamageInfo { // Power Guard/Mana Reflection
	ReturnDamageInfo() : reduction(0), damage(0), mapMobId(0), isPhysical(true), pos(0,0) { }
	uint8_t reduction;
	int32_t damage;
	int32_t mapMobId;
	bool isPhysical;
	Pos pos;
};

struct MpEaterInfo { // MP Eater
	MpEaterInfo() : skillId(0), level(0), x(0), prop(0), used(false) { }
	int32_t skillId;
	uint8_t level;
	int16_t x;
	uint16_t prop;
	bool used;
};

struct MobSkillLevelInfo {
	int8_t summonEffect;
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

struct BanishField {
	BanishField() : message(""), portal(""), field(Maps::NoMap) { }
	string message;
	string portal;
	int32_t field;
};

struct MorphData {
	MorphData() : superman(false) { }
	uint8_t speed;
	uint8_t jump;
	double traction;
	double swim;
	bool superman;
};

namespace SkillTypes {
	enum {
		Melee,
		Ranged,
		Magic,
		Summon,
		EnergyCharge
	};
}

struct Attack {
	typedef unordered_map<int32_t, vector<int32_t> >::const_iterator iterator;
	typedef vector<int32_t>::const_iterator diterator;

	Attack() :
		isMesoExplosion(false),
		isChargeSkill(false),
		isPiercingArrow(false),
		isShadowMeso(false),
		isHeal(false),
		charge(0),
		starPos(-1),
		cashStarPos(-1),
		starId(0),
		skillLevel(0),
		totalDamage(0),
		portals(0)
		{ }

	bool isMesoExplosion;
	bool isShadowMeso;
	bool isChargeSkill;
	bool isPiercingArrow;
	bool isHeal;
	int8_t targets;
	int8_t hits;
	uint8_t display;
	uint8_t weaponSpeed;
	uint8_t animation;
	uint8_t weaponClass;
	uint8_t skillLevel;
	uint8_t portals;
	int16_t starPos;
	int16_t cashStarPos;
	int32_t skillId;
	int32_t summonId;
	int32_t charge;
	int32_t ticks;
	int32_t starId;
	int64_t totalDamage;
	Pos projectilePos;
	Pos playerPos;
	unordered_map<int32_t, vector<int32_t> > damages;
};

class SkillDataProvider : boost::noncopyable {
public:
	static SkillDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new SkillDataProvider();
		return singleton;
	}
	void loadData();

	bool isSkill(int32_t skillid) { return (skills.find(skillid) != skills.end()); }
	uint8_t getMaxLevel(int32_t skillid) { return (maxlevels.find(skillid) != maxlevels.end() ? maxlevels[skillid] : 0); }
	SkillLevelInfo * getSkill(int32_t skill, uint8_t level);
	MobSkillLevelInfo * getMobSkill(uint8_t skill, uint8_t level);

	bool hasBanishData(int32_t mobid) { return (banishinfo.find(mobid) != banishinfo.end()); }
	BanishField * getBanishData(int32_t mobid) { return (hasBanishData(mobid) ? &banishinfo[mobid] : nullptr); }

	bool hasMorphData(int16_t morph) { return (morphinfo.find(morph) != morphinfo.end()); }
	MorphData * getMorphData(int16_t morph) { return (hasMorphData(morph) ? &morphinfo[morph] : nullptr); }
private:
	SkillDataProvider() {}
	static SkillDataProvider *singleton;

	void loadPlayerSkills();
	void loadMobSkills();
	void loadMobSummons();
	void loadBanishData();
	void loadMorphs();

	unordered_map<uint8_t, MobSkillsLevelInfo> mobskills;
	unordered_map<int32_t, SkillsLevelInfo> skills;
	unordered_map<int32_t, uint8_t> maxlevels;
	unordered_map<int32_t, BanishField> banishinfo;
	unordered_map<int16_t, MorphData> morphinfo;
};
