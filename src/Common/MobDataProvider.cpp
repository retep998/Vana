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
#include "MobDataProvider.h"
#include "Database.h"
#include "GameConstants.h"
#include "InitializeCommon.h"
#include "StringUtilities.h"
#include <iostream>
#include <string>

using std::string;
using Initializing::outputWidth;
using StringUtilities::atob;
using StringUtilities::runFlags;

MobDataProvider * MobDataProvider::singleton = 0;

void MobDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Mobs... ";

	loadAttacks();
	loadSkills();
	loadMobs();
	loadSummons();

	std::cout << "DONE" << std::endl;
}

void MobDataProvider::loadAttacks() {
	attacks.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM mob_attacks");
	mysqlpp::UseQueryResult res = query.use();
	int32_t mobid;
	MobAttackInfo mobattack;

	struct MobAttackFunctor {
		void operator() (const string &cmp) {
			if (cmp == "deadly") attack->deadlyattack = true;
		}
		MobAttackInfo *attack;
	};

	enum AttackData {
		MobId = 0,
		AttackId, MpCons, MpBurn, Disease, Level,
		Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		mobattack = MobAttackInfo();
		MobAttackFunctor whoo = {&mobattack};
		runFlags(row[Flags], whoo);

		mobid = atoi(row[MobId]);
		mobattack.id = atoi(row[AttackId]);
		mobattack.mpconsume = atoi(row[MpCons]);
		mobattack.mpburn = atoi(row[MpBurn]);
		mobattack.disease = atoi(row[Disease]);
		mobattack.level = atoi(row[Level]);

		attacks[mobid].push_back(mobattack);
	}
}

void MobDataProvider::loadSkills() {
	skills.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM mob_skills");
	mysqlpp::UseQueryResult res = query.use();
	int32_t mobid;
	MobSkillInfo mobskill;

	enum SkillData {
		Id = 0,
		MobId, SkillId, Level, EffectAfter
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		mobid = atoi(row[MobId]);

		mobskill.skillid = atoi(row[SkillId]);
		mobskill.level = atoi(row[Level]);
		mobskill.effectAfter = atoi(row[EffectAfter]);

		skills[mobid].push_back(mobskill);
	}
}

