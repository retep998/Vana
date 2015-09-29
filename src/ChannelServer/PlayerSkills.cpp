/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "Common/Algorithm.hpp"
#include "Common/Database.hpp"
#include "Common/GameConstants.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "Common/Randomizer.hpp"
#include "Common/SkillDataProvider.hpp"
#include "Common/Timer.hpp"
#include "Common/TimerId.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/Map.hpp"
#include "ChannelServer/MapPacket.hpp"
#include "ChannelServer/MysticDoor.hpp"
#include "ChannelServer/Party.hpp"
#include "ChannelServer/PartyPacket.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/Skills.hpp"
#include "ChannelServer/SkillsPacket.hpp"

namespace Vana {
namespace ChannelServer {

PlayerSkills::PlayerSkills(Player *player) :
	m_player{player}
{
	load();
}

auto PlayerSkills::load() -> void {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	PlayerSkillInfo skill;
	player_id_t playerId = m_player->getId();
	skill_id_t skillId = 0;

	soci::rowset<> rs = (sql.prepare
		<< "SELECT s.skill_id, s.points, s.max_level "
		<< "FROM " << db.makeTable("skills") << " s "
		<< "WHERE s.character_id = :char",
		soci::use(playerId, "char"));

	for (const auto &row : rs) {
		skillId = row.get<skill_id_t>("skill_id");
		if (GameLogicUtilities::isBlessingOfTheFairy(skillId)) {
			continue;
		}

		skill = PlayerSkillInfo{};
		skill.level = row.get<skill_level_t>("points");
		skill.maxSkillLevel = ChannelServer::getInstance().getSkillDataProvider().getMaxLevel(skillId);
		skill.playerMaxSkillLevel = row.get<skill_level_t>("max_level");
		m_skills[skillId] = skill;
	}

	rs = (sql.prepare
		<< "SELECT c.* "
		<< "FROM " << db.makeTable("cooldowns") << " c "
		<< "WHERE c.character_id = :char",
		soci::use(playerId, "char"));

	for (const auto &row : rs) {
		skill_id_t skillId = row.get<skill_id_t>("skill_id");
		seconds_t timeLeft = seconds_t{row.get<int16_t>("remaining_time")};
		Skills::startCooldown(m_player, skillId, timeLeft, true);
		m_cooldowns[skillId] = timeLeft;
	}

	skillId = getBlessingOfTheFairy();

	opt_string_t blessingPlayerName;
	optional_t<player_level_t> blessingPlayerLevel;

	account_id_t accountId = m_player->getAccountId();
	world_id_t worldId = m_player->getWorldId();

	// TODO FIXME skill
	// Allow Cygnus <-> Adventurer selection here or allow it to be ignored
	// That is, some versions only allowed Adv. Blessing to be populated by Cygnus levels and vice versa
	// Some later versions lifted this restriction entirely
	sql.once
		<< "SELECT c.name, c.level "
		<< "FROM " << db.makeTable("characters") << " c "
		<< "WHERE c.world_id = :world AND c.account_id = :account AND c.character_id <> :char "
		<< "ORDER BY c.level DESC "
		<< "LIMIT 1 ",
		soci::use(accountId, "account"),
		soci::use(worldId, "world"),
		soci::use(playerId, "char"),
		soci::into(blessingPlayerName),
		soci::into(blessingPlayerLevel);

	if (blessingPlayerLevel.is_initialized()) {
		skill = PlayerSkillInfo{};
		skill.maxSkillLevel = ChannelServer::getInstance().getSkillDataProvider().getMaxLevel(skillId);
		skill.level = std::min<skill_level_t>(blessingPlayerLevel.get() / 10, skill.maxSkillLevel);
		m_blessingPlayer = blessingPlayerName.get();
		m_skills[skillId] = skill;
	}
}

auto PlayerSkills::save(bool saveCooldowns) -> void {
	using namespace soci;
	player_id_t playerId = m_player->getId();
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();

	skill_id_t skillId = 0;
	skill_level_t level = 0;
	skill_level_t maxLevel = 0;
	statement st = (sql.prepare
		<< "REPLACE INTO " << db.makeTable("skills") << " VALUES (:player, :skill, :level, :maxLevel)",
		use(playerId, "player"),
		use(skillId, "skill"),
		use(level, "level"),
		use(maxLevel, "maxLevel"));

	for (const auto &kvp : m_skills) {
		if (GameLogicUtilities::isBlessingOfTheFairy(kvp.first)) {
			continue;
		}
		skillId = kvp.first;
		level = kvp.second.level;
		maxLevel = kvp.second.playerMaxSkillLevel;
		st.execute(true);
	}

	if (saveCooldowns) {
		sql.once << "DELETE FROM " << db.makeTable("cooldowns") << " WHERE character_id = :char", soci::use(playerId, "char");

		if (m_cooldowns.size() > 0) {
			int16_t remainingTime = 0;
			st = (sql.prepare
				<< "INSERT INTO " << db.makeTable("cooldowns") << " (character_id, skill_id, remaining_time) "
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

auto PlayerSkills::addSkillLevel(skill_id_t skillId, skill_level_t amount, bool sendPacket) -> bool {
	if (!ChannelServer::getInstance().getSkillDataProvider().isValidSkill(skillId)) {
		return false;
	}

	// Keep people from adding too much SP and prevent it from going negative

	auto kvp = m_skills.find(skillId);
	skill_level_t newLevel = (kvp != std::end(m_skills) ? kvp->second.level : 0) + amount;
	skill_level_t maxSkillLevel = ChannelServer::getInstance().getSkillDataProvider().getMaxLevel(skillId);
	if (newLevel > maxSkillLevel || (GameLogicUtilities::isFourthJobSkill(skillId) && newLevel > getMaxSkillLevel(skillId))) {
		return false;
	}

	m_skills[skillId].level = newLevel;
	m_skills[skillId].maxSkillLevel = maxSkillLevel;
	if (sendPacket) {
		m_player->send(Packets::Skills::addSkill(skillId, m_skills[skillId]));
	}
	return true;
}

auto PlayerSkills::getSkillLevel(skill_id_t skillId) const -> skill_level_t {
	auto skill = ext::find_value_ptr(m_skills, skillId);
	return skill == nullptr ? 0 : skill->level;
}

auto PlayerSkills::setMaxSkillLevel(skill_id_t skillId, skill_level_t maxLevel, bool sendPacket) -> void {
	// Set max level for 4th job skills
	m_skills[skillId].playerMaxSkillLevel = maxLevel;

	if (sendPacket) {
		m_player->getSkills()->addSkillLevel(skillId, 0);
	}
}

auto PlayerSkills::getMaxSkillLevel(skill_id_t skillId) const -> skill_level_t {
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

auto PlayerSkills::getSkillInfo(skill_id_t skillId) const -> const SkillLevelInfo * const {
	auto skill = ext::find_value_ptr(m_skills, skillId);
	return skill == nullptr ? nullptr : ChannelServer::getInstance().getSkillDataProvider().getSkill(skillId, skill->level);
}

auto PlayerSkills::hasSkill(skill_id_t skillId) const -> bool {
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

auto PlayerSkills::hasDarkSightInterruptionSkill() const -> bool {
	return hasSkill(getDarkSightInterruptionSkill());
}

auto PlayerSkills::hasNoDamageSkill() const -> bool {
	return hasSkill(getNoDamageSkill());
}

auto PlayerSkills::hasFollowTheLead() const -> bool {
	return hasSkill(getFollowTheLead());
}

auto PlayerSkills::hasLegendarySpirit() const -> bool {
	return hasSkill(getLegendarySpirit());
}

auto PlayerSkills::hasMaker() const -> bool {
	return hasSkill(getMaker());
}

auto PlayerSkills::hasBlessingOfTheFairy() const -> bool {
	return hasSkill(getBlessingOfTheFairy());
}

auto PlayerSkills::getElementalAmp() const -> skill_id_t {
	skill_id_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::FpMage:
		case Jobs::JobIds::FpArchMage: skillId = Vana::Skills::FpMage::ElementAmplification; break;
		case Jobs::JobIds::IlMage:
		case Jobs::JobIds::IlArchMage: skillId = Vana::Skills::IlMage::ElementAmplification; break;
		case Jobs::JobIds::BlazeWizard3:
		case Jobs::JobIds::BlazeWizard4: skillId = Vana::Skills::BlazeWizard::ElementAmplification; break;
	}
	return skillId;
}

auto PlayerSkills::getAchilles() const -> skill_id_t {
	skill_id_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Hero: skillId = Vana::Skills::Hero::Achilles; break;
		case Jobs::JobIds::Paladin: skillId = Vana::Skills::Paladin::Achilles; break;
		case Jobs::JobIds::DarkKnight: skillId = Vana::Skills::DarkKnight::Achilles; break;
	}
	return skillId;
}

auto PlayerSkills::getEnergyCharge() const -> skill_id_t {
	skill_id_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Marauder:
		case Jobs::JobIds::Buccaneer: skillId = Vana::Skills::Marauder::EnergyCharge; break;
		case Jobs::JobIds::ThunderBreaker2:
		case Jobs::JobIds::ThunderBreaker3:
		case Jobs::JobIds::ThunderBreaker4: skillId = Vana::Skills::ThunderBreaker::EnergyCharge; break;
	}
	return skillId;
}

auto PlayerSkills::getAdvancedCombo() const -> skill_id_t {
	skill_id_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Hero: skillId = Vana::Skills::Hero::AdvancedComboAttack; break;
		case Jobs::JobIds::DawnWarrior3:
		case Jobs::JobIds::DawnWarrior4: skillId = Vana::Skills::DawnWarrior::AdvancedCombo; break;
	}
	return skillId;
}

auto PlayerSkills::getAlchemist() const -> skill_id_t {
	skill_id_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Hermit:
		case Jobs::JobIds::NightLord: skillId = Vana::Skills::Hermit::Alchemist; break;
		case Jobs::JobIds::NightWalker3:
		case Jobs::JobIds::NightWalker4: skillId = Vana::Skills::NightWalker::Alchemist; break;
	}
	return skillId;
}

auto PlayerSkills::getHpIncrease() const -> skill_id_t {
	skill_id_t skillId = 0;
	switch (GameLogicUtilities::getJobTrack(m_player->getStats()->getJob())) {
		case Jobs::JobTracks::Warrior: skillId = Vana::Skills::Swordsman::ImprovedMaxHpIncrease; break;
		case Jobs::JobTracks::DawnWarrior: skillId = Vana::Skills::DawnWarrior::MaxHpEnhancement; break;
		case Jobs::JobTracks::ThunderBreaker: skillId = Vana::Skills::ThunderBreaker::ImproveMaxHp; break;
		case Jobs::JobTracks::Pirate:
			if ((m_player->getStats()->getJob() / 10) == (Jobs::JobIds::Brawler / 10)) {
				skillId = Vana::Skills::Brawler::ImproveMaxHp;
			}
			break;
	}
	return skillId;
}

auto PlayerSkills::getMpIncrease() const -> skill_id_t {
	skill_id_t skillId = 0;
	switch (GameLogicUtilities::getJobTrack(m_player->getStats()->getJob())) {
		case Jobs::JobTracks::Magician: skillId = Vana::Skills::Magician::ImprovedMaxMpIncrease; break;
		case Jobs::JobTracks::BlazeWizard: skillId = Vana::Skills::BlazeWizard::IncreasingMaxMp; break;
	}
	return skillId;
}

auto PlayerSkills::getMastery() const -> skill_id_t {
	skill_id_t masteryId = 0;
	switch (GameLogicUtilities::getItemType(m_player->getInventory()->getEquippedId(EquipSlots::Weapon))) {
		case Items::Types::Weapon1hSword:
		case Items::Types::Weapon2hSword:
			switch (m_player->getStats()->getJob()) {
				case Jobs::JobIds::Fighter:
				case Jobs::JobIds::Crusader:
				case Jobs::JobIds::Hero: masteryId = Vana::Skills::Fighter::SwordMastery; break;
				case Jobs::JobIds::Page:
				case Jobs::JobIds::WhiteKnight:
				case Jobs::JobIds::Paladin: masteryId = Vana::Skills::Page::SwordMastery; break;
			}
			break;
		case Items::Types::Weapon1hAxe:
		case Items::Types::Weapon2hAxe: masteryId = Vana::Skills::Fighter::AxeMastery; break;
		case Items::Types::Weapon1hMace:
		case Items::Types::Weapon2hMace: masteryId = Vana::Skills::Page::BwMastery; break;
		case Items::Types::WeaponSpear: masteryId = Vana::Skills::Spearman::SpearMastery; break;
		case Items::Types::WeaponPolearm: masteryId = Vana::Skills::Spearman::PolearmMastery; break;
		case Items::Types::WeaponDagger: masteryId = Vana::Skills::Bandit::DaggerMastery; break;
		case Items::Types::WeaponKnuckle: masteryId = Vana::Skills::Brawler::KnucklerMastery; break;
		case Items::Types::WeaponBow: masteryId = Vana::Skills::Hunter::BowMastery; break;
		case Items::Types::WeaponCrossbow: masteryId = Vana::Skills::Crossbowman::CrossbowMastery; break;
		case Items::Types::WeaponClaw: masteryId = Vana::Skills::Assassin::ClawMastery; break;
		case Items::Types::WeaponGun: masteryId = Vana::Skills::Gunslinger::GunMastery; break;
	}
	return masteryId;
}

auto PlayerSkills::getMpEater() const -> skill_id_t {
	skill_id_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::FpWizard:
		case Jobs::JobIds::FpMage:
		case Jobs::JobIds::FpArchMage: skillId = Vana::Skills::FpWizard::MpEater; break;
		case Jobs::JobIds::IlWizard:
		case Jobs::JobIds::IlMage:
		case Jobs::JobIds::IlArchMage: skillId = Vana::Skills::IlWizard::MpEater; break;
		case Jobs::JobIds::Cleric:
		case Jobs::JobIds::Priest:
		case Jobs::JobIds::Bishop: skillId = Vana::Skills::Cleric::MpEater; break;
	}
	return skillId;
}

auto PlayerSkills::getVenomousWeapon() const -> skill_id_t {
	skill_id_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::NightLord: skillId = Vana::Skills::NightLord::VenomousStar; break;
		case Jobs::JobIds::Shadower: skillId = Vana::Skills::Shadower::VenomousStab; break;
		case Jobs::JobIds::NightWalker3: 
		case Jobs::JobIds::NightWalker4: skillId = Vana::Skills::NightWalker::Venom; break;
	}
	return skillId;
}

auto PlayerSkills::getDarkSightInterruptionSkill() const -> skill_id_t {
	skill_id_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::NightWalker2:
		case Jobs::JobIds::NightWalker3:
		case Jobs::JobIds::NightWalker4: skillId = Vana::Skills::NightWalker::Vanish; break;
		case Jobs::JobIds::WindArcher2:
		case Jobs::JobIds::WindArcher3: 
		case Jobs::JobIds::WindArcher4: skillId = Vana::Skills::WindArcher::WindWalk; break;
	}
	return skillId;
}

