/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "SkillDataProvider.h"
#include "Skills.h"
#include "SkillsPacket.h"

void PlayerSkills::load() {
	soci::session &sql = Database::getCharDb();
	PlayerSkillInfo skill;
	int32_t playerId = m_player->getId();

	soci::rowset<> rs = (Database::getCharDb().prepare << "SELECT s.skill_id, s.points, s.max_level FROM skills s WHERE s.character_id = :char",
															soci::use(playerId, "char"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		skill = PlayerSkillInfo();
		skill.level = row.get<uint8_t>("points");
		skill.maxLevel = row.get<int8_t>("max_level");
		m_skills[row.get<int32_t>("skill_id")] = skill;
	}

	rs = (Database::getCharDb().prepare << "SELECT c.* FROM cooldowns c WHERE c.character_id = :char",
											soci::use(playerId, "char"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		int32_t skillId = row.get<int32_t>("skill_id");
		int16_t timeLeft = row.get<int16_t>("remaining_time");
		Skills::startCooldown(m_player, skillId, timeLeft, true);
		m_cooldowns[skillId] = timeLeft;
	}
}

void PlayerSkills::save(bool saveCooldowns) {
	using namespace soci;
	int32_t playerId = m_player->getId();
	session &sql = Database::getCharDb();

	int32_t skillId = 0;
	uint8_t level = 0;
	uint8_t maxLevel = 0;
	statement st = (sql.prepare << "REPLACE INTO skills VALUES (:player, :skill, :level, :maxLevel)",
									use(playerId, "player"),
									use(skillId, "skill"),
									use(level, "level"),
									use(maxLevel, "maxLevel"));

	for (unordered_map<int32_t, PlayerSkillInfo>::iterator iter = m_skills.begin(); iter != m_skills.end(); ++iter) {
		skillId = iter->first;
		level = iter->second.level;
		maxLevel = iter->second.maxLevel;
		st.execute(true);
	}

	if (saveCooldowns) {
		sql.once << "DELETE FROM cooldowns WHERE character_id = :char",
					soci::use(playerId, "char");

		if (m_cooldowns.size() > 0) {
			int16_t remainingTime = 0;
			st = (sql.prepare << "INSERT INTO cooldowns (character_id, skill_id, remaining_time) VALUES (:char, :skill, :time)",
									use(playerId, "char"),
									use(skillId, "skill"),
									use(remainingTime, "time"));

			for (unordered_map<int32_t, int16_t>::iterator iter = m_cooldowns.begin(); iter != m_cooldowns.end(); ++iter) {
				skillId = iter->first;
				remainingTime = Skills::getCooldownTimeLeft(m_player, iter->first);
				st.execute(true);
			}
		}
	}
}

bool PlayerSkills::addSkillLevel(int32_t skillId, uint8_t amount, bool sendPacket) {
	// Keep people from adding too much SP and prevent it from going negative
	uint8_t newLevel = ((m_skills.find(skillId) != m_skills.end()) ? m_skills[skillId].level : 0) + amount;
	if (newLevel > SkillDataProvider::Instance()->getMaxLevel(skillId) || (GameLogicUtilities::isFourthJobSkill(skillId) && newLevel > getMaxSkillLevel(skillId))) {
		return false; // Let the caller handle this
	}

	m_skills[skillId].level = newLevel;
	if (sendPacket) {
		SkillsPacket::addSkill(m_player, skillId, m_skills[skillId]);
	}
	return true;
}

uint8_t PlayerSkills::getSkillLevel(int32_t skillId) {
	if (m_skills.find(skillId) != m_skills.end()) {
		return m_skills[skillId].level;
	}
	return 0;
}

void PlayerSkills::setMaxSkillLevel(int32_t skillId, uint8_t maxLevel, bool sendPacket) {
	// Set max level for 4th job skills
	m_skills[skillId].maxLevel = maxLevel;

	if (sendPacket) {
		m_player->getSkills()->addSkillLevel(skillId, 0);
	}
}

uint8_t PlayerSkills::getMaxSkillLevel(int32_t skillId) {
	// Get max level for 4th job skills
	if (m_skills.find(skillId) != m_skills.end()) {
		return m_skills[skillId].maxLevel;
	}
	return 0;
}

SkillLevelInfo * PlayerSkills::getSkillInfo(int32_t skillId) {
	if (m_skills.find(skillId) == m_skills.end()) {
		return nullptr;
	}
	return SkillDataProvider::Instance()->getSkill(skillId, m_skills[skillId].level);
}

bool PlayerSkills::hasElementalAmp() {
	int32_t skillId = getElementalAmp();
	return (skillId != 0 && getSkillLevel(skillId) > 0);
}

bool PlayerSkills::hasAchilles() {
	int32_t skillId = getAchilles();
	return (skillId != 0 && getSkillLevel(skillId) > 0);
}

bool PlayerSkills::hasEnergyCharge() {
	int32_t skillId = getEnergyCharge();
	return (skillId != 0 && getSkillLevel(skillId) > 0);
}

bool PlayerSkills::hasHpIncrease() {
	int32_t skillId = getHpIncrease();
	return (skillId != 0 && getSkillLevel(skillId) > 0);
}

bool PlayerSkills::hasMpIncrease() {
	int32_t skillId = getMpIncrease();
	return (skillId != 0 && getSkillLevel(skillId) > 0);
}

bool PlayerSkills::hasVenomousWeapon() {
	int32_t skillId = getVenomousWeapon();
	return (skillId != 0 && getSkillLevel(skillId) > 0);
}

bool PlayerSkills::hasNoDamageSkill() {
	int32_t skillId = getNoDamageSkill();
	return (skillId != 0 && getSkillLevel(skillId) > 0);
}

int32_t PlayerSkills::getElementalAmp() {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::FpMage:
		case Jobs::JobIds::FpArchMage: skillId = Jobs::FpMage::ElementAmplification; break;
		case Jobs::JobIds::IlMage:
		case Jobs::JobIds::IlArchMage: skillId = Jobs::IlMage::ElementAmplification; break;
		case Jobs::JobIds::BlazeWizard3: skillId = Jobs::BlazeWizard::ElementAmplification; break;
	}
	return skillId;
}

int32_t PlayerSkills::getAchilles() {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Hero: skillId = Jobs::Hero::Achilles; break;
		case Jobs::JobIds::Paladin: skillId = Jobs::Paladin::Achilles; break;
		case Jobs::JobIds::DarkKnight: skillId = Jobs::DarkKnight::Achilles; break;
	}
	return skillId;
}

