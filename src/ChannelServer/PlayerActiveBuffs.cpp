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
#include "Player.h"
#include "PlayerSkills.h"
#include "Randomizer.h"
#include "Skills.h"
#include "SkillsPacket.h"
#include "Timer/Container.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

// Buff Skills
void PlayerActiveBuffs::addBuff(int skill, unsigned char level) {
	clock_t skillExpire = Skills::skills[skill][level].time * 1000;
	Timer::Id id(Timer::Types::SkillTimer, skill, 0);
	new Timer::Timer(bind(&Skills::stopSkill, m_player, skill, true),
		id, m_player->getTimers(), skillExpire, false);

	m_buffs.push_back(skill);
}

void PlayerActiveBuffs::removeBuff(int skill, bool fromTimer) {
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

int PlayerActiveBuffs::buffTimeLeft(int skill) {
	Timer::Id id(Timer::Types::SkillTimer, skill, 0);
	return m_player->getTimers()->checkTimer(id);
}

// Skill "acts"
void PlayerActiveBuffs::addAct(int skill, Act act, short value, int time) {
	struct {
		void operator()() {
			switch (act) {
				case ACT_HEAL: Skills::heal(player, value, skill); break;
				case ACT_HURT: Skills::hurt(player, value, skill); break;
			}
		}
		Player *player;
		int skill;
		Act act;
		short value;
	} runAct = {m_player, skill, act, value};

	Timer::Id id(Timer::Types::SkillActTimer, act, 0);
	new Timer::Timer(runAct, id, getActTimer(skill), time, true);
}

Timer::Container * PlayerActiveBuffs::getActTimer(int skill) {
	if (m_skill_acts.find(skill) == m_skill_acts.end()) {
		m_skill_acts[skill] = shared_ptr<Timer::Container>(new Timer::Container);
	}
	return m_skill_acts[skill].get();
}

void PlayerActiveBuffs::removeAct(int skill) {
	m_skill_acts.erase(skill);
}

// Combo attack stuff
void PlayerActiveBuffs::setCombo(char combo, bool sendPacket) {
	m_combo = combo;
	if (sendPacket) {
		SkillActiveInfo playerSkill = m_player->getSkills()->getSkillPlayerInfo(1111002);
		SkillActiveInfo mapSkill = m_player->getSkills()->getSkillMapInfo(1111002);
		playerSkill.vals[0] = combo + 1;
		SkillsPacket::useSkill(m_player, 1111002, buffTimeLeft(1111002), playerSkill, mapSkill, 0);
	}
}

void PlayerActiveBuffs::addCombo() { // Add combo orbs
	if (m_player->getSkills()->getActiveSkillLevel(1111002) > 0) {
		char advcombo = m_player->getSkills()->getSkillLevel(1120003);
		char maxcombo = (char) (advcombo > 0 ? Skills::skills[1120003][advcombo].x : Skills::skills[1111002][m_player->getSkills()->getSkillLevel(1111002)].x);
		
		if (m_combo == maxcombo) {
			return;
		}

		if (maxcombo > 5 && Randomizer::Instance()->randInt(99) < Skills::skills[1120003][advcombo].prop) {
			m_combo += 2; // 4th job skill gives chance to add second orb
		}
		else {
			m_combo += 1;
		}

		if (m_combo > maxcombo) {
			m_combo = maxcombo;
		}

		setCombo(m_combo, true);
	}
}

void PlayerActiveBuffs::checkBerserk(bool display) {
	if (m_player->getJob() == 132) { // Berserk calculations
		int skillid = 1320006;
		char level = m_player->getSkills()->getSkillLevel(skillid);
		if (level > 0) {
			float x = (float)Skills::skills[skillid][level].x;
			bool change = false;
			float ratio = (float)((float)m_player->getHP() / (float)m_player->getMHP() * 100.0); // Floating point math makes Bui an angry man
			if (m_berserk && ratio > x) { // If on and we're above Berserk HP, Berserk fails
				m_berserk = false;
				change = true;
			}
			else if (!m_berserk && ratio <= x) { // If off and we're below Berserk HP, let's rock
				m_berserk = true;
				change = true;
			}
			if (change || display)
				SkillsPacket::showBerserk(m_player, level, m_berserk, true);
		}
	}
}