auto PlayerSkills::getNoDamageSkill() const -> skill_id_t {
	skill_id_t skillId = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::NightLord: skillId = Vana::Skills::NightLord::ShadowShifter; break;
		case Jobs::JobIds::Shadower: skillId = Vana::Skills::Shadower::ShadowShifter; break;
		case Jobs::JobIds::Hero: skillId = Vana::Skills::Hero::Guardian; break;
		case Jobs::JobIds::Paladin: skillId = Vana::Skills::Paladin::Guardian; break;
	}
	return skillId;
}

auto PlayerSkills::getFollowTheLead() const -> skill_id_t {
	skill_id_t skillId = 0;
	switch (GameLogicUtilities::getJobType(m_player->getStats()->getJob())) {
		case Jobs::JobType::Adventurer: skillId = Vana::Skills::Beginner::FollowTheLead; break;
		case Jobs::JobType::Cygnus: skillId = Vana::Skills::Noblesse::FollowTheLead; break;
	}
	return skillId;
}

auto PlayerSkills::getLegendarySpirit() const -> skill_id_t {
	skill_id_t skillId = 0;
	switch (GameLogicUtilities::getJobType(m_player->getStats()->getJob())) {
		case Jobs::JobType::Adventurer: skillId = Vana::Skills::Beginner::LegendarySpirit; break;
		case Jobs::JobType::Cygnus: skillId = Vana::Skills::Noblesse::LegendarySpirit; break;
	}
	return skillId;
}

