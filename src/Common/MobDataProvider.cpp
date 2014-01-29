/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "MobDataProvider.hpp"
#include "Algorithm.hpp"
#include "Database.hpp"
#include "GameConstants.hpp"
#include "InitializeCommon.hpp"
#include "StringUtilities.hpp"
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

auto MobDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Mobs... ";

	loadAttacks();
	loadSkills();
	loadMobs();
	loadSummons();

	std::cout << "DONE" << std::endl;
}

auto MobDataProvider::loadAttacks() -> void {
	m_attacks.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM mob_attacks");

	for (const auto &row : rs) {
		MobAttackInfo mobAttack;

		int32_t mobId = row.get<int32_t>("mobid");
		mobAttack.id = row.get<int8_t>("attackid");
		mobAttack.mpConsume = row.get<uint8_t>("mp_cost");
		mobAttack.mpBurn = row.get<uint16_t>("mp_burn");
		mobAttack.disease = row.get<uint8_t>("mob_skillid");
		mobAttack.level = row.get<uint8_t>("mob_skill_level");

		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&mobAttack](const string_t &cmp) {
			if (cmp == "deadly") mobAttack.deadlyAttack = true;
		});
		StringUtilities::runEnum(row.get<string_t>("attack_type"), [&mobAttack](const string_t &cmp) {
			if (cmp == "normal") mobAttack.attackType = MobAttackType::Normal;
			else if (cmp == "projectile") mobAttack.attackType = MobAttackType::Projectile;
			else if (cmp == "single_target") mobAttack.attackType = MobAttackType::SingleTarget;
			else if (cmp == "area_effect") mobAttack.attackType = MobAttackType::AreaEffect;
			else if (cmp == "area_effect_plus") mobAttack.attackType = MobAttackType::AreaEffectPlus;
		});

		m_attacks[mobId].push_back(mobAttack);
	}
}

auto MobDataProvider::loadSkills() -> void {
	m_skills.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM mob_skills");

	for (const auto &row : rs) {
		MobSkillInfo mobSkill;
		int32_t mobId = row.get<int32_t>("mobid");
		mobSkill.skillId = row.get<uint8_t>("skillid");
		mobSkill.level = row.get<uint8_t>("skill_level");
		mobSkill.effectAfter = row.get<int16_t>("effect_delay");

		m_skills[mobId].push_back(mobSkill);
	}
}

auto MobDataProvider::loadMobs() -> void {
	m_mobInfo.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM mob_data");

	for (const auto &row : rs) {
		auto mob = make_ref_ptr<MobInfo>();

		int32_t mobId = row.get<int32_t>("mobid");
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
		mob->damagedBySkill = row.get<int32_t>("damaged_by_skill_only");
		mob->damagedByMob = row.get<int32_t>("damaged_by_mob_only");
		mob->knockback = row.get<int32_t>("knockback");
		mob->summonType = row.get<int16_t>("summon_type");
		mob->fixedDamage = row.get<int32_t>("fixed_damage");

		auto getElement = [&row](const string_t &modifier) -> MobElementalAttribute {
			MobElementalAttribute ret;
			StringUtilities::runEnum(row.get<string_t>(modifier), [&ret](const string_t &cmp) {
				if (cmp == "normal") ret = MobElementalAttribute::Normal;
				else if (cmp == "immune") ret = MobElementalAttribute::Immune;
				else if (cmp == "strong") ret = MobElementalAttribute::Strong;
				else if (cmp == "weak") ret = MobElementalAttribute::Weak;
			});
			return ret;
		};

		mob->iceAttr = getElement("ice_modifier");
		mob->fireAttr = getElement("fire_modifier");
		mob->poisonAttr = getElement("poison_modifier");
		mob->lightningAttr = getElement("lightning_modifier");
		mob->holyAttr = getElement("holy_modifier");
		mob->nonElemAttr = getElement("nonelemental_modifier");

		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&mob](const string_t &cmp) {
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
			else if (cmp == "cannot_damage_player") mob->canDoBumpDamage = false;
			else if (cmp == "player_cannot_damage") mob->damageable = false;
		});

		mob->canFreeze = (!mob->boss && mob->iceAttr != MobElementalAttribute::Immune && mob->iceAttr != MobElementalAttribute::Strong);
		mob->canPoison = (!mob->boss && mob->poisonAttr != MobElementalAttribute::Immune && mob->poisonAttr != MobElementalAttribute::Strong);

		// Skill count relies on skills being loaded first
		auto kvp = m_skills.find(mobId);
		mob->skillCount = kvp != m_skills.end() ? kvp->second.size() : 0; 
		m_mobInfo[mobId] = mob;
	}
}

auto MobDataProvider::loadSummons() -> void {
	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM mob_summons");

	for (const auto &row : rs) {
		int32_t mobId = row.get<int32_t>("mobid");
		int32_t summonId = row.get<int32_t>("summonid");

		m_mobInfo[mobId]->summon.push_back(summonId);
	}
}

auto MobDataProvider::mobExists(int32_t mobId) const -> bool {
	return ext::is_element(m_mobInfo, mobId);
}

auto MobDataProvider::getMobInfo(int32_t mobId) const -> ref_ptr_t<MobInfo> {
	return m_mobInfo.find(mobId)->second;
}

auto MobDataProvider::getMobAttack(int32_t mobId, uint8_t index) const -> const MobAttackInfo * const {
	return ext::find_value_ptr(
		ext::find_value_ptr(m_attacks, mobId), index);
}

auto MobDataProvider::getMobSkill(int32_t mobId, uint8_t index) const -> const MobSkillInfo * const {
	return ext::find_value_ptr(
		ext::find_value_ptr(m_skills, mobId), index);
}

auto MobDataProvider::getSkills(int32_t mobId) const -> const vector_t<MobSkillInfo> & {
	return m_skills.find(mobId)->second;
}