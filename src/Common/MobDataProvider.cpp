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
#include "MobDataProvider.h"
#include "Database.h"
#include "GameConstants.h"
#include "InitializeCommon.h"
#include "StringUtilities.h"
#include <iostream>
#include <string>

using std::string;
using Initializing::outputWidth;
using StringUtilities::runFlags;

MobDataProvider * MobDataProvider::singleton = nullptr;

void MobDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Mobs... ";

	loadAttacks();
	loadSkills();
	loadMobs();
	loadSummons();

	std::cout << "DONE" << std::endl;
}

namespace Functors {
	struct MobAttackFlags {
		void operator() (const string &cmp) {
			if (cmp == "deadly") attack->deadlyAttack = true;
		}
		MobAttackInfo *attack;
	};
}

void MobDataProvider::loadAttacks() {
	m_attacks.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM mob_attacks");
	mysqlpp::UseQueryResult res = query.use();
	int32_t mobId;
	MobAttackInfo mobAttack;

	using namespace Functors;

	enum AttackData {
		MobId = 0,
		AttackId, MpCons, MpBurn, Disease, Level,
		AttackType, Element, Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		mobAttack = MobAttackInfo();
		MobAttackFlags whoo = {&mobAttack};
		runFlags(row[Flags], whoo);

		mobId = atoi(row[MobId]);
		mobAttack.id = atoi(row[AttackId]);
		mobAttack.mpConsume = atoi(row[MpCons]);
		mobAttack.mpBurn = atoi(row[MpBurn]);
		mobAttack.disease = atoi(row[Disease]);
		mobAttack.level = atoi(row[Level]);

		m_attacks[mobId].push_back(mobAttack);
	}
}

void MobDataProvider::loadSkills() {
	m_skills.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM mob_skills");
	mysqlpp::UseQueryResult res = query.use();
	int32_t mobId;
	MobSkillInfo mobSkill;

	enum SkillData {
		Id = 0,
		MobId, SkillId, Level, EffectAfter
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		mobId = atoi(row[MobId]);

		mobSkill.skillId = atoi(row[SkillId]);
		mobSkill.level = atoi(row[Level]);
		mobSkill.effectAfter = atoi(row[EffectAfter]);

		m_skills[mobId].push_back(mobSkill);
	}
}

namespace Functors {
	struct MobDataFlags {
		void operator() (const string &cmp) {
			if (cmp == "boss") mob->boss = true;
			else if (cmp == "undead") mob->undead = true;
			else if (cmp == "flying") mob->flying = true;
			else if (cmp == "friendly") mob->friendly = true;
			else if (cmp == "public_reward") mob->publicReward = true;
			else if (cmp == "explosive_reward") mob->explosiveReward = true;
			else if (cmp == "invincible") mob->invincible = true;
			else if (cmp == "auto_aggro") mob->autoAggro = true;
			else if (cmp == "damaged_by_normal_attacks_only") mob->onlyNormalAttacks = true;
			else if (cmp == "no_remove_on_death") mob->keepCorpse = true;
			else if (cmp == "cannot_damage_player") mob->canDamage = false;
			else if (cmp == "player_cannot_damage") mob->damageable = false;
		}
		MobInfo mob;
	};
}