auto PlayerSkills::getMaker() const -> skill_id_t {
	skill_id_t skillId = 0;
	switch (GameLogicUtilities::getJobType(m_player->getStats()->getJob())) {
		case Jobs::JobType::Adventurer: skillId = Vana::Skills::Beginner::Maker; break;
		case Jobs::JobType::Cygnus: skillId = Vana::Skills::Noblesse::Maker; break;
	}
	return skillId;
}

auto PlayerSkills::getBlessingOfTheFairy() const -> skill_id_t {
	skill_id_t skillId = 0;
	switch (GameLogicUtilities::getJobType(m_player->getStats()->getJob())) {
		case Jobs::JobType::Adventurer: skillId = Vana::Skills::Beginner::BlessingOfTheFairy; break;
		case Jobs::JobType::Cygnus: skillId = Vana::Skills::Noblesse::BlessingOfTheFairy; break;
	}
	return skillId;
}

auto PlayerSkills::getRechargeableBonus() const -> slot_qty_t {
	slot_qty_t bonus = 0;
	switch (m_player->getStats()->getJob()) {
		case Jobs::JobIds::Assassin:
		case Jobs::JobIds::Hermit:
		case Jobs::JobIds::NightLord: bonus = getSkillLevel(Vana::Skills::Assassin::ClawMastery) * 10; break;
		case Jobs::JobIds::Gunslinger:
		case Jobs::JobIds::Outlaw:
		case Jobs::JobIds::Corsair: bonus = getSkillLevel(Vana::Skills::Gunslinger::GunMastery) * 10; break;
		case Jobs::JobIds::NightWalker2:
		case Jobs::JobIds::NightWalker3:
		case Jobs::JobIds::NightWalker4: bonus = getSkillLevel(Vana::Skills::NightWalker::ClawMastery) * 10; break;
	}
	return bonus;
}

