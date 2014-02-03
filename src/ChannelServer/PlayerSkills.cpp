/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "PlayerSkills.hpp"
#include "Algorithm.hpp"
#include "Database.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "Player.hpp"
#include "Randomizer.hpp"
#include "SkillDataProvider.hpp"
#include "Skills.hpp"
#include "SkillsPacket.hpp"

auto PlayerSkills::load() -> void {
	soci::session &sql = Database::getCharDb();
	PlayerSkillInfo skill;
	int32_t playerId = m_player->getId();
	int32_t skillId = 0;

	soci::rowset<> rs = (Database::getCharDb().prepare
		<< "SELECT s.skill_id, s.points, s.max_level "
		<< "FROM " << Database::makeCharTable("skills") << " s "
		<< "WHERE s.character_id = :char",
		soci::use(playerId, "char"));

	for (const auto &row : rs) {
		skill = PlayerSkillInfo();
		skillId = row.get<int32_t>("skill_id");
		skill.level = row.get<uint8_t>("points");
		skill.maxSkillLevel = SkillDataProvider::getInstance().getMaxLevel(skillId);
		skill.playerMaxSkillLevel = row.get<uint8_t>("max_level");
		m_skills[skillId] = skill;
	}

	rs = (Database::getCharDb().prepare
		<< "SELECT c.* "
		<< "FROM " << Database::makeCharTable("cooldowns") << " c "
		<< "WHERE c.character_id = :char",
		soci::use(playerId, "char"));

	for (const auto &row : rs) {
		int32_t skillId = row.get<int32_t>("skill_id");
		int16_t timeLeft = row.get<int16_t>("remaining_time");
		Skills::startCooldown(m_player, skillId, timeLeft, true);
		m_cooldowns[skillId] = timeLeft;
	}
}

auto PlayerSkills::save(bool saveCooldowns) -> void {
	using namespace soci;
	int32_t playerId = m_player->getId();
	session &sql = Database::getCharDb();

	int32_t skillId = 0;
	uint8_t level = 0;
	uint8_t maxLevel = 0;
	statement st = (sql.prepare
		<< "REPLACE INTO " << Database::makeCharTable("skills") << " VALUES (:player, :skill, :level, :maxLevel)",
		use(playerId, "player"),
		use(skillId, "skill"),
		use(level, "level"),
		use(maxLevel, "maxLevel"));

	for (const auto &kvp : m_skills) {
		skillId = kvp.first;
		level = kvp.second.level;
		maxLevel = kvp.second.playerMaxSkillLevel;
		st.execute(true);
	}

	if (saveCooldowns) {
		sql.once << "DELETE FROM " << Database::makeCharTable("cooldowns") << " WHERE character_id = :char", soci::use(playerId, "char");

		if (m_cooldowns.size() > 0) {
			int16_t remainingTime = 0;
			st = (sql.prepare
				<< "INSERT INTO " << Database::makeCharTable("cooldowns") << " (character_id, skill_id, remaining_time) "
				<< "VALUES (:char, :skill, :time)",
				use(playerId, "char"),
				use(skillId, "skill"),
				use(remainingTime, "time"));

			for (const auto &kvp : m_cooldowns) {
				skillId = kvp.first;
				remainingTime = Skills::getCooldownTimeLeft(m_player, kvp.first);
				st.execute(true);
			}
		}
	}
}

auto PlayerSkills::addSkillLevel(int32_t skillId, uint8_t amount, bool sendPacket) -> bool {
	if (!SkillDataProvider::getInstance().isValidSkill(skillId)) {
		return false;
	}

	// Keep people from adding too much SP and prevent it from going negative

	auto kvp = m_skills.find(skillId);
	uint8_t newLevel = (kvp != std::end(m_skills) ? kvp->second.level : 0) + amount;
	uint8_t maxSkillLevel = SkillDataProvider::getInstance().getMaxLevel(skillId);
	if (newLevel > maxSkillLevel || (GameLogicUtilities::isFourthJobSkill(skillId) && newLevel > getMaxSkillLevel(skillId))) {
		return false;
	}

	m_skills[skillId].level = newLevel;
	m_skills[skillId].maxSkillLevel = maxSkillLevel;
	if (sendPacket) {
		m_player->send(SkillsPacket::addSkill(skillId, m_skills[skillId]));
	}
	return true;
}

