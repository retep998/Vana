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
#include "SkillDataProvider.h"
#include "Database.h"
#include "InitializeCommon.h"
#include "SkillConstants.h"
#include "StringUtilities.h"

using Initializing::outputWidth;
using StringUtilities::runFlags;

SkillDataProvider * SkillDataProvider::singleton = nullptr;

void SkillDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Skills... ";

	loadPlayerSkills();
	loadMobSkills();
	loadMobSummons();
	loadBanishData();
	loadMorphs();

	std::cout << "DONE" << std::endl;
}

void SkillDataProvider::loadPlayerSkills() {
	maxlevels.clear();
	skills.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM skill_player_level_data");
	mysqlpp::UseQueryResult res = query.use();
	SkillLevelInfo level;
	int32_t skillid;
	uint8_t skilllevel;

	enum SkillData {
		SkillId = 0,
		Level, MobCount, HitCount, Range, Time,
		Mp, Hp, Damage, FixedDamage, CriticalDamage,
		Mastery, OptionalItem, Item, ItemCount, BulletCon,
		MoneyCon, X, Y, Speed, Jump,
		Str, Watk, Wdef, Matk, Mdef,
		Acc, Avoid, HpProp, MpProp, Prop,
		Morph, LTX, LTY, RBX, RBY,
		Cooldown
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		skillid = atoi(row[SkillId]);
		skilllevel = atoi(row[Level]);

		level.mobCount = atoi(row[MobCount]);
		level.hitCount = atoi(row[HitCount]);
		level.time = atoi(row[Time]);
		level.range = atoi(row[Range]);
		level.mp = atoi(row[Mp]);
		level.hp = atoi(row[Hp]);
		level.damage = atoi(row[Damage]);
		level.fixedDamage = atoi(row[FixedDamage]);
		level.criticalDamage = atoi(row[CriticalDamage]);
		level.item = atoi(row[Item]);
		level.optionalItem = atoi(row[OptionalItem]);
		level.itemCount = atoi(row[ItemCount]);
		level.bulletConsume = atoi(row[BulletCon]);
		level.moneyConsume = atoi(row[MoneyCon]);
		level.x = atoi(row[X]);
		level.y = atoi(row[Y]);
		level.speed = atoi(row[Speed]);
		level.jump = atoi(row[Jump]);
		level.str = atoi(row[Str]);
		level.wAtk = atoi(row[Watk]);
		level.wDef = atoi(row[Wdef]);
		level.mAtk = atoi(row[Matk]);
		level.mDef = atoi(row[Mdef]);
		level.acc = atoi(row[Acc]);
		level.avo = atoi(row[Avoid]);
		level.hpProp = atoi(row[HpProp]);
		level.mpProp = atoi(row[MpProp]);
		level.prop = atoi(row[Prop]);
		level.morph = atoi(row[Morph]);
		level.lt = Pos(atoi(row[LTX]), atoi(row[LTY]));
		level.rb = Pos(atoi(row[RBX]), atoi(row[RBY]));
		level.cooltime = atoi(row[Cooldown]);

		skills[skillid][skilllevel] = level;
		if (maxlevels.find(skillid) == maxlevels.end() || maxlevels[skillid] < skilllevel) {
			maxlevels[skillid] = skilllevel;
		}
	}
}

void SkillDataProvider::loadMobSkills() {
	mobskills.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM skill_mob_data");
	mysqlpp::UseQueryResult res = query.use();
	uint8_t skillid;
	uint8_t level;

	MobSkillLevelInfo moblevel;

	enum MobSkills {
		SkillId = 0,
		SkillLevel, Time, Mp, X, Y,
		Prop, Count, Interval, LTX, RBX,
		LTY, RBY, Hp, Limit, SummonEffect
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		skillid = atoi(row[SkillId]);
		level = atoi(row[SkillLevel]);

		moblevel.time = atoi(row[Time]);
		moblevel.mp = atoi(row[Mp]);
		moblevel.x = atoi(row[X]);
		moblevel.y = atoi(row[Y]);
		moblevel.prop = atoi(row[Prop]);
		moblevel.count = atoi(row[Count]);
		moblevel.interval = atoi(row[Interval]);
		moblevel.lt.x = atoi(row[LTX]);
		moblevel.rb.x = atoi(row[RBX]);
		moblevel.lt.y = atoi(row[LTY]);
		moblevel.rb.y = atoi(row[RBY]);
		moblevel.hp = atoi(row[Hp]);
		moblevel.limit = atoi(row[Limit]);
		moblevel.summonEffect = atoi(row[SummonEffect]);

		mobskills[skillid][level] = moblevel;
	}
}

void SkillDataProvider::loadMobSummons() {
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM skill_mob_summons");
	mysqlpp::UseQueryResult res = query.use();
	uint8_t level;
	int32_t mobid;

	enum MobSummons {
		SkillLevel = 0,
		MobId
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		level = atoi(row[SkillLevel]);
		mobid = atoi(row[MobId]);

		mobskills[MobSkills::Summon][level].summons.push_back(mobid);
	}
}

void SkillDataProvider::loadBanishData() {
	banishinfo.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM skill_mob_banish_data");
	mysqlpp::UseQueryResult res = query.use();
	BanishField banish;
	int32_t mobid;

	enum BanishData {
		MobId = 0,
		Message, Field, Portal
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		mobid = atoi(row[MobId]);

		banish.message = row[Message];
		banish.field = atoi(row[Field]);
		banish.portal = row[Portal];

		banishinfo[mobid] = banish;
	}
}

namespace Functors {
	struct MorphFlags {
		void operator()(const string &cmp) {
			if (cmp == "superman") morph->superman = true;
		}
		MorphData *morph;
	};
}

void SkillDataProvider::loadMorphs() {
	morphinfo.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM morph_data");
	mysqlpp::UseQueryResult res = query.use();
	MorphData morph;
	int16_t morphid;

	using namespace Functors;

	enum MorphDataTable {
		Id = 0,
		Speed, Jump, Traction, Swim, Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		morph = MorphData();
		morphid = atoi(row[Id]);

		MorphFlags whoo = {&morph};
		runFlags(row[Flags], whoo);

		morph.speed = atoi(row[Speed]);
		morph.jump = atoi(row[Jump]);
		morph.traction = atof(row[Traction]);
		morph.swim = atof(row[Swim]);

		morphinfo[morphid] = morph;
	}
}

SkillLevelInfo * SkillDataProvider::getSkill(int32_t skill, uint8_t level) {
	if (skills.find(skill) != skills.end()) {
		if (skills[skill].find(level) != skills[skill].end()) {
			return &skills[skill][level];
		}
	}
	return nullptr;
}

MobSkillLevelInfo * SkillDataProvider::getMobSkill(uint8_t skill, uint8_t level) {
	if (mobskills.find(skill) != mobskills.end()) {
		if (mobskills[skill].find(level) != mobskills[skill].end()) {
			return &mobskills[skill][level];
		}
	}
	return nullptr;
}