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
#include "PlayerSkills.h"
#include "Database.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Randomizer.h"
#include "Skills.h"
#include "SkillsPacket.h"

bool PlayerSkills::addSkillLevel(int32_t skillid, uint8_t amount, bool sendpacket) {
	// Keep people from adding too much SP and prevent it from going negative
	uint8_t newlevel = ((playerskills.find(skillid) != playerskills.end()) ? playerskills[skillid].level : 0) + amount;
	if (newlevel > Skills::maxlevels[skillid]) {
		return false; // Let the caller handle this
	}

	playerskills[skillid].level = newlevel;
	if (sendpacket) {
		SkillsPacket::addSkill(player, skillid, playerskills[skillid]);
	}
	return true;
}

uint8_t PlayerSkills::getSkillLevel(int32_t skillid) {
	if (playerskills.find(skillid) != playerskills.end())
		return playerskills[skillid].level;
	return 0;
}

void PlayerSkills::setMaxSkillLevel(int32_t skillid, uint8_t maxlevel, bool sendpacket) { // Set max level for 4th job skills
	playerskills[skillid].maxlevel = maxlevel;

	if (sendpacket) {
		player->getSkills()->addSkillLevel(skillid, 0);
	}
}

uint8_t PlayerSkills::getMaxSkillLevel(int32_t skillid) {
	// Get max level for 4th job skills
	if (playerskills.find(skillid) != playerskills.end())
		return playerskills[skillid].maxlevel;
	return 0;
}

void PlayerSkills::load() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT skillid, points, maxlevel FROM skills WHERE charid = " << player->getId();
	mysqlpp::StoreQueryResult res = query.store();
	for (size_t i = 0; i < res.num_rows(); i++) {
		PlayerSkillInfo skill;
		skill.level = (uint8_t) res[i][1];
		skill.maxlevel = (uint8_t) res[i][2];
		playerskills[res[i][0]] = skill;
	}
}

void PlayerSkills::save() {
	mysqlpp::Query query = Database::getCharDB().query();

	bool firstrun = true;
	for (unordered_map<int32_t, PlayerSkillInfo>::iterator iter = playerskills.begin(); iter != playerskills.end(); iter++) {
		if (firstrun) {
			query << "REPLACE INTO skills VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		query << player->getId() << "," << iter->first << "," << (int16_t) iter->second.level << "," << (int16_t) iter->second.maxlevel << ")";
	}
	if (!firstrun)
		query.exec();
}

void PlayerSkills::connectData(PacketCreator &packet) {
	packet.add<int16_t>((int16_t) playerskills.size());
	for (unordered_map<int32_t, PlayerSkillInfo>::iterator iter = playerskills.begin(); iter != playerskills.end(); iter++) {
		packet.add<int32_t>(iter->first);
		packet.add<int32_t>(iter->second.level);
		if (FOURTHJOB_SKILL(iter->first))
			packet.add<int32_t>(iter->second.maxlevel); // Max Level for 4th job skills
	}
}
