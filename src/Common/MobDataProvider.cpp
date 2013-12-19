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
#include "MobDataProvider.h"
#include "Database.h"
#include "GameConstants.h"
#include "InitializeCommon.h"
#include "StringUtilities.h"
#include <iomanip>
#include <iostream>
#include <string>

using std::string;
using Initializing::OutputWidth;
using StringUtilities::runFlags;

MobDataProvider * MobDataProvider::singleton = nullptr;

void MobDataProvider::loadData() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Mobs... ";

	loadAttacks();
	loadSkills();
	loadMobs();
	loadSummons();

	std::cout << "DONE" << std::endl;
}

void MobDataProvider::loadAttacks() {
	m_attacks.clear();
	int32_t mobId;
	MobAttackInfo mobAttack;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM mob_attacks");

	for (const auto &row : rs) {
		mobAttack = MobAttackInfo();
		runFlags(row.get<opt_string>("flags"), [&mobAttack](const string &cmp) {
			if (cmp == "deadly") mobAttack.deadlyAttack = true;
		});

		mobId = row.get<int32_t>("mobid");
		mobAttack.id = row.get<int8_t>("attackid");
		mobAttack.mpConsume = row.get<uint8_t>("mp_cost");
		mobAttack.mpBurn = row.get<uint16_t>("mp_burn");
		mobAttack.disease = row.get<uint8_t>("mob_skillid");
		mobAttack.level = row.get<uint8_t>("mob_skill_level");

		m_attacks[mobId].push_back(mobAttack);
	}
}

void MobDataProvider::loadSkills() {
	m_skills.clear();
	int32_t mobId;
	MobSkillInfo mobSkill;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM mob_skills");

	for (const auto &row : rs) {
		mobId = row.get<int32_t>("mobid");
		mobSkill.skillId = row.get<uint8_t>("skillid");
		mobSkill.level = row.get<uint8_t>("skill_level");
		mobSkill.effectAfter = row.get<int16_t>("effect_delay");

		m_skills[mobId].push_back(mobSkill);
	}
}

void MobDataProvider::loadMobs() {
	m_mobInfo.clear();
	int32_t mobId;
	MobInfo mob;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM mob_data");

	for (const auto &row : rs) {
		mob = std::make_shared<MobInfoRaw>();

		runFlags(row.get<opt_string>("flags"), [&mob](const string &cmp) {
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
		});

		mobId = row.get<int32_t>("mobid");
		mob->level = row.get<uint16_t>("mob_level");
		mob->hp = row.get<uint32_t>("hp");
		mob->mp = row.get<uint32_t>("mp");
		mob->hpRecovery = row.get<uint32_t>("hp_recovery");
		mob->mpRecovery = row.get<uint32_t>("mp_recovery");
		mob->selfDestruction = row.get<int32_t>("explode_hp");
		mob->exp = row.get<uint32_t>("experience");
		mob->link = row.get<int32_t>("link");
		mob->buff = row.get<int32_t>("death_buff");
		mob->removeAfter = row.get<int32_t>("death_after");
		mob->hpColor = row.get<int8_t>("hp_bar_color");
		mob->hpBackgroundColor = row.get<int8_t>("hp_bar_bg_color");
		mob->carnivalPoints = row.get<int8_t>("carnival_points");
		mob->avo = row.get<int16_t>("avoidability");
		mob->acc = row.get<int16_t>("accuracy");
		mob->speed = row.get<int16_t>("speed");
		mob->chaseSpeed = row.get<int16_t>("chase_speed");
		mob->wAtk = row.get<int16_t>("physical_attack");
		mob->wDef = row.get<int16_t>("physical_defense");
		mob->mAtk = row.get<int16_t>("magical_attack");
		mob->mDef = row.get<int16_t>("magical_defense");
		mob->traction = row.get<double>("traction");
		mob->damageSkill = row.get<int32_t>("damaged_by_skill_only");
		mob->damageMob = row.get<int32_t>("damaged_by_mob_only");
		mob->knockback = row.get<int32_t>("knockback");
		mob->summonType = row.get<int16_t>("summon_type");
		mob->fixedDamage = row.get<int32_t>("fixed_damage");

		mob->iceAttr = getElemModifier(row.get<string>("ice_modifier"));
		mob->fireAttr = getElemModifier(row.get<string>("fire_modifier"));
		mob->poisonAttr = getElemModifier(row.get<string>("poison_modifier"));
		mob->lightningAttr = getElemModifier(row.get<string>("lightning_modifier"));
		mob->holyAttr = getElemModifier(row.get<string>("holy_modifier"));
		mob->nonElemAttr = getElemModifier(row.get<string>("nonelemental_modifier"));

		mob->canFreeze = (!mob->boss && mob->iceAttr != MobElements::Immune && mob->iceAttr != MobElements::Strong);
		mob->canPoison = (!mob->boss && mob->poisonAttr != MobElements::Immune && mob->poisonAttr != MobElements::Strong);

		mob->skillCount = getSkillCount(mobId); // Relies on skills being loaded first
		m_mobInfo[mobId] = mob;
	}
}

void MobDataProvider::loadSummons() {
	int32_t mobId;
	int32_t summonId;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM mob_summons");

	for (const auto &row : rs) {
		mobId = row.get<int32_t>("mobid");
		summonId = row.get<int32_t>("summonid");

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
	return static_cast<uint8_t>(m_skills.find(mobId) != m_skills.end() ? m_skills[mobId].size() : 0);
}

int8_t MobDataProvider::getElemModifier(const string &elemAttr) {
	int8_t ret = MobElements::Normal;
	if (elemAttr == "immune") ret = MobElements::Immune;
	else if (elemAttr == "strong") ret = MobElements::Strong;
	else if (elemAttr == "weak") ret = MobElements::Weak;
	return ret;
}