auto PlayerSkills::getSkillLevel(int32_t skillId) const -> uint8_t {
	auto skill = ext::find_value_ptr(m_skills, skillId);
	return skill == nullptr ? 0 : skill->level;
}

auto PlayerSkills::setMaxSkillLevel(int32_t skillId, uint8_t maxLevel, bool sendPacket) -> void {
	// Set max level for 4th job skills
	m_skills[skillId].playerMaxSkillLevel = maxLevel;

	if (sendPacket) {
		m_player->getSkills()->addSkillLevel(skillId, 0);
	}
}

auto PlayerSkills::getMaxSkillLevel(int32_t skillId) const -> uint8_t {
	// Get max level for 4th job skills
	if (m_skills.find(skillId) != std::end(m_skills)) {
		const PlayerSkillInfo &info = m_skills.find(skillId)->second;
		if (GameLogicUtilities::isFourthJobSkill(skillId)) {
			return info.playerMaxSkillLevel;
		}
		return info.maxSkillLevel;
	}
	return 0;
}

auto PlayerSkills::getSkillInfo(int32_t skillId) const -> const SkillLevelInfo * const {
	auto skill = ext::find_value_ptr(m_skills, skillId);
	return skill == nullptr ? nullptr : SkillDataProvider::getInstance().getSkill(skillId, skill->level);
}

auto PlayerSkills::hasSkill(int32_t skillId) const -> bool {
	return skillId != 0 && getSkillLevel(skillId) > 0;
}

auto PlayerSkills::hasElementalAmp() const -> bool {
	return hasSkill(getElementalAmp());
}

auto PlayerSkills::hasAchilles() const -> bool {
	return hasSkill(getAchilles());
}

auto PlayerSkills::hasEnergyCharge() const -> bool {
	return hasSkill(getEnergyCharge());
}

auto PlayerSkills::hasHpIncrease() const -> bool {
	return hasSkill(getHpIncrease());
}

auto PlayerSkills::hasMpIncrease() const -> bool {
	return hasSkill(getMpIncrease());
}

auto PlayerSkills::hasVenomousWeapon() const -> bool {
	return hasSkill(getVenomousWeapon());
}

auto PlayerSkills::hasNoDamageSkill() const -> bool {
	return hasSkill(getNoDamageSkill());
}

auto PlayerSkills::getElementalAmp() const -> int32_t {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::FpMage:
		case Jobs::JobIds::FpArchMage: skillId = Skills::FpMage::ElementAmplification; break;
		case Jobs::JobIds::IlMage:
		case Jobs::JobIds::IlArchMage: skillId = Skills::IlMage::ElementAmplification; break;
		case Jobs::JobIds::BlazeWizard3: skillId = Skills::BlazeWizard::ElementAmplification; break;
	}
	return skillId;
}

auto PlayerSkills::getAchilles() const -> int32_t {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Hero: skillId = Skills::Hero::Achilles; break;
		case Jobs::JobIds::Paladin: skillId = Skills::Paladin::Achilles; break;
		case Jobs::JobIds::DarkKnight: skillId = Skills::DarkKnight::Achilles; break;
	}
	return skillId;
}

auto PlayerSkills::getEnergyCharge() const -> int32_t {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Marauder:
		case Jobs::JobIds::Buccaneer: skillId = Skills::Marauder::EnergyCharge; break;
		case Jobs::JobIds::ThunderBreaker2:
		case Jobs::JobIds::ThunderBreaker3: skillId = Skills::ThunderBreaker::EnergyCharge; break;
	}
	return skillId;
}

