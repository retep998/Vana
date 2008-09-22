/*
Copyright (C) 2008 Vana Development Team

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
#include "SkillsPacket.h"
#include "Timer/Container.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

// Buff Skills
void PlayerActiveBuffs::addBuff(int32_t skill, int32_t time) {
	clock_t skillExpire = time * 1000;
	Timer::Id id(Timer::Types::SkillTimer, skill, 0);
	new Timer::Timer(bind(&Skills::stopSkill, m_player, skill, true),
		id, m_player->getTimers(), skillExpire, false);

	m_buffs.push_back(skill);
}

void PlayerActiveBuffs::removeBuff(int32_t skill, bool fromTimer) {
	if (!fromTimer) {
		Timer::Id id(Timer::Types::SkillTimer, skill, 0);
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
	Timer::Id id(Timer::Types::SkillTimer, skill, 0);
	return m_player->getTimers()->checkTimer(id);
}

// Skill "acts"
void PlayerActiveBuffs::addAct(int32_t skill, Act act, int16_t value, int32_t time) {
	struct {
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
	} runAct = {m_player, skill, act, value};

	Timer::Id id(Timer::Types::SkillActTimer, act, 0);
	new Timer::Timer(runAct, id, getActTimer(skill), time, true);
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
		SkillActiveInfo mapSkill = getBuffMapInfo(1111002);
		playerSkill.vals[0] = combo + 1;
		SkillsPacket::useSkill(m_player, 1111002, buffTimeLeft(1111002), playerSkill, mapSkill, 0);
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
			int16_t x = Skills::skills[skillid][level].x;
			int16_t ratio = (m_player->getHP() * 100) / m_player->getMHP();
			bool change = false;
			if (m_berserk && ratio > x) { // If on and we're above Berserk HP, Berserk fails
				m_berserk = false;
				change = true;
			}
			else if (!m_berserk && ratio <= x) { // If off and we're below Berserk HP, let's rock
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

SkillActiveInfo PlayerActiveBuffs::getBuffMapInfo(int32_t skillid) {
	return activemapskill[skillid];
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
	if (activelevels.find(skillid) != activelevels.end())
		return activelevels[skillid];
	return 0;

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

void PlayerActiveBuffs::setBuffMapInfo(int32_t skillid, SkillActiveInfo skill) {
	activemapskill[skillid] = skill;
}

void PlayerActiveBuffs::setActiveSkillLevel(int32_t skillid, uint8_t level) {
	activelevels[skillid] = level;
}