int32_t PlayerSkills::getEnergyCharge() {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Marauder:
		case Jobs::JobIds::Buccaneer: skillId = Jobs::Marauder::EnergyCharge; break;
		case Jobs::JobIds::ThunderBreaker2:
		case Jobs::JobIds::ThunderBreaker3: skillId = Jobs::ThunderBreaker::EnergyCharge; break;
	}
	return skillId;
}

int32_t PlayerSkills::getComboAttack() {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Crusader:
		case Jobs::JobIds::Hero: skillId = Jobs::Crusader::ComboAttack; break;
		case Jobs::JobIds::DawnWarrior3:
		case Jobs::JobIds::DawnWarrior4: skillId = Jobs::DawnWarrior::ComboAttack; break;
	}
	return skillId;
}

int32_t PlayerSkills::getAdvancedCombo() {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Hero: skillId = Jobs::Hero::AdvancedComboAttack; break;
		case Jobs::JobIds::DawnWarrior3:
		case Jobs::JobIds::DawnWarrior4: skillId = Jobs::DawnWarrior::AdvancedCombo; break;
	}
	return skillId;
}

int32_t PlayerSkills::getAlchemist() {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Hermit:
		case Jobs::JobIds::NightLord: skillId = Jobs::Hermit::Alchemist; break;
		case Jobs::JobIds::NightWalker3: skillId = Jobs::NightWalker::Alchemist; break;
	}
	return skillId;
}

int32_t PlayerSkills::getHpIncrease() {
	int32_t skillId = 0;
	switch (GameLogicUtilities::getJobTrack(m_player->getStats()->getJob())) {
		case Jobs::JobTracks::Warrior: skillId = Jobs::Swordsman::ImprovedMaxHpIncrease; break;
		case Jobs::JobTracks::DawnWarrior: skillId = Jobs::DawnWarrior::MaxHpEnhancement; break;
		case Jobs::JobTracks::ThunderBreaker: skillId = Jobs::ThunderBreaker::ImproveMaxHp; break;
		case Jobs::JobTracks::Pirate:
			if ((m_player->getStats()->getJob() / 10) == (Jobs::JobIds::Brawler / 10)) {
				skillId = Jobs::Brawler::ImproveMaxHp;
			}
			break;
	}
	return skillId;
}

int32_t PlayerSkills::getMpIncrease() {
	int32_t skillId = 0;
	switch (GameLogicUtilities::getJobTrack(m_player->getStats()->getJob())) {
		case Jobs::JobTracks::Magician: skillId = Jobs::Magician::ImprovedMaxMpIncrease; break;
		case Jobs::JobTracks::BlazeWizard: skillId = Jobs::BlazeWizard::IncreasingMaxMp; break;
	}
	return skillId;
}