auto PlayerSkills::addCooldown(skill_id_t skillId, seconds_t time) -> void {
	m_cooldowns[skillId] = time;
}

auto PlayerSkills::removeCooldown(skill_id_t skillId) -> void {
	auto kvp = m_cooldowns.find(skillId);
	if (kvp != std::end(m_cooldowns)) {
		m_cooldowns.erase(kvp);
	}
}

auto PlayerSkills::removeAllCooldowns() -> void {
	auto dupe = m_cooldowns;
	for (const auto &kvp : dupe) {
		if (kvp.first != Vana::Skills::Buccaneer::TimeLeap) {
			Skills::stopCooldown(m_player, kvp.first);
		}
	}
}

auto PlayerSkills::openMysticDoor(const Point &pos, seconds_t doorTime) -> MysticDoorResult {
	Party *party = m_player->getParty();
	bool isDisplacement = m_mysticDoor != nullptr;

	uint8_t zeroBasedPartyIndex = 0;
	if (party != nullptr) {
		zeroBasedPartyIndex = party->getZeroBasedIndexByMember(m_player);
	}

	MysticDoorOpenResult result = party == nullptr ?
		m_player->getMap()->getTownMysticDoorPortal(m_player) :
		m_player->getMap()->getTownMysticDoorPortal(m_player, zeroBasedPartyIndex);

	if (result.result != MysticDoorResult::Success) {
		return result.result;
	}

	if (isDisplacement) {
		if (party != nullptr) {
			party->runFunction([&](Player *partyMember) {
				if (partyMember->getMapId() == m_mysticDoor->getMapId()) {
					partyMember->send(Packets::Map::removeDoor(m_mysticDoor, false));
				}
			});
		}
		else {
			m_player->send(Packets::Map::removeDoor(m_mysticDoor, false));
		}
	}

	auto townId = result.townId;
	auto portal = result.portal;
	m_mysticDoor = make_ref_ptr<MysticDoor>(m_player, townId, portal->id, pos, portal->pos, isDisplacement, doorTime);

	if (party != nullptr) {
		party->runFunction([&](Player *partyMember) {
			bool sendSpawnPacket = false;
			bool inTown = false;
			if (partyMember->getMapId() == m_mysticDoor->getMapId()) {
				sendSpawnPacket = true;
			}
			else if (partyMember->getMapId() == m_mysticDoor->getTownId()) {
				sendSpawnPacket = true;
				inTown = true;
			}
			if (sendSpawnPacket) {
				partyMember->send(Packets::Map::spawnDoor(m_mysticDoor, false, false));
				partyMember->send(Packets::Party::updateDoor(zeroBasedPartyIndex, m_mysticDoor));
			}
		});
	}
	else {
		m_player->send(Packets::Map::spawnDoor(m_mysticDoor, false, false));
		m_player->send(Packets::Map::spawnPortal(m_mysticDoor, m_player->getMapId()));
	}

	Vana::Timer::Timer::create(
		[this](const time_point_t &now) {
			this->closeMysticDoor(true);
		},
		Vana::Timer::Id{TimerType::DoorTimer},
		m_player->getTimerContainer(),
		m_mysticDoor->getDoorTime());

	return MysticDoorResult::Success;
}