void MobDataProvider::loadMobs() {
	mobinfo.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * from mob_data");
	mysqlpp::UseQueryResult res = query.use();
	int32_t mobid;
	MobInfo mob;

	struct MobDataFunctor {
		void operator() (const string &cmp) {
			if (cmp == "boss") mob->boss = true;
			else if (cmp == "undead") mob->undead = true;
			else if (cmp == "flying") mob->flying = true;
			else if (cmp == "friendly") mob->friendly = true;
			else if (cmp == "public_reward") mob->publicreward = true;
			else if (cmp == "explosive_reward") mob->explosivereward = true;
			else if (cmp == "invincible") mob->invincible = true;
			else if (cmp == "auto_aggro") mob->autoaggro = true;
			else if (cmp == "damaged_by_normal_attacks_only") mob->onlynormalattacks = true;
			else if (cmp == "no_remove_on_death") mob->keepcorpse = true;
			else if (cmp == "cannot_damage_player") mob->candamage = false;
			else if (cmp == "player_cannot_damage") mob->damageable = false;
		}
		MobInfo *mob;
	};

	enum MobData {
		MobId = 0,
		Level, Flags, Hp, Mp, HpRecovery,
		MpRecovery, SelfDestruct, Exp, Link, SummonType,
		Knockback, FixedDamage, DeathBuff, DeathAfter, Traction,
		DamageSkill, DamageMob, HpBar, HpBarBg, CarnivalPoints,
		PhysicalAtt, PhysicalDef, MagicAtt, MagicDef, Accuracy,
		Avoidability, Speed, ChaseSpeed, IceMod, FireMod,
		PoisonMod, LightningMod, HolyMod, NonElementalMod
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		mob = MobInfo();
		MobDataFunctor whoo = {&mob};
		runFlags(row[Flags], whoo);

		mobid = atoi(row[MobId]);
		mob.level = atoi(row[Level]);
		mob.hp = atoi(row[Hp]);
		mob.mp = atoi(row[Mp]);
		mob.hprecovery = atoi(row[HpRecovery]);
		mob.mprecovery = atoi(row[MpRecovery]);
		mob.selfdestruction = atoi(row[SelfDestruct]);
		mob.exp = atoi(row[Exp]);
		mob.link = atoi(row[Link]);
		mob.buff = atoi(row[DeathBuff]);
		mob.removeafter = atoi(row[DeathAfter]);
		mob.hpcolor = atoi(row[HpBar]);
		mob.hpbgcolor = atoi(row[HpBarBg]);
		mob.cp = atoi(row[CarnivalPoints]);
		mob.avo = atoi(row[Avoidability]);
		mob.acc = atoi(row[Accuracy]);
		mob.speed = atoi(row[Speed]);
		mob.chasespeed = atoi(row[ChaseSpeed]);
		mob.watk = atoi(row[PhysicalAtt]);
		mob.wdef = atoi(row[PhysicalDef]);
		mob.matk = atoi(row[MagicAtt]);
		mob.mdef = atoi(row[MagicDef]);
		mob.traction = atof(row[Traction]);
		mob.damageskill = atoi(row[DamageSkill]);
		mob.damagemob = atoi(row[DamageMob]);
		mob.knockback = atoi(row[Knockback]);
		mob.summontype = atoi(row[SummonType]);
		mob.fixeddamage = atoi(row[FixedDamage]);

		mob.iceattr = getElemModifier(row[IceMod]);
		mob.fireattr = getElemModifier(row[FireMod]);
		mob.poisonattr = getElemModifier(row[PoisonMod]);
		mob.lightningattr = getElemModifier(row[LightningMod]);
		mob.holyattr = getElemModifier(row[HolyMod]);
		mob.nonelemattr = getElemModifier(row[NonElementalMod]);

		mob.canfreeze = (!mob.boss && mob.iceattr != MobElements::Immune && mob.iceattr != MobElements::Strong);
		mob.canpoison = (!mob.boss && mob.poisonattr != MobElements::Immune && mob.poisonattr != MobElements::Strong);

		mob.skillcount = getSkillCount(mobid); // Relies on skills being loaded first
		mobinfo[mobid] = mob;
	}
}

void MobDataProvider::loadSummons() {
	mysqlpp::Query query = Database::getDataDB().query("SELECT * from mob_summons");
	mysqlpp::UseQueryResult res = query.use();
	int32_t mobid;
	int32_t summonid;

	enum SummonData {
		Id = 0,
		MobId, SummonId
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		mobid = atoi(row[MobId]);
		summonid = atoi(row[SummonId]);

		mobinfo[mobid].summon.push_back(summonid);
	}
}

MobAttackInfo * MobDataProvider::getMobAttack(int32_t mobid, uint8_t index) {
	try {
		return &attacks[mobid].at(index);
	}
	catch (std::out_of_range) {
		std::cout << "Attack does not exist for mobid " << mobid << " at index " << index << std::endl;
	}
	return 0;
}

MobSkillInfo * MobDataProvider::getMobSkill(int32_t mobid, uint8_t index) {
	try {
		return &skills[mobid].at(index);
	}
	catch (std::out_of_range) {
		std::cout << "Skill does not exist for mobid " << mobid << " at index " << index << std::endl;
	}
	return 0;
}

uint8_t MobDataProvider::getSkillCount(int32_t mobid) {
	return (skills.find(mobid) != skills.end() ? skills[mobid].size() : 0);
}

int8_t MobDataProvider::getElemModifier(const string &elemattr) {
	int8_t ret = MobElements::Normal;
	if (elemattr == "immune") ret = MobElements::Immune;
	else if (elemattr == "strong") ret = MobElements::Strong;
	else if (elemattr == "weak") ret = MobElements::Weak;
	return ret;
}