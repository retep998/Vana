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
#include "PlayerSkills.h"
#include "Player.h"
#include "Randomizer.h"
#include "SkillTimer.h"
#include "Skills.h"
#include "SkillsPacket.h"
#include "MySQLM.h"

void PlayerSkills::addSkillLevel(int skillid, unsigned char amount, bool sendpacket) {
	if (playerskills.find(skillid) != playerskills.end())
		playerskills[skillid] += amount;
	else
		playerskills[skillid] = amount;

	// Keep people from adding too much SP and prevent it from going negative
	if (playerskills[skillid] > Skills::maxlevels[skillid])
		playerskills[skillid] = Skills::maxlevels[skillid];
	else if (playerskills[skillid] <= 0)
		playerskills[skillid] = 0;

	if (sendpacket) {
		char maxlevel = 0;
		if (FORTHJOB_SKILL(skillid)) {
			maxlevel = getMaxSkillLevel(skillid);
		}
		SkillsPacket::addSkill(player, skillid, getSkillLevel(skillid), maxlevel);
	}
}

void PlayerSkills::deleteSkillMapEnterInfo(int skillid) {
	for (size_t i = 0; i < activemapenterskill.size(); i++) {
		if (activemapenterskill[i].skill == skillid) {
			activemapenterskill.erase(activemapenterskill.begin()+i);
		}
	}
}

unsigned char PlayerSkills::getSkillLevel(int skillid) {
	if (playerskills.find(skillid) != playerskills.end())
		return playerskills[skillid];
	return 0;
}

unsigned char PlayerSkills::getMaxSkillLevel(int skillid) {
	// Get max level for 4th job skills
	if (maxlevels.find(skillid) != maxlevels.end())
		return maxlevels[skillid];
	return 0;
}

SkillActiveInfo PlayerSkills::getSkillMapInfo(int skillid) {
	return activemapskill[skillid];
}

SkillMapEnterActiveInfo PlayerSkills::getSkillMapEnterInfo() {
	SkillMapEnterActiveInfo skill;
	for (size_t i = 0; i < activemapenterskill.size(); i++) {
		skill.types[activemapenterskill[i].byte] += activemapenterskill[i].type;
		if (activemapenterskill[i].isvalue) {
			skill.val = activemapenterskill[i].value;
			skill.isval = true;
		}
	}
	return skill;
}

SkillActiveInfo PlayerSkills::getSkillPlayerInfo(int skillid) {
	return activeplayerskill[skillid];
}

unsigned char PlayerSkills::getActiveSkillLevel(int skillid) {
	if (activelevels.find(skillid) != activelevels.end())
		return activelevels[skillid];
	return 0;

}

void PlayerSkills::setSkillMapEnterInfo(int skillid, vector <SkillMapActiveInfo> skill) {
	// TEMP //
	for (size_t i = 0; i < activemapenterskill.size(); i++) { 
		if (activemapenterskill[i].isvalue) {
			activemapenterskill.erase(activemapenterskill.begin()+i);
			break;
		}
	}
	//////////
	for (size_t i = 0; i < skill.size(); i++) {
		activemapenterskill.push_back(skill[i]);
	}
}

void PlayerSkills::setSkillPlayerInfo(int skillid, SkillActiveInfo skill) {
	activeplayerskill[skillid] = skill;
}

void PlayerSkills::setSkillMapInfo(int skillid, SkillActiveInfo skill) {
	activemapskill[skillid] = skill;
}

void PlayerSkills::setActiveSkillLevel(int skillid, int level) {
	activelevels[skillid] = level;
}
// Combo attack stuff
void PlayerSkills::setCombo(char combo, bool sendPacket) {
	this->combo = combo;
	if (sendPacket) {
		activeplayerskill[1111002].vals[0] = combo + 1;
		SkillsPacket::useSkill(this->player, 1111002, SkillTimer::Instance()->skillTime(player, 1111002), activeplayerskill[1111002], activemapskill[1111002], 0);
	}
}

void PlayerSkills::addCombo() { // Add combo orbs
	if (getActiveSkillLevel(1111002) > 0) {
		char advcombo = getSkillLevel(1120003);
		char maxcombo = (char) (advcombo > 0 ? Skills::skills[1120003][advcombo].x : Skills::skills[1111002][getSkillLevel(1111002)].x);
		if (this->combo == maxcombo)
			return;

		if (maxcombo > 5 && Randomizer::Instance()->randInt(99) < Skills::skills[1120003][advcombo].prop)
			this->combo += 2; // 4th job skill gives chance to add second orb
		else
			this->combo += 1;

		if (this->combo > maxcombo)
			this->combo = maxcombo;

		setCombo(combo, true);
	}
}

void PlayerSkills::save() {
	mysqlpp::Query query = Database::chardb.query();

	bool firstrun = true;
	for (hash_map<int, unsigned char>::iterator iter = playerskills.begin(); iter != playerskills.end(); iter++) {
		if (firstrun) {
			query << "REPLACE INTO skills VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		query << mysqlpp::quote << player->getId() << "," << mysqlpp::quote << iter->first << "," << mysqlpp::quote << iter->second << "," << mysqlpp::quote << getMaxSkillLevel(iter->first) << ")";
	}
	query.exec();
}
