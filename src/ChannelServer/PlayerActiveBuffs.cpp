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
#include "PlayerActiveBuffs.h"
#include "BuffsPacket.h"
#include "GameConstants.h"
#include "Maps.h"
#include "Player.h"
#include "Randomizer.h"
#include "Skills.h"
#include "SkillsPacket.h"
#include "Timer/Container.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

// Buff skills
void PlayerActiveBuffs::addBuff(int32_t skill, int32_t time) {
	clock_t skillExpire = time * 1000;
	Timer::Id id(Timer::Types::BuffTimer, skill, 0);
	new Timer::Timer(bind(&Skills::stopSkill, m_player, skill, true),
		id, m_player->getTimers(), Timer::Time::fromNow(skillExpire));

	m_buffs.push_back(skill);
}

void PlayerActiveBuffs::removeBuff(int32_t skill, bool fromTimer) {
	if (!fromTimer) {
		Timer::Id id(Timer::Types::BuffTimer, skill, 0);
		m_player->getTimers()->removeTimer(id);
	}
	removeAct(skill);
	m_buffs.remove(skill);
}

void PlayerActiveBuffs::removeBuff() {
	while (m_buffs.size() > 0) {
		removeBuff(*m_buffs.begin());
	}
}

int32_t PlayerActiveBuffs::buffTimeLeft(int32_t skill) {
	Timer::Id id(Timer::Types::BuffTimer, skill, 0);
	return m_player->getTimers()->checkTimer(id);
}

// Skill "acts"
struct RunAct {
	void operator()() {
		switch (act) {
			case ActHeal: Skills::heal(player, value, skill); break;
			case ActHurt: Skills::hurt(player, value, skill); break;
		}
	}
	Player *player;
	int32_t skill;
	Act act;
	int16_t value;
};

void PlayerActiveBuffs::addAct(int32_t skill, Act act, int16_t value, int32_t time) {
	RunAct runAct;
	runAct.player = m_player;
	runAct.skill = skill;
	runAct.act = act;
	runAct.value = value;

	Timer::Id id(Timer::Types::SkillActTimer, act, 0);
	new Timer::Timer(runAct, id, getActTimer(skill), 0, time);
}

Timer::Container * PlayerActiveBuffs::getActTimer(int32_t skill) {
	if (m_skill_acts.find(skill) == m_skill_acts.end()) {
		m_skill_acts[skill] = shared_ptr<Timer::Container>(new Timer::Container);
	}
	return m_skill_acts[skill].get();
}

void PlayerActiveBuffs::removeAct(int32_t skill) {
	m_skill_acts.erase(skill);
}

// Map entry stuff
void PlayerActiveBuffs::deleteMapEntryBuffInfo(ActiveMapBuff &buff) {
	size_t vals = 0;
	for (size_t i = 0; i < buff.bytes.size(); i++) {
		uint8_t byte = buff.bytes[i];
		m_mapbuffs.types[byte] -= buff.types[i];
		if (!m_mapbuffs.types[byte] == 0) {
			m_mapbuffs.val[byte] = false;
		}
		if (buff.usevals[i]) {
			m_mapbuffs.values[byte].clear(); // Might not be so swell, I'd have to think about it
		}
	}
}

void PlayerActiveBuffs::addMapEntryBuffInfo(ActiveMapBuff &buff) {
	size_t vals = 0;
	for (size_t i = 0; i < buff.bytes.size(); i++) {
		uint8_t byte = buff.bytes[i];
		m_mapbuffs.types[byte] += buff.types[i];
		if (!m_mapbuffs.val[byte]) {
			m_mapbuffs.val[byte] = buff.usevals[i];
		}
		if (buff.usevals[i]) {
			m_mapbuffs.values[byte].push_back(buff.values[vals++]);
		}
	}
}

MapEntryBuffs PlayerActiveBuffs::getMapEntryBuffs() {
	return m_mapbuffs;
}

void PlayerActiveBuffs::setMountInfo(int32_t skillid, int32_t mountid) {
	m_mapbuffs.mountskill = skillid;
	m_mapbuffs.mountid = mountid;
}

void PlayerActiveBuffs::setMapEntryBuffs(MapEntryBuffs &buffs) {
	m_mapbuffs = buffs;
}

// Active skill levels
uint8_t PlayerActiveBuffs::getActiveSkillLevel(int32_t skillid) {
	return m_activelevels.find(skillid) != m_activelevels.end() ? m_activelevels[skillid] : 0;
}

void PlayerActiveBuffs::setActiveSkillLevel(int32_t skillid, uint8_t level) {
	m_activelevels[skillid] = level;
}

