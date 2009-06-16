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
	switch (player->getStats()->getJob()) {
		case Jobs::JobIds::FPMage:
		case Jobs::JobIds::FPArchMage:
			has = (getSkillLevel(Jobs::FPMage::ElementAmplification) > 0);
			break;
		case Jobs::JobIds::ILMage:
		case Jobs::JobIds::ILArchMage:
			has = (getSkillLevel(Jobs::ILMage::ElementAmplification) > 0);
			break;
	}
	return has;
}

int32_t PlayerSkills::getElementalAmp() {
	int32_t skillid = 0;
	switch (player->getStats()->getJob()) {
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

bool PlayerSkills::hasAchilles() {
	bool has = false;
	switch (player->getStats()->getJob()) {
		case Jobs::JobIds::Hero:
			has = (getSkillLevel(Jobs::Hero::Achilles) > 0);
			break;
		case Jobs::JobIds::Paladin:
			has = (getSkillLevel(Jobs::Paladin::Achilles) > 0);
			break;
		case Jobs::JobIds::DarkKnight:
			has = (getSkillLevel(Jobs::DarkKnight::Achilles) > 0);
			break;
	}
	return has;
}

int32_t PlayerSkills::getAchilles() {
	int32_t skillid = 0;
	switch (player->getStats()->getJob()) {
		case Jobs::JobIds::Hero:
			skillid = Jobs::Hero::Achilles;
			break;
		case Jobs::JobIds::Paladin:
			skillid = Jobs::Paladin::Achilles;
			break;
		case Jobs::JobIds::DarkKnight:
			skillid = Jobs::DarkKnight::Achilles;
			break;
	}
	return skillid;
}

bool PlayerSkills::hasEnergyCharge() {
	bool has = false;
	switch (player->getStats()->getJob()) {
		case Jobs::JobIds::Marauder:
		case Jobs::JobIds::Buccaneer:
			has = (getSkillLevel(Jobs::Marauder::EnergyCharge) > 0);
			break;
	}
	return has;
}

int32_t PlayerSkills::getEnergyCharge() {
	int32_t skillid = 0;
	switch (player->getStats()->getJob()) {
		case Jobs::JobIds::Marauder:
		case Jobs::JobIds::Buccaneer:
			skillid = Jobs::Marauder::EnergyCharge;
			break;
	}
	return skillid;
}

int32_t PlayerSkills::getComboAttack() {
	int32_t skillid = 0;
	switch (player->getStats()->getJob()) {
		case Jobs::JobIds::Crusader:
		case Jobs::JobIds::Hero:
			skillid = Jobs::Crusader::ComboAttack;
			break;
	}
	return skillid;
}

int32_t PlayerSkills::getAdvancedCombo() {
	int32_t skillid = 0;
	switch (player->getStats()->getJob()) {
		case Jobs::JobIds::Hero:
			skillid = Jobs::Hero::AdvancedComboAttack;
			break;
	}
	return skillid;
}

int32_t PlayerSkills::getAlchemist() {
	int32_t skillid = 0;
	switch (player->getStats()->getJob()) {
		case Jobs::JobIds::Hermit:
		case Jobs::JobIds::NightLord:
			skillid = Jobs::Hermit::Alchemist;
			break;
	}
	return skillid;
}

bool PlayerSkills::hasHpIncrease() {
	bool has = false;
	switch (GameLogicUtilities::getJobTrack(player->getStats()->getJob())) {
		case Jobs::JobTracks::Warrior:
			has = (getSkillLevel(Jobs::Swordsman::ImprovedMaxHpIncrease) > 0);
			break;
		case Jobs::JobTracks::Pirate:
			if ((player->getStats()->getJob() / 10) == (Jobs::JobIds::Infighter / 10))
				has = (getSkillLevel(Jobs::Infighter::ImproveMaxHp) > 0);
			break;
	}
	return has;
}

int32_t PlayerSkills::getHpIncrease() {
	int32_t skillid = 0;
	switch (GameLogicUtilities::getJobTrack(player->getStats()->getJob())) {
		case Jobs::JobTracks::Warrior:
			skillid = Jobs::Swordsman::ImprovedMaxHpIncrease;
			break;
		case Jobs::JobTracks::Pirate:
			if ((player->getStats()->getJob() / 10) == (Jobs::JobIds::Infighter / 10))
				skillid = Jobs::Infighter::ImproveMaxHp;
			break;
	}
	return skillid;
}

bool PlayerSkills::hasMpIncrease() {
	bool has = false;
	switch (GameLogicUtilities::getJobTrack(player->getStats()->getJob())) {
		case Jobs::JobTracks::Magician:
			has = (getSkillLevel(Jobs::Magician::ImprovedMaxMpIncrease) > 0);
			break;
	}
	return has;
}

int32_t PlayerSkills::getMpIncrease() {
	int32_t skillid = 0;
	switch (GameLogicUtilities::getJobTrack(player->getStats()->getJob())) {
		case Jobs::JobTracks::Magician:
			skillid = Jobs::Magician::ImprovedMaxMpIncrease;
			break;
	}
	return skillid;
}

int32_t PlayerSkills::getMastery() {
	int32_t masteryid = 0;
	switch (GameLogicUtilities::getItemType(player->getInventory()->getEquippedId(EquipSlots::Weapon))) {
		case Weapon1hSword:
		case Weapon2hSword:
			switch (player->getStats()->getJob()) {
				case Jobs::JobIds::Fighter:
				case Jobs::JobIds::Crusader:
				case Jobs::JobIds::Hero:
					masteryid = Jobs::Fighter::SwordMastery;
					break;
				case Jobs::JobIds::Page:
				case Jobs::JobIds::WhiteKnight:
				case Jobs::JobIds::Paladin:
					masteryid = Jobs::Page::SwordMastery;
					break;
			}
			break;
		case Weapon1hAxe:
		case Weapon2hAxe:
			masteryid = Jobs::Fighter::AxeMastery;
			break;
		case Weapon1hMace:
		case Weapon2hMace:
			masteryid = Jobs::Page::BwMastery;
			break;
		case WeaponSpear:
			masteryid = Jobs::Spearman::SpearMastery;
			break;
		case WeaponPolearm:
			masteryid = Jobs::Spearman::PolearmMastery;
			break;
		case WeaponDagger:
			masteryid = Jobs::Bandit::DaggerMastery;
			break;
		case WeaponKnuckle:
			masteryid = Jobs::Infighter::KnucklerMastery;
			break;
		case WeaponBow:
			masteryid = Jobs::Hunter::BowMastery;
			break;
		case WeaponCrossbow:
			masteryid = Jobs::Crossbowman::CrossbowMastery;
			break;
		case WeaponClaw:
			masteryid = Jobs::Assassin::ClawMastery;
			break;
		case WeaponGun:
			masteryid = Jobs::Gunslinger::GunMastery;
			break;
	}
	return masteryid;
}

int32_t PlayerSkills::getMpEater() {
	int32_t skillid = 0;
	switch (player->getStats()->getJob()) {
		case Jobs::JobIds::FPWizard:
		case Jobs::JobIds::FPMage:
		case Jobs::JobIds::FPArchMage:
			skillid = Jobs::FPWizard::MpEater;
			break;
		case Jobs::JobIds::ILWizard:
		case Jobs::JobIds::ILMage:
		case Jobs::JobIds::ILArchMage:
			skillid = Jobs::ILWizard::MpEater;
			break;
		case Jobs::JobIds::Cleric:
		case Jobs::JobIds::Priest:
		case Jobs::JobIds::Bishop:
			skillid = Jobs::Cleric::MpEater;
			break;
	}
	return skillid;
}

int16_t PlayerSkills::getRechargeableBonus() {
	int16_t bonus = 0;
	switch (player->getStats()->getJob()) {
		case Jobs::JobIds::Assassin:
		case Jobs::JobIds::Hermit:
		case Jobs::JobIds::NightLord:
			bonus = getSkillLevel(Jobs::Assassin::ClawMastery) * 10;
			break;
		case Jobs::JobIds::Gunslinger:
		case Jobs::JobIds::Outlaw:
		case Jobs::JobIds::Corsair:
			bonus = getSkillLevel(Jobs::Gunslinger::GunMastery) * 10;
			break;
	}
	return bonus;
}

bool PlayerSkills::hasVenomousWeapon() {
	bool has = false;
	switch (player->getStats()->getJob()) {
		case Jobs::JobIds::NightLord:
			has = (getSkillLevel(Jobs::NightLord::VenomousStar) > 0);
			break;
		case Jobs::JobIds::Shadower:
			has = (getSkillLevel(Jobs::Shadower::VenomousStab) > 0);
			break;
	}
	return has;
}

int32_t PlayerSkills::getVenomousWeapon() {
	int32_t skill = 0;
	switch (player->getStats()->getJob()) {
		case Jobs::JobIds::NightLord:
			skill = Jobs::NightLord::VenomousStar;
			break;
		case Jobs::JobIds::Shadower:
			skill = Jobs::Shadower::VenomousStab;
			break;
	}
	return skill;
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