void MobDataProvider::loadMobs() {
	m_mobInfo.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * from mob_data");
	mysqlpp::UseQueryResult res = query.use();
	int32_t mobId;
	MobInfo mob;

	using namespace Functors;

	enum MobData {
		MobId = 0,
		Level, Flags, Hp, Mp, HpRecovery,
		MpRecovery, SelfDestruct, Exp, Link, SummonType,
		Knockback, FixedDamage, DeathBuff, DeathAfter, Traction,
		DamageSkill, DamageMob, DropPeriod, HpBar, HpBarBg,
		CarnivalPoints, PhysicalAtt, PhysicalDef, MagicAtt, MagicDef,
		Accuracy, Avoidability, Speed, ChaseSpeed, IceMod,
		FireMod, PoisonMod, LightningMod, HolyMod, NonElementalMod
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		mob.reset(new MobInfoRaw());
		MobDataFlags whoo = {mob};
		runFlags(row[Flags], whoo);

		mobId = atoi(row[MobId]);
		mob->level = atoi(row[Level]);
		mob->hp = atoi(row[Hp]);
		mob->mp = atoi(row[Mp]);
		mob->hpRecovery = atoi(row[HpRecovery]);
		mob->mpRecovery = atoi(row[MpRecovery]);
		mob->selfDestruction = atoi(row[SelfDestruct]);
		mob->exp = atoi(row[Exp]);
		mob->link = atoi(row[Link]);
		mob->buff = atoi(row[DeathBuff]);
		mob->removeAfter = atoi(row[DeathAfter]);
		mob->hpColor = atoi(row[HpBar]);
		mob->hpBackgroundColor = atoi(row[HpBarBg]);
		mob->carnivalPoints = atoi(row[CarnivalPoints]);
		mob->avo = atoi(row[Avoidability]);
		mob->acc = atoi(row[Accuracy]);
		mob->speed = atoi(row[Speed]);
		mob->chaseSpeed = atoi(row[ChaseSpeed]);
		mob->wAtk = atoi(row[PhysicalAtt]);
		mob->wDef = atoi(row[PhysicalDef]);
		mob->mAtk = atoi(row[MagicAtt]);
		mob->mDef = atoi(row[MagicDef]);
		mob->traction = atof(row[Traction]);
		mob->damageSkill = atoi(row[DamageSkill]);
		mob->damageMob = atoi(row[DamageMob]);
		mob->knockback = atoi(row[Knockback]);
		mob->summonType = atoi(row[SummonType]);
		mob->fixedDamage = atoi(row[FixedDamage]);

		mob->iceAttr = getElemModifier(row[IceMod]);
		mob->fireAttr = getElemModifier(row[FireMod]);
		mob->poisonAttr = getElemModifier(row[PoisonMod]);
		mob->lightningAttr = getElemModifier(row[LightningMod]);
		mob->holyAttr = getElemModifier(row[HolyMod]);
		mob->nonElemAttr = getElemModifier(row[NonElementalMod]);

		mob->canFreeze = (!mob->boss && mob->iceAttr != MobElements::Immune && mob->iceAttr != MobElements::Strong);
		mob->canPoison = (!mob->boss && mob->poisonAttr != MobElements::Immune && mob->poisonAttr != MobElements::Strong);

		mob->skillCount = getSkillCount(mobId); // Relies on skills being loaded first
		m_mobInfo[mobId] = mob;
	}
}

void MobDataProvider::loadSummons() {
	mysqlpp::Query query = Database::getDataDb().query("SELECT * from mob_summons");
	mysqlpp::UseQueryResult res = query.use();
	int32_t mobId;
	int32_t summonId;

	enum SummonData {
		Id = 0,
		MobId, SummonId
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		mobId = atoi(row[MobId]);
		summonId = atoi(row[SummonId]);

		m_mobInfo[mobId]->summon.push_back(summonId);
	}
}

MobAttackInfo * MobDataProvider::getMobAttack(int32_t mobId, uint8_t index) {
	try {
		return &m_attacks[mobId].at(index);
	}
	catch (std::out_of_range) {
		std::cerr << "Attack does not exist for mobId " << mobId << " at index " << index << std::endl;
	}
	return nullptr;
}

MobSkillInfo * MobDataProvider::getMobSkill(int32_t mobId, uint8_t index) {
	try {
		return &m_skills[mobId].at(index);
	}
	catch (std::out_of_range) {
		std::cerr << "Skill does not exist for mobId " << mobId << " at index " << index << std::endl;
	}
	return nullptr;
}

uint8_t MobDataProvider::getSkillCount(int32_t mobId) {
	return (m_skills.find(mobId) != m_skills.end() ? m_skills[mobId].size() : 0);
}

int8_t MobDataProvider::getElemModifier(const string &elemAttr) {
	int8_t ret = MobElements::Normal;
	if (elemAttr == "immune") ret = MobElements::Immune;
	else if (elemAttr == "strong") ret = MobElements::Strong;
	else if (elemAttr == "weak") ret = MobElements::Weak;
	return ret;
}