// Buff addition/removal
void PlayerActiveBuffs::addBuffInfo(int32_t skillid, const vector<Buff> &buffs) {
	for (size_t i = 0; i < buffs.size(); i++) {
		Buff cur = buffs[i];
		m_activebuffsbytype[cur.byte][cur.type] = skillid;
	}
}

ActiveBuff PlayerActiveBuffs::removeBuffInfo(int32_t skillid, const vector<Buff> &buffs) {
	ActiveBuff ret;
	for (size_t i = 0; i < buffs.size(); i++) {
		Buff cur = buffs[i];
		if (m_activebuffsbytype[cur.byte][cur.type] == skillid) { // Make sure that the buff types are still affected by the skill
			m_activebuffsbytype[cur.byte].erase(cur.type);
			ret.types[cur.byte] += cur.type;
		}
	}
	return ret;
}

void PlayerActiveBuffs::setActiveBuffsByType(ActiveBuffsByType &buffs) {
	m_activebuffsbytype = buffs;
}

// Specific skill stuff
void PlayerActiveBuffs::setCombo(uint8_t combo, bool sendPacket) {
	m_combo = combo;
	if (sendPacket) {
		int32_t skillid = Jobs::Crusader::ComboAttack;
		int32_t timeleft = buffTimeLeft(skillid) / 1000;
		ActiveBuff playerskill = Buffs::Instance()->parseBuffInfo(m_player, skillid, getActiveSkillLevel(skillid));
		ActiveMapBuff mapskill = Buffs::Instance()->parseBuffMapInfo(m_player, skillid, getActiveSkillLevel(skillid));
		BuffsPacket::useSkill(m_player, skillid, timeleft, playerskill, mapskill, 0);
	}
}

void PlayerActiveBuffs::addCombo() { // Add orbs
	if (getActiveSkillLevel(Jobs::Crusader::ComboAttack) > 0) {
		int8_t advcombo = m_player->getSkills()->getSkillLevel(Jobs::Hero::AdvancedComboAttack);
		int8_t maxcombo = (int8_t) (advcombo > 0 ? Skills::skills[Jobs::Hero::AdvancedComboAttack][advcombo].x : Skills::skills[Jobs::Crusader::ComboAttack][m_player->getSkills()->getSkillLevel(Jobs::Crusader::ComboAttack)].x);
		if (m_combo == maxcombo)
			return;
		if (advcombo > 0 && Randomizer::Instance()->randShort(99) < Skills::skills[Jobs::Hero::AdvancedComboAttack][advcombo].prop)
			m_combo += 1; // 4th job skill gives chance to add second orb
		m_combo += 1;
		if (m_combo > maxcombo)
			m_combo = maxcombo;
		setCombo(m_combo, true);
	}
}

void PlayerActiveBuffs::checkBerserk(bool display) {
	if (m_player->getJob() == 132) { // Berserk calculations
		int32_t skillid = Jobs::DarkKnight::Berserk;
		int8_t level = m_player->getSkills()->getSkillLevel(skillid);
		if (level > 0) {
			int16_t r_hp = m_player->getMHP() * Skills::skills[skillid][level].x / 100;
			int16_t hp = m_player->getHP();
			bool change = false;
			if (m_berserk && hp > r_hp) { // If on and we're above Berserk HP, Berserk fails
				m_berserk = false;
				change = true;
			}
			else if (!m_berserk && hp <= r_hp) { // If off and we're below Berserk HP, let's rock
				m_berserk = true;
				change = true;
			}
			if (change || display)
				SkillsPacket::showBerserk(m_player, level, m_berserk);
		}
	}
}

void PlayerActiveBuffs::increaseEnergyChargeLevel(int8_t targets) {
	if (m_energycharge != 10000 && targets > 0) {
		Timer::Id id(Timer::Types::BuffTimer, Jobs::Marauder::EnergyCharge, m_timeseed);
		if (m_player->getTimers()->checkTimer(id) > 0)
			stopEnergyChargeTimer();
		startEnergyChargeTimer();
		m_energycharge += Skills::skills[Jobs::Marauder::EnergyCharge][m_player->getSkills()->getSkillLevel(Jobs::Marauder::EnergyCharge)].x * targets;
		if (m_energycharge > 10000) {
			m_energycharge = 10000;
			stopEnergyChargeTimer();
		}
		Buffs::Instance()->addBuff(m_player, Jobs::Marauder::EnergyCharge, m_player->getSkills()->getSkillLevel(Jobs::Marauder::EnergyCharge), 0);
	}
}

