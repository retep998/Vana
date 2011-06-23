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

using Initializing::OutputWidth;
using StringUtilities::runFlags;

SkillDataProvider * SkillDataProvider::singleton = nullptr;

void SkillDataProvider::loadData() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Skills... ";

	loadPlayerSkills();
	loadMobSkills();
	loadMobSummons();
	loadBanishData();
	loadMorphs();

	std::cout << "DONE" << std::endl;
}

void SkillDataProvider::loadPlayerSkills() {
	m_skillMaxLevels.clear();
	m_skills.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM skill_player_level_data");
	mysqlpp::UseQueryResult res = query.use();
	SkillLevelInfo level;
	int32_t skillId;
	uint8_t skillLevel;

	enum SkillData {
		SkillId = 0,
		Level, MobCount, HitCount, Range, Time,
		Mp, Hp, Damage, FixedDamage, CriticalDamage,
		Mastery, OptionalItem, Item, ItemCount, BulletCon,
		MoneyCon, X, Y, Speed, Jump,
		Str, Watk, Wdef, Matk, Mdef,
		Acc, Avoid, HpP, MpP, Prop,
		Morph, LTX, LTY, RBX, RBY,
		Cooldown
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		skillId = atoi(row[SkillId]);
		skillLevel = atoi(row[Level]);

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
		level.hpProp = atoi(row[HpP]);
		level.mpProp = atoi(row[MpP]);
		level.prop = atoi(row[Prop]);
		level.morph = atoi(row[Morph]);
		level.lt = Pos(atoi(row[LTX]), atoi(row[LTY]));
		level.rb = Pos(atoi(row[RBX]), atoi(row[RBY]));
		level.coolTime = atoi(row[Cooldown]);

		m_skills[skillId][skillLevel] = level;
		if (m_skillMaxLevels.find(skillId) == m_skillMaxLevels.end() || m_skillMaxLevels[skillId] < skillLevel) {
			m_skillMaxLevels[skillId] = skillLevel;
		}
	}
}

void SkillDataProvider::loadMobSkills() {
	m_mobSkills.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM skill_mob_data");
	mysqlpp::UseQueryResult res = query.use();
	uint8_t skillId;
	uint8_t level;

	MobSkillLevelInfo mobLevel;

	enum MobSkills {
		SkillId = 0,
		SkillLevel, Time, Mp, X, Y,
		Prop, Count, Interval, LTX, RBX,
		LTY, RBY, Hp, Limit, SummonEffect
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		skillId = atoi(row[SkillId]);
		level = atoi(row[SkillLevel]);

		mobLevel.time = atoi(row[Time]);
		mobLevel.mp = atoi(row[Mp]);
		mobLevel.x = atoi(row[X]);
		mobLevel.y = atoi(row[Y]);
		mobLevel.prop = atoi(row[Prop]);
		mobLevel.count = atoi(row[Count]);
		mobLevel.interval = atoi(row[Interval]);
		mobLevel.lt.x = atoi(row[LTX]);
		mobLevel.rb.x = atoi(row[RBX]);
		mobLevel.lt.y = atoi(row[LTY]);
		mobLevel.rb.y = atoi(row[RBY]);
		mobLevel.hp = atoi(row[Hp]);
		mobLevel.limit = atoi(row[Limit]);
		mobLevel.summonEffect = atoi(row[SummonEffect]);

		m_mobSkills[skillId][level] = mobLevel;
	}
}

void SkillDataProvider::loadMobSummons() {
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM skill_mob_summons");
	mysqlpp::UseQueryResult res = query.use();
	uint8_t level;
	int32_t mobId;

	enum MobSummons {
		SkillLevel = 0,
		MobId
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		level = atoi(row[SkillLevel]);
		mobId = atoi(row[MobId]);

		m_mobSkills[MobSkills::Summon][level].summons.push_back(mobId);
	}
}

void SkillDataProvider::loadBanishData() {
	m_banishInfo.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM skill_mob_banish_data");
	mysqlpp::UseQueryResult res = query.use();
	BanishField banish;
	int32_t mobId;

	enum BanishData {
		MobId = 0,
		Message, Field, Portal
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		mobId = atoi(row[MobId]);

		banish.message = row[Message];
		banish.field = atoi(row[Field]);
		banish.portal = row[Portal];

		m_banishInfo[mobId] = banish;
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
	m_morphInfo.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM morph_data");
	mysqlpp::UseQueryResult res = query.use();
	MorphData morph;
	int16_t morphId;

	using namespace Functors;

	enum MorphDataTable {
		Id = 0,
		Speed, Jump, Traction, Swim, Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		morph = MorphData();
		morphId = atoi(row[Id]);

		MorphFlags whoo = {&morph};
		runFlags(row[Flags], whoo);

		morph.speed = atoi(row[Speed]);
		morph.jump = atoi(row[Jump]);
		morph.traction = atof(row[Traction]);
		morph.swim = atof(row[Swim]);

		m_morphInfo[morphId] = morph;
	}
}

SkillLevelInfo * SkillDataProvider::getSkill(int32_t skill, uint8_t level) {
	if (m_skills.find(skill) != m_skills.end()) {
		if (m_skills[skill].find(level) != m_skills[skill].end()) {
			return &m_skills[skill][level];
		}
	}
	return nullptr;
}

MobSkillLevelInfo * SkillDataProvider::getMobSkill(uint8_t skill, uint8_t level) {
	if (m_mobSkills.find(skill) != m_mobSkills.end()) {
		if (m_mobSkills[skill].find(level) != m_mobSkills[skill].end()) {
			return &m_mobSkills[skill][level];
		}
	}
	return nullptr;
}