auto PlayerSkills::closeMysticDoor(bool fromTimer) -> void {
	if (!fromTimer) {
		m_player->getTimerContainer()->removeTimer(Vana::Timer::Id{TimerType::DoorTimer});
	}

	ref_ptr_t<MysticDoor> door = m_mysticDoor;
	m_mysticDoor.reset();

	if (Party *party = m_player->getParty()) {
		uint8_t zeroBasedPartyIndex = party->getZeroBasedIndexByMember(m_player);

		party->runFunction([&](Player *partyMember) {
			map_id_t memberMap = partyMember->getMapId();
			if (memberMap == door->getMapId()) {
				partyMember->send(Packets::Map::removeDoor(door, fromTimer));
			}
			else if (memberMap == door->getTownId()) {
				partyMember->send(Packets::Party::updateDoor(zeroBasedPartyIndex, m_mysticDoor));
			}
		});
	}
	else {
		map_id_t playerMap = m_player->getMapId();
		if (fromTimer && (playerMap == door->getMapId() || playerMap == door->getTownId())) {
			m_player->send(Packets::Map::removeDoor(door, true));
			m_player->send(Packets::Map::removePortal());
		}
	}
}

auto PlayerSkills::getMysticDoor() const -> ref_ptr_t<MysticDoor> {
	return m_mysticDoor;
}