auto PlayerSkills::getComboAttack() const -> int32_t {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Crusader:
		case Jobs::JobIds::Hero: skillId = Skills::Crusader::ComboAttack; break;
		case Jobs::JobIds::DawnWarrior3:
		case Jobs::JobIds::DawnWarrior4: skillId = Skills::DawnWarrior::ComboAttack; break;
	}
	return skillId;
}

auto PlayerSkills::getAdvancedCombo() const -> int32_t {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Hero: skillId = Skills::Hero::AdvancedComboAttack; break;
		case Jobs::JobIds::DawnWarrior3:
		case Jobs::JobIds::DawnWarrior4: skillId = Skills::DawnWarrior::AdvancedCombo; break;
	}
	return skillId;
}

auto PlayerSkills::getAlchemist() const -> int32_t {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Hermit:
		case Jobs::JobIds::NightLord: skillId = Skills::Hermit::Alchemist; break;
		case Jobs::JobIds::NightWalker3: skillId = Skills::NightWalker::Alchemist; break;
	}
	return skillId;
}

auto PlayerSkills::getHpIncrease() const -> int32_t {
	int32_t skillId = 0;
	switch (GameLogicUtilities::getJobTrack(m_player->getStats()->getJob())) {
		case Jobs::JobTracks::Warrior: skillId = Skills::Swordsman::ImprovedMaxHpIncrease; break;
		case Jobs::JobTracks::DawnWarrior: skillId = Skills::DawnWarrior::MaxHpEnhancement; break;
		case Jobs::JobTracks::ThunderBreaker: skillId = Skills::ThunderBreaker::ImproveMaxHp; break;
		case Jobs::JobTracks::Pirate:
			if ((m_player->getStats()->getJob() / 10) == (Jobs::JobIds::Brawler / 10)) {
				skillId = Skills::Brawler::ImproveMaxHp;
			}
			break;
	}
	return skillId;
}

auto PlayerSkills::getMpIncrease() const -> int32_t {
	int32_t skillId = 0;
	switch (GameLogicUtilities::getJobTrack(m_player->getStats()->getJob())) {
		case Jobs::JobTracks::Magician: skillId = Skills::Magician::ImprovedMaxMpIncrease; break;
		case Jobs::JobTracks::BlazeWizard: skillId = Skills::BlazeWizard::IncreasingMaxMp; break;
	}
	return skillId;
}

auto PlayerSkills::getMastery() const -> int32_t {
	int32_t masteryId = 0;
	switch (GameLogicUtilities::getItemType(m_player->getInventory()->getEquippedId(EquipSlots::Weapon))) {
		case Items::Types::Weapon1hSword:
		case Items::Types::Weapon2hSword:
			switch (m_player->getStats()->getJob()) {
				case Jobs::JobIds::Fighter:
				case Jobs::JobIds::Crusader:
				case Jobs::JobIds::Hero: masteryId = Skills::Fighter::SwordMastery; break;
				case Jobs::JobIds::Page:
				case Jobs::JobIds::WhiteKnight:
				case Jobs::JobIds::Paladin: masteryId = Skills::Page::SwordMastery; break;
			}
			break;
		case Items::Types::Weapon1hAxe:
		case Items::Types::Weapon2hAxe: masteryId = Skills::Fighter::AxeMastery; break;
		case Items::Types::Weapon1hMace:
		case Items::Types::Weapon2hMace: masteryId = Skills::Page::BwMastery; break;
		case Items::Types::WeaponSpear: masteryId = Skills::Spearman::SpearMastery; break;
		case Items::Types::WeaponPolearm: masteryId = Skills::Spearman::PolearmMastery; break;
		case Items::Types::WeaponDagger: masteryId = Skills::Bandit::DaggerMastery; break;
		case Items::Types::WeaponKnuckle: masteryId = Skills::Brawler::KnucklerMastery; break;
		case Items::Types::WeaponBow: masteryId = Skills::Hunter::BowMastery; break;
		case Items::Types::WeaponCrossbow: masteryId = Skills::Crossbowman::CrossbowMastery; break;
		case Items::Types::WeaponClaw: masteryId = Skills::Assassin::ClawMastery; break;
		case Items::Types::WeaponGun: masteryId = Skills::Gunslinger::GunMastery; break;
	}
	return masteryId;
}