int32_t PlayerSkills::getMastery() {
	int32_t masteryId = 0;
	switch (GameLogicUtilities::getItemType(m_player->getInventory()->getEquippedId(EquipSlots::Weapon))) {
		case Items::Types::Weapon1hSword:
		case Items::Types::Weapon2hSword:
			switch (m_player->getStats()->getJob()) {
				case Jobs::JobIds::Fighter:
				case Jobs::JobIds::Crusader:
				case Jobs::JobIds::Hero: masteryId = Jobs::Fighter::SwordMastery; break;
				case Jobs::JobIds::Page:
				case Jobs::JobIds::WhiteKnight:
				case Jobs::JobIds::Paladin: masteryId = Jobs::Page::SwordMastery; break;
			}
			break;
		case Items::Types::Weapon1hAxe:
		case Items::Types::Weapon2hAxe: masteryId = Jobs::Fighter::AxeMastery; break;
		case Items::Types::Weapon1hMace:
		case Items::Types::Weapon2hMace: masteryId = Jobs::Page::BwMastery; break;
		case Items::Types::WeaponSpear: masteryId = Jobs::Spearman::SpearMastery; break;
		case Items::Types::WeaponPolearm: masteryId = Jobs::Spearman::PolearmMastery; break;
		case Items::Types::WeaponDagger: masteryId = Jobs::Bandit::DaggerMastery; break;
		case Items::Types::WeaponKnuckle: masteryId = Jobs::Brawler::KnucklerMastery; break;
		case Items::Types::WeaponBow: masteryId = Jobs::Hunter::BowMastery; break;
		case Items::Types::WeaponCrossbow: masteryId = Jobs::Crossbowman::CrossbowMastery; break;
		case Items::Types::WeaponClaw: masteryId = Jobs::Assassin::ClawMastery; break;
		case Items::Types::WeaponGun: masteryId = Jobs::Gunslinger::GunMastery; break;
	}
	return masteryId;
}

int32_t PlayerSkills::getMpEater() {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::FpWizard:
		case Jobs::JobIds::FpMage:
		case Jobs::JobIds::FpArchMage: skillId = Jobs::FpWizard::MpEater; break;
		case Jobs::JobIds::IlWizard:
		case Jobs::JobIds::IlMage:
		case Jobs::JobIds::IlArchMage: skillId = Jobs::IlWizard::MpEater; break;
		case Jobs::JobIds::Cleric:
		case Jobs::JobIds::Priest:
		case Jobs::JobIds::Bishop: skillId = Jobs::Cleric::MpEater; break;
	}
	return skillId;
}

int32_t PlayerSkills::getVenomousWeapon() {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::NightLord: skillId = Jobs::NightLord::VenomousStar; break;
		case Jobs::JobIds::Shadower: skillId = Jobs::Shadower::VenomousStab; break;
	}
	return skillId;
}

int32_t PlayerSkills::getNoDamageSkill() {
	int32_t noDamageId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::NightLord: noDamageId = Jobs::NightLord::ShadowShifter; break;
		case Jobs::JobIds::Shadower: noDamageId = Jobs::Shadower::ShadowShifter; break;
		case Jobs::JobIds::Hero: noDamageId = Jobs::Hero::Guardian; break;
		case Jobs::JobIds::Paladin: noDamageId = Jobs::Paladin::Guardian; break;
	}
	return noDamageId;
}

int16_t PlayerSkills::getRechargeableBonus() {
	int16_t bonus = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Assassin:
		case Jobs::JobIds::Hermit:
		case Jobs::JobIds::NightLord: bonus = getSkillLevel(Jobs::Assassin::ClawMastery) * 10; break;
		case Jobs::JobIds::Gunslinger:
		case Jobs::JobIds::Outlaw:
		case Jobs::JobIds::Corsair: bonus = getSkillLevel(Jobs::Gunslinger::GunMastery) * 10; break;
		case Jobs::JobIds::NightWalker2:
		case Jobs::JobIds::NightWalker3: bonus = getSkillLevel(Jobs::NightWalker::ClawMastery) * 10; break;
	}
	return bonus;
}

void PlayerSkills::addCooldown(int32_t skillId, int16_t time) {
	m_cooldowns[skillId] = time;
}

void PlayerSkills::removeCooldown(int32_t skillId) {
	if (m_cooldowns.find(skillId) != m_cooldowns.end()) {
		m_cooldowns.erase(skillId);
	}
}

void PlayerSkills::removeAllCooldowns() {
	unordered_map<int32_t, int16_t> dupe = m_cooldowns;
	for (unordered_map<int32_t, int16_t>::iterator iter = dupe.begin(); iter != dupe.end(); ++iter) {
		if (iter->first != Jobs::Buccaneer::TimeLeap) {
			Skills::stopCooldown(m_player, iter->first);
		}
	}
}

void PlayerSkills::connectData(PacketCreator &packet) {
	// Skill levels
	packet.add<uint16_t>(m_skills.size());
	for (unordered_map<int32_t, PlayerSkillInfo>::iterator iter = m_skills.begin(); iter != m_skills.end(); ++iter) {
		packet.add<int32_t>(iter->first);
		packet.add<int32_t>(iter->second.level);
		if (GameLogicUtilities::isFourthJobSkill(iter->first)) {
			packet.add<int32_t>(iter->second.maxLevel); // Max Level for 4th job skills
		}
	}
	// Cooldowns
	packet.add<uint16_t>(m_cooldowns.size());
	for (unordered_map<int32_t, int16_t>::iterator iter = m_cooldowns.begin(); iter != m_cooldowns.end(); ++iter) {
		packet.add<int32_t>(iter->first);
		packet.add<int16_t>(iter->second);
	}
}