auto PlayerSkills::onJoinParty(Party *party, Player *player) -> void {
	if (player == m_player) {
		if (m_mysticDoor == nullptr) {
			return;
		}

		uint8_t zeroBasedPartyIndex = party->getZeroBasedIndexByMember(player);
		MysticDoorOpenResult result = m_mysticDoor->getMap()->getTownMysticDoorPortal(player, zeroBasedPartyIndex);
		if (result.result != MysticDoorResult::Success) {
			// ???
			return;
		}

		auto portal = result.portal;
		m_mysticDoor = m_mysticDoor->withNewPortal(portal->id, portal->pos);

		// The actual door itself doesn't have to be modified on the map if the player happens to be there
		// If the player is in town, the join party packet takes care of it

		return;
	}

	if (ref_ptr_t<MysticDoor> door = player->getSkills()->getMysticDoor()) {
		if (m_player->getMapId() == door->getMapId()) {
			m_player->send(Packets::Map::spawnDoor(door, false, true));
		}
	}

	if (m_mysticDoor != nullptr) {
		uint8_t zeroBasedPartyIndex = party->getZeroBasedIndexByMember(m_player);
		MysticDoorOpenResult result = m_mysticDoor->getMap()->getTownMysticDoorPortal(m_player, zeroBasedPartyIndex);
		if (result.result != MysticDoorResult::Success) {
			// ???
			return;
		}

		auto portal = result.portal;
		m_mysticDoor = m_mysticDoor->withNewPortal(portal->id, portal->pos);

		if (player->getMapId() == m_mysticDoor->getMapId()) {
			player->send(Packets::Map::spawnDoor(m_mysticDoor, false, true));
		}
	}
}

auto PlayerSkills::onLeaveParty(Party *party, Player *player, bool kicked) -> void {
	if (player == m_player) {
		if (m_mysticDoor == nullptr) {
			return;
		}

		MysticDoorOpenResult result = m_mysticDoor->getMap()->getTownMysticDoorPortal(player);
		if (result.result != MysticDoorResult::Success) {
			// ???
			return;
		}

		auto portal = result.portal;
		m_mysticDoor = m_mysticDoor->withNewPortal(portal->id, portal->pos);

		// The actual door itself doesn't have to be modified on the map if the player happens to be there

		if (player->getMapId() == m_mysticDoor->getTownId()) {
			player->send(Packets::Party::updateDoor(0, nullptr));
			player->send(Packets::Map::spawnDoor(m_mysticDoor, true, true));
			player->send(Packets::Map::spawnPortal(m_mysticDoor, player->getMapId()));
		}

		return;
	}

	if (ref_ptr_t<MysticDoor> door = player->getSkills()->getMysticDoor()) {
		if (m_player->getMapId() == door->getMapId()) {
			m_player->send(Packets::Map::removeDoor(door, false));
		}
	}

	if (m_mysticDoor != nullptr) {
		uint8_t zeroBasedPartyIndex = party->getZeroBasedIndexByMember(m_player);
		MysticDoorOpenResult result = m_mysticDoor->getMap()->getTownMysticDoorPortal(m_player, zeroBasedPartyIndex);
		if (result.result != MysticDoorResult::Success) {
			// ???
			return;
		}

		auto portal = result.portal;
		m_mysticDoor = m_mysticDoor->withNewPortal(portal->id, portal->pos);

		if (player->getMapId() == m_mysticDoor->getMapId()) {
			player->send(Packets::Map::removeDoor(m_mysticDoor, false));
		}
	}
}