auto PlayerSkills::getMpEater() const -> int32_t {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::FpWizard:
		case Jobs::JobIds::FpMage:
		case Jobs::JobIds::FpArchMage: skillId = Skills::FpWizard::MpEater; break;
		case Jobs::JobIds::IlWizard:
		case Jobs::JobIds::IlMage:
		case Jobs::JobIds::IlArchMage: skillId = Skills::IlWizard::MpEater; break;
		case Jobs::JobIds::Cleric:
		case Jobs::JobIds::Priest:
		case Jobs::JobIds::Bishop: skillId = Skills::Cleric::MpEater; break;
	}
	return skillId;
}

auto PlayerSkills::getVenomousWeapon() const -> int32_t {
	int32_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::NightLord: skillId = Skills::NightLord::VenomousStar; break;
		case Jobs::JobIds::Shadower: skillId = Skills::Shadower::VenomousStab; break;
	}
	return skillId;
}

auto PlayerSkills::getNoDamageSkill() const -> int32_t {
	int32_t noDamageId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::NightLord: noDamageId = Skills::NightLord::ShadowShifter; break;
		case Jobs::JobIds::Shadower: noDamageId = Skills::Shadower::ShadowShifter; break;
		case Jobs::JobIds::Hero: noDamageId = Skills::Hero::Guardian; break;
		case Jobs::JobIds::Paladin: noDamageId = Skills::Paladin::Guardian; break;
	}
	return noDamageId;
}

auto PlayerSkills::getRechargeableBonus() const -> int16_t {
	int16_t bonus = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Assassin:
		case Jobs::JobIds::Hermit:
		case Jobs::JobIds::NightLord: bonus = getSkillLevel(Skills::Assassin::ClawMastery) * 10; break;
		case Jobs::JobIds::Gunslinger:
		case Jobs::JobIds::Outlaw:
		case Jobs::JobIds::Corsair: bonus = getSkillLevel(Skills::Gunslinger::GunMastery) * 10; break;
		case Jobs::JobIds::NightWalker2:
		case Jobs::JobIds::NightWalker3: bonus = getSkillLevel(Skills::NightWalker::ClawMastery) * 10; break;
	}
	return bonus;
}

auto PlayerSkills::addCooldown(int32_t skillId, int16_t time) -> void {
	m_cooldowns[skillId] = time;
}

auto PlayerSkills::removeCooldown(int32_t skillId) -> void {
	auto kvp = m_cooldowns.find(skillId);
	if (kvp != std::end(m_cooldowns)) {
		m_cooldowns.erase(kvp);
	}
}

auto PlayerSkills::removeAllCooldowns() -> void {
	hash_map_t<int32_t, int16_t> dupe = m_cooldowns;
	for (const auto &kvp : dupe) {
		if (kvp.first != Skills::Buccaneer::TimeLeap) {
			Skills::stopCooldown(m_player, kvp.first);
		}
	}
}

auto PlayerSkills::connectData(PacketBuilder &packet) const -> void {
	// Skill levels
	packet.add<uint16_t>(m_skills.size());
	for (const auto &kvp : m_skills) {
		packet.add<int32_t>(kvp.first);
		packet.add<int32_t>(kvp.second.level);
		if (GameLogicUtilities::isFourthJobSkill(kvp.first)) {
			packet.add<int32_t>(kvp.second.playerMaxSkillLevel);
		}
	}
	// Cooldowns
	packet.add<uint16_t>(m_cooldowns.size());
	for (const auto &kvp : m_cooldowns) {
		packet.add<int32_t>(kvp.first);
		packet.add<int16_t>(kvp.second);
	}
}