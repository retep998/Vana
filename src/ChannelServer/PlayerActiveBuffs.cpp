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
#include "NewTimer.h"
#include "Player.h"
#include "PlayerSkills.h"
#include "Randomizer.h"
#include "Skills.h"
#include "SkillsPacket.h"
#include "SkillTimer.h"
#include <boost/bind.hpp>

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

	NewTimer::OneTimer::Id id(NewTimer::Types::SkillActTimer, skill, act);
	new NewTimer::OneTimer(runAct, id, m_player->getTimers(), time, true);

	m_skill_acts[skill].push_back(act);
}

void PlayerActiveBuffs::removeAct(int skill, Act act) {
	NewTimer::OneTimer::Id id(NewTimer::Types::SkillActTimer, skill, act);
	m_player->getTimers()->removeTimer(id);

	m_skill_acts[skill].remove(act);
}

void PlayerActiveBuffs::removeAct(int skill) {
	for (list<Act>::iterator iter = m_skill_acts[skill].begin(); iter != m_skill_acts[skill].end(); iter++) {
		removeAct(skill, *iter);
	}
}

void PlayerActiveBuffs::removeAct() {
	for (unordered_map<int, list<Act>>::iterator iter = m_skill_acts.begin(); iter != m_skill_acts.end(); iter++) {
		removeAct(iter->first);
	}
}

// Combo attack stuff
void PlayerActiveBuffs::setCombo(char combo, bool sendPacket) {
	m_combo = combo;
	if (sendPacket) {
		SkillActiveInfo playerSkill = m_player->getSkills()->getSkillPlayerInfo(1111002);
		SkillActiveInfo mapSkill = m_player->getSkills()->getSkillMapInfo(1111002);
		playerSkill.vals[0] = combo + 1;
		SkillsPacket::useSkill(m_player, 1111002, SkillTimer::Instance()->skillTime(m_player, 1111002), playerSkill, mapSkill, 0);
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