auto PlayerSkills::onPartyDisband(Party *party) -> void {
	if (m_mysticDoor == nullptr) {
		return;
	}

	uint8_t zeroBasedPartyIndex = party->getZeroBasedIndexByMember(m_player);
	party->runFunction([&](Player *partyMember) {
		map_id_t memberMap = partyMember->getMapId();
		if (memberMap == m_mysticDoor->getTownId()) {
			partyMember->send(Packets::Party::updateDoor(zeroBasedPartyIndex, nullptr));
		}
		else if (partyMember != m_player && memberMap == m_mysticDoor->getMapId()) {
			partyMember->send(Packets::Map::removeDoor(m_mysticDoor, false));
		}
	});

	MysticDoorOpenResult result = m_mysticDoor->getMap()->getTownMysticDoorPortal(m_player);
	if (result.result != MysticDoorResult::Success) {
		// ???
		return;
	}

	auto portal = result.portal;
	auto newDoor = m_mysticDoor->withNewPortal(portal->id, portal->pos);

	if (m_player->getMapId() == newDoor->getTownId()) {
		m_player->send(Packets::Map::spawnDoor(newDoor, true, true));
		m_player->send(Packets::Map::spawnPortal(newDoor, m_player->getMapId()));
	}

	m_mysticDoor = newDoor;
}

auto PlayerSkills::onMapChange() const -> void {
	if (Party *party = m_player->getParty()) {
		party->runFunction([&](Player *partyMember) {
			if (ref_ptr_t<MysticDoor> door = partyMember->getSkills()->getMysticDoor()) {
				if (m_player->getMapId() == door->getMapId()) {
					m_player->send(Packets::Map::spawnDoor(door, false, true));
				}
			}
		});

		// Unconditional return here since the player is in the party
		return;
	}

	if (m_mysticDoor == nullptr) {
		return;
	}

	map_id_t mapId = m_player->getMapId();
	bool inTown = mapId == m_mysticDoor->getTownId();
	if (mapId == m_mysticDoor->getMapId() || inTown) {
		m_player->send(Packets::Map::spawnDoor(m_mysticDoor, inTown, true));
		m_player->send(Packets::Map::spawnPortal(m_mysticDoor, mapId));
	}
}

auto PlayerSkills::onDisconnect() -> void {
	if (m_mysticDoor != nullptr) {
		closeMysticDoor(false);
	}
}

auto PlayerSkills::connectPacket(PacketBuilder &builder) const -> void {
	// Skill levels
	builder.add<uint16_t>(static_cast<uint16_t>(m_skills.size()));
	for (const auto &kvp : m_skills) {
		builder.add<skill_id_t>(kvp.first);
		builder.add<int32_t>(kvp.second.level);
		if (GameLogicUtilities::isFourthJobSkill(kvp.first)) {
			builder.add<int32_t>(kvp.second.playerMaxSkillLevel);
		}
	}
	// Cooldowns
	builder.add<uint16_t>(static_cast<uint16_t>(m_cooldowns.size()));
	for (const auto &kvp : m_cooldowns) {
		builder.add<skill_id_t>(kvp.first);
		builder.add<int16_t>(static_cast<int16_t>(kvp.second.count()));
	}
}

auto PlayerSkills::connectPacketForBlessing(PacketBuilder &builder) const -> void {
	// Orange text wasn't added until sometime after .75 and before .82
	//if (!m_blessingPlayer.empty()) {
	//	packet.add<bool>(true);
	//	packet.add<string_t>(m_blessingPlayer);
	//}
	//else {
	//	packet.add<bool>(false);
	//}
}

}
}