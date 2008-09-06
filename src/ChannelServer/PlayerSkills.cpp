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

void PlayerSkills::save() {
	mysqlpp::Query query = Database::getCharDB().query();

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
