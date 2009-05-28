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
#include "GameConstants.h"
#include "GameLogicUtilities.h"
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

bool PlayerSkills::hasElementalAmp() {
	bool has = false;
	switch (player->getJob()) {
		case Jobs::JobIds::FPMage:
		case Jobs::JobIds::FPArchMage:
			if (getSkillLevel(Jobs::FPMage::ElementAmplification) > 0)
				has = true;
			break;
		case Jobs::JobIds::ILMage:
		case Jobs::JobIds::ILArchMage:
			if (getSkillLevel(Jobs::ILMage::ElementAmplification) > 0)
				has = true;
			break;
	}
	return has;
}

int32_t PlayerSkills::getElementalAmp() {
	int32_t skillid = 0;
	switch (player->getJob()) {
		case Jobs::JobIds::FPMage:
		case Jobs::JobIds::FPArchMage:
			skillid = Jobs::FPMage::ElementAmplification;
			break;
		case Jobs::JobIds::ILMage:
		case Jobs::JobIds::ILArchMage:
			skillid = Jobs::ILMage::ElementAmplification;
			break;
	}
	return skillid;
}

bool PlayerSkills::hasEnergyCharge() {
	return (getSkillLevel(Jobs::Marauder::EnergyCharge) > 0);
}

int32_t PlayerSkills::getEnergyCharge() {
	int32_t skillid = Jobs::Marauder::EnergyCharge;
	return skillid;
}

int32_t PlayerSkills::getComboAttack() {
	int32_t skillid = Jobs::Crusader::ComboAttack;
	return skillid;
}

int32_t PlayerSkills::getAdvancedCombo() {
	int32_t skillid = Jobs::Hero::AdvancedComboAttack;
	return skillid;
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

	query << "SELECT * FROM cooldowns WHERE charid = " << player->getId();
	res = query.store();
	for (size_t i = 0; i < res.size(); i++) {
		int32_t skillid = res[i]["skillid"];
		int16_t timeleft = static_cast<int16_t>(res[i]["timeleft"]);
		Skills::startCooldown(player, skillid, timeleft, true);
		cooldowns[skillid] = timeleft;
	}
}

void PlayerSkills::save(bool savecooldowns) {
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

	if (savecooldowns) {
		query << "DELETE FROM cooldowns WHERE charid = " << player->getId();
		query.exec();
		if (cooldowns.size() > 0) {
			firstrun = true;
			for (unordered_map<int32_t, int16_t>::iterator iter = cooldowns.begin(); iter != cooldowns.end(); iter++) {
				if (firstrun) {
					query << "INSERT INTO cooldowns (charid, skillid, timeleft) VALUES (";
					firstrun = false;
				}
				else {
					query << ",(";
				}
				query << player->getId() << ","
						<< iter->first << ","
						<< Skills::getCooldownTimeLeft(player, iter->first) << ")";
			}
			query.exec();
		}
	}
}

void PlayerSkills::connectData(PacketCreator &packet) {
	// Skill levels
	packet.add<int16_t>((int16_t) playerskills.size());
	for (unordered_map<int32_t, PlayerSkillInfo>::iterator iter = playerskills.begin(); iter != playerskills.end(); iter++) {
		packet.add<int32_t>(iter->first);
		packet.add<int32_t>(iter->second.level);
		if (GameLogicUtilities::isFourthJobSkill(iter->first))
			packet.add<int32_t>(iter->second.maxlevel); // Max Level for 4th job skills
	}
	// Cooldowns
	packet.add<int16_t>((int16_t) cooldowns.size());
	for (unordered_map<int32_t, int16_t>::iterator iter = cooldowns.begin(); iter != cooldowns.end(); iter++) {
		packet.add<int32_t>(iter->first);
		packet.add<int16_t>(iter->second);
	}
}

void PlayerSkills::addCooldown(int32_t skillid, int16_t time) {
	cooldowns[skillid] = time;
}

void PlayerSkills::removeCooldown(int32_t skillid) {
	if (cooldowns.find(skillid) != cooldowns.end())
		cooldowns.erase(skillid);
}

void PlayerSkills::removeAllCooldowns() {
	unordered_map<int32_t, int16_t> dupe = cooldowns;
	for (unordered_map<int32_t, int16_t>::iterator iter = dupe.begin(); iter != dupe.end(); iter++) {
		if (iter->first != Jobs::Buccaneer::TimeLeap) {
			Skills::stopCooldown(player, iter->first);
		}
	}
}