void PlayerActiveBuffs::decreaseEnergyChargeLevel() {
	m_energycharge -= 200; // Always the same
	if (m_energycharge < 0) {
		m_energycharge = 0;
	}
	else {
		startEnergyChargeTimer();
	}
	Buffs::Instance()->addBuff(m_player, Jobs::Marauder::EnergyCharge, m_player->getSkills()->getSkillLevel(Jobs::Marauder::EnergyCharge), 0);
}

void PlayerActiveBuffs::resetEnergyChargeLevel() {
	m_energycharge = 0;
}

void PlayerActiveBuffs::startEnergyChargeTimer() {
	m_timeseed = static_cast<uint32_t>(clock());
	Timer::Id id(Timer::Types::BuffTimer, Jobs::Marauder::EnergyCharge, m_timeseed); // Causes heap errors when it's a static number, but we need it for ID
	new Timer::Timer(bind(&PlayerActiveBuffs::decreaseEnergyChargeLevel, this),
		id, m_player->getTimers(), Timer::Time::fromNow(10 * 1000)); // 10 seconds
}

void PlayerActiveBuffs::setEnergyChargeLevel(int16_t chargelevel, bool startTimer) {
	m_energycharge = chargelevel;
	if (startTimer) {
		startEnergyChargeTimer();
	}
}

void PlayerActiveBuffs::stopEnergyChargeTimer() {
	Timer::Id id(Timer::Types::BuffTimer, Jobs::Marauder::EnergyCharge, m_timeseed);
	m_player->getTimers()->removeTimer(id);
}

void PlayerActiveBuffs::stopBooster() {
	if (m_activebooster != 0) {
		Skills::stopSkill(m_player, m_activebooster);
	}
}

void PlayerActiveBuffs::stopCharge() {
	if (m_activecharge != 0) {
		Skills::stopSkill(m_player, m_activecharge);
	}
}

bool PlayerActiveBuffs::hasIceCharge() const {
	return (m_activecharge == Jobs::WhiteKnight::BwIceCharge || m_activecharge == Jobs::WhiteKnight::SwordIceCharge);
}

const bool PlayerActiveBuffs::hasInfinity() {
	return (getActiveSkillLevel(Jobs::FPArchMage::Infinity) > 0 || getActiveSkillLevel(Jobs::ILArchMage::Infinity) > 0 || getActiveSkillLevel(Jobs::Bishop::Infinity) > 0);
}

const bool PlayerActiveBuffs::hasMesoUp() {
	return getActiveSkillLevel(Jobs::Hermit::MesoUp) > 0;
}

const bool PlayerActiveBuffs::hasMagicGuard() {
	return getActiveSkillLevel(Jobs::Magician::MagicGuard) > 0;
}

const bool PlayerActiveBuffs::hasHolySymbol() {
	return (getActiveSkillLevel(Jobs::Priest::HolySymbol) > 0 || getActiveSkillLevel(Jobs::SuperGM::HolySymbol) > 0);
}

const int32_t PlayerActiveBuffs::getHolySymbol() {
	return (getActiveSkillLevel(Jobs::Priest::HolySymbol) > 0 ? Jobs::Priest::HolySymbol : Jobs::SuperGM::HolySymbol);
}

const bool PlayerActiveBuffs::hasPowerStance() {
	// Energy Charge exhibits Power Stance effect as well
	return (getActiveSkillLevel(Jobs::Hero::PowerStance) > 0 || getActiveSkillLevel(Jobs::Paladin::PowerStance) > 0 || getActiveSkillLevel(Jobs::DarkKnight::PowerStance) > 0 || getActiveSkillLevel(Jobs::Marauder::EnergyCharge) > 0);
}

const int32_t PlayerActiveBuffs::getPowerStance() {
	int32_t skillid = 0;
	if (getActiveSkillLevel(Jobs::Hero::PowerStance) > 0)
		skillid = Jobs::Hero::PowerStance;
	else if (getActiveSkillLevel(Jobs::Paladin::PowerStance) > 0)
		skillid = Jobs::Paladin::PowerStance;
	else if (getActiveSkillLevel(Jobs::DarkKnight::PowerStance) > 0)
		skillid = Jobs::DarkKnight::PowerStance;
	else if (getActiveSkillLevel(Jobs::Marauder::EnergyCharge) > 0)
		skillid = Jobs::Marauder::EnergyCharge;
	return skillid;
}

const bool PlayerActiveBuffs::hasHyperBody() {
	return (getActiveSkillLevel(Jobs::Spearman::HyperBody) > 0 || getActiveSkillLevel(Jobs::SuperGM::HyperBody) > 0);
}

const int32_t PlayerActiveBuffs::getHyperBody() {
	return (getActiveSkillLevel(Jobs::Spearman::HyperBody) > 0 ? Jobs::Spearman::HyperBody : Jobs::SuperGM::HyperBody);
}