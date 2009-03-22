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
#include "Buffs.h"
#include "Maps.h"
#include "Player.h"
#include "Randomizer.h"
#include "Skills.h"
#include "BuffsPacket.h"
#include "SkillsPacket.h"
#include "Timer/Container.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

// Buff Skills
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
			case ACT_HEAL: Skills::heal(player, value, skill); break;
			case ACT_HURT: Skills::hurt(player, value, skill); break;
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

// Combo attack stuff
void PlayerActiveBuffs::setCombo(uint8_t combo, bool sendPacket) {
	m_combo = combo;
	if (sendPacket) {
		SkillActiveInfo playerSkill = getBuffInfo(1111002);
		playerSkill.vals[0] = combo + 1;
		BuffsPacket::useSkill(m_player, 1111002, buffTimeLeft(1111002), playerSkill, 0);
	}
}

void PlayerActiveBuffs::addCombo() { // Add combo orbs
	if (getActiveSkillLevel(1111002) > 0) {
		int8_t advcombo = m_player->getSkills()->getSkillLevel(1120003);
		int8_t maxcombo = (int8_t) (advcombo > 0 ? Skills::skills[1120003][advcombo].x : Skills::skills[1111002][m_player->getSkills()->getSkillLevel(1111002)].x);
		if (m_combo == maxcombo)
			return;
		if (advcombo > 0 && Randomizer::Instance()->randShort(99) < Skills::skills[1120003][advcombo].prop)
			m_combo += 1; // 4th job skill gives chance to add second orb
		m_combo += 1;
		if (m_combo > maxcombo)
			m_combo = maxcombo;
		setCombo(m_combo, true);
	}
}

void PlayerActiveBuffs::checkBerserk(bool display) {
	if (m_player->getJob() == 132) { // Berserk calculations
		int32_t skillid = 1320006;
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

void PlayerActiveBuffs::deleteSkillMapEnterInfo(int32_t skillid) {
	for (size_t i = 0; i < activemapenterskill.size(); i++) {
		if (activemapenterskill[i].skill == skillid) {
			activemapenterskill.erase(activemapenterskill.begin() + i);
		}
	}
}

SkillMapEnterActiveInfo PlayerActiveBuffs::getSkillMapEnterInfo() {
	SkillMapEnterActiveInfo skill;
	for (size_t i = 0; i < activemapenterskill.size(); i++) {
		SkillMapActiveInfo cur = activemapenterskill[i];
		skill.types[cur.byte] += cur.type;
		if (cur.isvalue) {
			skill.val = cur.value;
			skill.isval = true;
		}
	}
	return skill;
}

SkillActiveInfo PlayerActiveBuffs::getBuffInfo(int32_t skillid) {
	return activeplayerskill[skillid];
}

uint8_t PlayerActiveBuffs::getActiveSkillLevel(int32_t skillid) {
	return activelevels.find(skillid) != activelevels.end() ? activelevels[skillid] : 0;
}

void PlayerActiveBuffs::setSkillMapEnterInfo(int32_t skillid, const vector<SkillMapActiveInfo> &skill) {
	// TEMP //
	for (size_t i = 0; i < activemapenterskill.size(); i++) {
		if (activemapenterskill[i].isvalue) {
			activemapenterskill.erase(activemapenterskill.begin() + i);
			break;
		}
	}
	//////////
	for (size_t i = 0; i < skill.size(); i++) {
		activemapenterskill.push_back(skill[i]);
	}
}

void PlayerActiveBuffs::setBuffInfo(int32_t skillid, SkillActiveInfo skill) {
	activeplayerskill[skillid] = skill;
}

void PlayerActiveBuffs::setActiveSkillLevel(int32_t skillid, uint8_t level) {
	activelevels[skillid] = level;
}

void PlayerActiveBuffs::increaseEnergyChargeLevel(int8_t targets) {
	if (m_energycharge == 0)
		startEnergyChargeTimer();
	m_energycharge += Skills::skills[5110001][m_player->getSkills()->getSkillLevel(5110001)].x * targets;
	if (m_energycharge > 10000) {
		m_energycharge = 10000;
		stopEnergyChargeTimer();
	}
	Buffs::Instance()->addBuff(m_player, 5110001, m_player->getSkills()->getSkillLevel(5110001), 0);
}

void PlayerActiveBuffs::decreaseEnergyChargeLevel() {
	m_energycharge -= 200; // Always the same
	if (m_energycharge < 0) {
		m_energycharge = 0;
		stopEnergyChargeTimer();
	}
	Buffs::Instance()->addBuff(m_player, 5110001, m_player->getSkills()->getSkillLevel(5110001), 0);
}

void PlayerActiveBuffs::resetEnergyChargeLevel() {
	m_energycharge = 0;
}

void PlayerActiveBuffs::startEnergyChargeTimer() {
	timeseed = static_cast<uint32_t>(clock());
	Timer::Id id(Timer::Types::BuffTimer, 5110001, timeseed); // Causes heap errors when it's a static number, but we need it for ID
	new Timer::Timer(bind(&PlayerActiveBuffs::decreaseEnergyChargeLevel, this),
		id, m_player->getTimers(), Timer::Time::fromNow(12 * 1000), 12 * 1000); // 12 Seconds
}

void PlayerActiveBuffs::stopEnergyChargeTimer() {
	Timer::Id id(Timer::Types::BuffTimer, 5110001, timeseed);
	m_player->getTimers()->removeTimer(id);
}