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
#include "Skills.h"
#include "SkillsPacket.h"
#include "MySQLM.h"

bool PlayerSkills::addSkillLevel(int skillid, unsigned char amount, bool sendpacket) {
	// Keep people from adding too much SP and prevent it from going negative
	int newlevel = ((playerskills.find(skillid) != playerskills.end()) ? playerskills[skillid] : 0) + amount;
	if (newlevel > Skills::maxlevels[skillid] || newlevel < 0) {
		return false; // Let the caller handle this
	}

	playerskills[skillid] = newlevel;

	if (sendpacket) {
		char maxlevel = 0;
		if (FORTHJOB_SKILL(skillid)) {
			maxlevel = getMaxSkillLevel(skillid);
		}
		SkillsPacket::addSkill(player, skillid, getSkillLevel(skillid), maxlevel);
	}
	
	return true;
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

void PlayerSkills::setSkillMapEnterInfo(int skillid, vector<SkillMapActiveInfo> skill) {
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

void PlayerSkills::save() {
	mysqlpp::Query query = Database::chardb.query();

	bool firstrun = true;
	for (unordered_map<int, unsigned char>::iterator iter = playerskills.begin(); iter != playerskills.end(); iter++) {
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
