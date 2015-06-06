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
#include "PlayerActiveBuffs.hpp"
#include "BuffsPacket.hpp"
#include "ChannelServer.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "Maps.hpp"
#include "PacketReader.hpp"
#include "Player.hpp"
#include "Randomizer.hpp"
#include "SkillDataProvider.hpp"
#include "Skills.hpp"
#include "SkillsPacket.hpp"
#include "TimeUtilities.hpp"
#include "Timer.hpp"
#include "TimerContainer.hpp"
#include <functional>

// Buff skills
auto PlayerActiveBuffs::addBuff(skill_id_t skill, const seconds_t &time) -> void {
	if (time.count() > 0) {
		// Only bother with timers when there is a time
		Timer::Id id(TimerType::BuffTimer, skill);

		if (GameLogicUtilities::isMobSkill(skill)) {
			mob_skill_id_t mobSkill = static_cast<mob_skill_id_t>(skill);
			Timer::Timer::create([this, mobSkill](const time_point_t &now) { this->removeDebuff(mobSkill, true); },
				id, m_player->getTimerContainer(), time);
		}
		else {
			Timer::Timer::create([this, skill](const time_point_t &now) { Skills::stopSkill(m_player, skill, true); },
				id, m_player->getTimerContainer(), time);
		}
	}
	m_buffs.push_back(skill);
}

auto PlayerActiveBuffs::removeBuff(skill_id_t skill, bool fromTimer) -> void {
	if (!fromTimer) {
		Timer::Id id(TimerType::BuffTimer, skill);
		m_player->getTimerContainer()->removeTimer(id);
	}
	removeAction(skill);

	auto iter = std::find(std::begin(m_buffs), std::end(m_buffs), skill);
	if (iter != std::end(m_buffs)) {
		m_buffs.erase(iter);
	}
}

auto PlayerActiveBuffs::removeBuffs() -> void {
	if (hasHyperBody()) {
		m_player->getStats()->setHyperBody(0, 0);
		m_player->getStats()->setHp(m_player->getStats()->getHp());
		m_player->getStats()->setMp(m_player->getStats()->getMp());
	}

	while (m_buffs.size() > 0) {
		skill_id_t skillId = *std::begin(m_buffs);
		removeBuff(skillId);
	}
}

auto PlayerActiveBuffs::getBuffSecondsRemaining(skill_id_t skill) const -> seconds_t {
	Timer::Id id(TimerType::BuffTimer, skill);
	return m_player->getTimerContainer()->getRemainingTime<seconds_t>(id);
}

// Skill actions
struct RunAction {
	auto operator()(const time_point_t &now) -> void {
		switch (act) {
			case ActHeal: Skills::heal(player, value, skill); break;
			case ActHurt: Skills::hurt(player, value, skill); break;
		}
	}

	int16_t value = 0;
	skill_id_t skill = 0;
	Player *player = nullptr;
	Action act;
};

auto PlayerActiveBuffs::addAction(skill_id_t skill, Action act, int16_t value, const milliseconds_t &time) -> void {
	RunAction runAct;
	runAct.player = m_player;
	runAct.skill = skill;
	runAct.act = act;
	runAct.value = value;

	Timer::Id id(TimerType::SkillActTimer, act);
	Timer::Timer::create(runAct, id, getActTimer(skill), seconds_t{0}, time);
}

auto PlayerActiveBuffs::getActTimer(skill_id_t skill) -> ref_ptr_t<Timer::Container> {
	if (m_skillActs.find(skill) == std::end(m_skillActs)) {
		m_skillActs[skill] = make_ref_ptr<Timer::Container>();
	}
	return m_skillActs[skill];
}

auto PlayerActiveBuffs::removeAction(skill_id_t skill) -> void {
	m_skillActs.erase(skill);
}

// Debuffs
auto PlayerActiveBuffs::addDebuff(mob_skill_id_t skill, mob_skill_level_t level) -> void {
	if (!m_player->getStats()->isDead() && !hasHolyShield() && !m_player->hasGmBenefits()) {
		int32_t maskBit = calculateDebuffMaskBit(skill);
		if (maskBit != 0 && (m_debuffMask & maskBit) == 0) {
			// Don't have the debuff, continue processing
			m_debuffMask += maskBit;
			Buffs::addDebuff(m_player, skill, level);
		}
	}
}

auto PlayerActiveBuffs::removeDebuff(mob_skill_id_t skill, bool fromTimer) -> void {
	int32_t maskBit = calculateDebuffMaskBit(skill);
	if ((m_debuffMask & maskBit) != 0) {
		m_debuffMask -= maskBit;
		Skills::stopSkill(m_player, skill, fromTimer);
	}
}

auto PlayerActiveBuffs::useDebuffHealingItem(int32_t mask) -> void {
	if ((mask & StatusEffects::Player::Seal) != 0) {
		removeDebuff(MobSkills::Seal);
	}
	if ((mask & StatusEffects::Player::Poison) != 0) {
		removeDebuff(MobSkills::Poison);
	}
	if ((mask & StatusEffects::Player::Curse) != 0) {
		removeDebuff(MobSkills::Curse);
	}
	if ((mask & StatusEffects::Player::Darkness) != 0) {
		removeDebuff(MobSkills::Darkness);
	}
	if ((mask & StatusEffects::Player::Weakness) != 0) {
		removeDebuff(MobSkills::Weakness);
	}
}

auto PlayerActiveBuffs::useDispel() -> void {
	removeDebuff(MobSkills::Seal);
	removeDebuff(MobSkills::Slow);
	removeDebuff(MobSkills::Darkness);
	removeDebuff(MobSkills::Weakness);
	removeDebuff(MobSkills::Curse);
	removeDebuff(MobSkills::Poison);
}

auto PlayerActiveBuffs::calculateDebuffMaskBit(mob_skill_id_t skill) -> int32_t {
	int32_t bitField = 0;
	switch (skill) {
		case MobSkills::Seal: bitField = StatusEffects::Player::Seal; break;
		case MobSkills::Darkness: bitField = StatusEffects::Player::Darkness; break;
		case MobSkills::Weakness: bitField = StatusEffects::Player::Weakness; break;
		case MobSkills::Stun: bitField = StatusEffects::Player::Stun; break;
		case MobSkills::Curse: bitField = StatusEffects::Player::Curse; break;
		case MobSkills::Poison: bitField = StatusEffects::Player::Poison; break;
		case MobSkills::Slow: bitField = StatusEffects::Player::Slow; break;
		case MobSkills::Seduce: bitField = StatusEffects::Player::Seduce; break;
		case MobSkills::Zombify: bitField = StatusEffects::Player::Zombify; break;
		case MobSkills::CrazySkull: bitField = StatusEffects::Player::CrazySkull; break;
	}
	return bitField;
}

// Map entry stuff
auto PlayerActiveBuffs::deleteMapEntryBuffInfo(const ActiveMapBuff &buff) -> void {
	size_t vals = 0;
	for (size_t i = 0; i < buff.bytes.size(); ++i) {
		uint8_t byte = buff.bytes[i];
		m_mapBuffs.types[byte] -= buff.types[i];
		if (m_mapBuffs.values[byte].find(buff.types[i]) != std::end(m_mapBuffs.values[byte])) {
			m_mapBuffs.values[byte].erase(buff.types[i]);
		}
	}
}

auto PlayerActiveBuffs::addMapEntryBuffInfo(const ActiveMapBuff &buff) -> void {
	size_t vals = 0;
	for (size_t i = 0; i < buff.bytes.size(); ++i) {
		uint8_t byte = buff.bytes[i];
		if ((m_mapBuffs.types[byte] & buff.types[i]) == 0) {
			m_mapBuffs.types[byte] += buff.types[i];
		}
		MapEntryVals val;
		val.use = buff.useVals[i];
		if (val.use) {
			if (buff.debuff) {
				val.debuff = true;
				val.skill = buff.values[vals++];
				val.val = buff.values[vals++];
			}
			else {
				val.val = buff.values[vals++];
			}
		}
		m_mapBuffs.values[byte][buff.types[i]] = val;
	}
}

auto PlayerActiveBuffs::getMapEntryBuffs() -> MapEntryBuffs {
	return m_mapBuffs;
}

auto PlayerActiveBuffs::setMountInfo(skill_id_t skillId, item_id_t mountId) -> void {
	m_mapBuffs.mountSkill = skillId;
	m_mapBuffs.mountId = mountId;
}

// Active skill levels
auto PlayerActiveBuffs::getActiveSkillLevel(skill_id_t skillId) const -> skill_level_t {
	return m_activeLevels.find(skillId) != std::end(m_activeLevels) ? m_activeLevels.find(skillId)->second : 0;
}

auto PlayerActiveBuffs::setActiveSkillLevel(skill_id_t skillId, skill_level_t level) -> void {
	m_activeLevels[skillId] = level;
}

auto PlayerActiveBuffs::getActiveSkillInfo(skill_id_t skillId) const -> const SkillLevelInfo * const {
	skill_level_t level = getActiveSkillLevel(skillId);
	return level != 0 ? ChannelServer::getInstance().getSkillDataProvider().getSkill(skillId, level) : nullptr;
}

// Buff addition/removal
auto PlayerActiveBuffs::addBuffInfo(skill_id_t skillId, const vector_t<Buff> &buffs) -> void {
	for (size_t i = 0; i < buffs.size(); ++i) {
		Buff cur = buffs[i];
		m_activeBuffsByType[cur.byte][cur.type] = skillId;
	}
}

auto PlayerActiveBuffs::removeBuffInfo(skill_id_t skillId, const vector_t<Buff> &buffs) -> ActiveBuff {
	ActiveBuff ret;
	for (size_t i = 0; i < buffs.size(); ++i) {
		Buff cur = buffs[i];
		if (m_activeBuffsByType[cur.byte][cur.type] == skillId) {
			// Make sure that the buff types are still affected by the skill
			m_activeBuffsByType[cur.byte].erase(cur.type);
			ret.types[cur.byte] += cur.type;
		}
	}
	return ret;
}

auto PlayerActiveBuffs::dispelBuffs() -> void {
	if (m_player->hasGmBenefits()) {
		return;
	}
	
	vector_t<skill_id_t> stopSkills;
	for (const auto &kvp : m_activeLevels) {
		if (kvp.first > 0 && !GameLogicUtilities::isMobSkill(kvp.first)) {
			// Only want active skills and skill buffs - no item buffs or debuffs
			stopSkills.push_back(kvp.first);
		}
	}

	for (const auto &skill : stopSkills) {
		Skills::stopSkill(m_player, skill);
	}
}

// Specific skill stuff
auto PlayerActiveBuffs::reduceBattleshipHp(uint16_t amount) -> void {
	m_battleshipHp -= amount;
	if (m_battleshipHp <= 0) {
		m_battleshipHp = 0;
		skill_id_t skillId = Skills::Corsair::Battleship;
		int16_t coolTime = getActiveSkillInfo(skillId)->coolTime;
		Skills::startCooldown(m_player, skillId, coolTime);
		Skills::stopSkill(m_player, skillId);
	}
}

auto PlayerActiveBuffs::resetBattleshipHp() -> void {
	skill_level_t shipLevel = m_player->getSkills()->getSkillLevel(Skills::Corsair::Battleship);
	player_level_t playerLevel = m_player->getStats()->getLevel();
	m_battleshipHp = GameLogicUtilities::getBattleshipHp(shipLevel, playerLevel);
}

auto PlayerActiveBuffs::setCombo(uint8_t combo, bool sendPacket) -> void {
	m_combo = combo;
	if (sendPacket) {
		skill_id_t skillId = m_player->getSkills()->getComboAttack();
		seconds_t timeLeft = getBuffSecondsRemaining(skillId);
		skill_level_t level = getActiveSkillLevel(skillId);
		ActiveBuff playerSkill = Buffs::parseBuffInfo(m_player, skillId, level);
		ActiveMapBuff mapSkill = Buffs::parseBuffMapInfo(m_player, skillId, level);
		m_player->sendMap(BuffsPacket::useSkill(m_player->getId(), skillId, timeLeft, playerSkill, mapSkill, 0));
	}
}

auto PlayerActiveBuffs::addCombo() -> void {
	skill_id_t skillId = m_player->getSkills()->getComboAttack();
	skill_level_t comboLevel = getActiveSkillLevel(skillId);
	if (comboLevel > 0) {
		skill_id_t advSkill = m_player->getSkills()->getAdvancedCombo();
		skill_level_t advCombo = m_player->getSkills()->getSkillLevel(advSkill);
		auto skill = ChannelServer::getInstance().getSkillDataProvider().getSkill(advCombo > 0 ? advSkill : skillId, advCombo > 0 ? advCombo : comboLevel);

		int8_t maxCombo = static_cast<int8_t>(skill->x);
		if (m_combo == maxCombo) {
			return;
		}

		if (advCombo > 0 && Randomizer::rand<uint16_t>(99) < skill->prop) {
			m_combo += 1;
		}
		m_combo += 1;
		if (m_combo > maxCombo) {
			m_combo = maxCombo;
		}

		setCombo(m_combo, true);
	}
}

auto PlayerActiveBuffs::checkBerserk(bool display) -> void {
	if (m_player->getStats()->getJob() == Jobs::JobIds::DarkKnight) {
		// Berserk calculations
		skill_id_t skillId = Skills::DarkKnight::Berserk;
		skill_level_t level = m_player->getSkills()->getSkillLevel(skillId);
		if (level > 0) {
			int16_t hpPercentage = m_player->getStats()->getMaxHp() * ChannelServer::getInstance().getSkillDataProvider().getSkill(skillId, level)->x / 100;
			health_t hp = m_player->getStats()->getHp();
			bool change = false;
			if (m_berserk && hp > hpPercentage) {
				m_berserk = false;
				change = true;
			}
			else if (!m_berserk && hp <= hpPercentage) {
				m_berserk = true;
				change = true;
			}
			if (change || display) {
				m_player->sendMap(SkillsPacket::showBerserk(m_player->getId(), level, m_berserk));
			}
		}
	}
}

auto PlayerActiveBuffs::increaseEnergyChargeLevel(int8_t targets) -> void {
	if (m_energyCharge != Stats::MaxEnergyChargeLevel && targets > 0) {
		skill_id_t skillId = m_player->getSkills()->getEnergyCharge();
		Timer::Id id(TimerType::BuffTimer, skillId, m_timeSeed);
		if (m_player->getTimerContainer()->isTimerRunning(id)) {
			stopEnergyChargeTimer();
		}
		startEnergyChargeTimer();
		m_energyCharge += m_player->getSkills()->getSkillInfo(skillId)->x * targets;
		if (m_energyCharge > Stats::MaxEnergyChargeLevel) {
			m_energyCharge = Stats::MaxEnergyChargeLevel;
			stopEnergyChargeTimer();
		}
		Buffs::addBuff(m_player, skillId, m_player->getSkills()->getSkillLevel(skillId), 0);
	}
}

auto PlayerActiveBuffs::decreaseEnergyChargeLevel() -> void {
	m_energyCharge -= Stats::EnergyChargeDecay;
	skill_id_t skillId = m_player->getSkills()->getEnergyCharge();
	if (m_energyCharge < 0) {
		m_energyCharge = 0;
	}
	else {
		startEnergyChargeTimer();
	}
	Buffs::addBuff(m_player, skillId, m_player->getSkills()->getSkillLevel(skillId), 0);
}

auto PlayerActiveBuffs::resetEnergyChargeLevel() -> void {
	m_energyCharge = 0;
}

auto PlayerActiveBuffs::startEnergyChargeTimer() -> void {
	m_timeSeed = static_cast<uint32_t>(clock());
	skill_id_t skillId = m_player->getSkills()->getEnergyCharge();
	Timer::Id id(TimerType::BuffTimer, skillId, m_timeSeed); // Causes heap errors when it's a static number, but we need it for ID
	Timer::Timer::create([this](const time_point_t &now) { this->decreaseEnergyChargeLevel(); },
		id, m_player->getTimerContainer(), seconds_t{10});
}

auto PlayerActiveBuffs::setEnergyChargeLevel(int16_t chargeLevel, bool startTimer) -> void {
	m_energyCharge = chargeLevel;
	if (startTimer) {
		startEnergyChargeTimer();
	}
}

auto PlayerActiveBuffs::stopEnergyChargeTimer() -> void {
	skill_id_t skillId = m_player->getSkills()->getEnergyCharge();
	Timer::Id id(TimerType::BuffTimer, skillId, m_timeSeed);
	m_player->getTimerContainer()->removeTimer(id);
}

auto PlayerActiveBuffs::stopBooster() -> void {
	if (m_activeBooster != 0) {
		Skills::stopSkill(m_player, m_activeBooster);
	}
}

auto PlayerActiveBuffs::stopCharge() -> void {
	if (m_activeCharge != 0) {
		Skills::stopSkill(m_player, m_activeCharge);
	}
}

auto PlayerActiveBuffs::stopBulletSkills() -> void {
	if (hasBuff(Skills::Hunter::SoulArrow)) {
		Skills::stopSkill(m_player, Skills::Hunter::SoulArrow);
	}
	else if (hasBuff(Skills::Crossbowman::SoulArrow)) {
		Skills::stopSkill(m_player, Skills::Crossbowman::SoulArrow);
	}
	else if (hasBuff(Skills::WindArcher::SoulArrow)) {
		Skills::stopSkill(m_player, Skills::WindArcher::SoulArrow);
	}
	else if (hasBuff(Skills::NightLord::ShadowStars)) {
		Skills::stopSkill(m_player, Skills::NightLord::ShadowStars);
	}
}

auto PlayerActiveBuffs::hasBuff(skill_id_t skillId) -> bool {
	return getActiveSkillLevel(skillId) > 0;
}

auto PlayerActiveBuffs::hasIceCharge() const -> bool {
	return m_activeCharge == Skills::WhiteKnight::BwIceCharge || m_activeCharge == Skills::WhiteKnight::SwordIceCharge;
}

auto PlayerActiveBuffs::hasInfinity() -> bool {
	return hasBuff(Skills::FpArchMage::Infinity) || hasBuff(Skills::IlArchMage::Infinity) || hasBuff(Skills::Bishop::Infinity);
}

auto PlayerActiveBuffs::hasMesoUp() -> bool {
	return hasBuff(Skills::Hermit::MesoUp);
}

auto PlayerActiveBuffs::hasMagicGuard() -> bool {
	return getMagicGuard() != 0;
}

auto PlayerActiveBuffs::hasMesoGuard() -> bool {
	return getMesoGuard() != 0;
}

auto PlayerActiveBuffs::hasHolySymbol() -> bool {
	return getHolySymbol() != 0;
}

auto PlayerActiveBuffs::hasPowerStance() -> bool {
	return getPowerStance() != 0;
}

auto PlayerActiveBuffs::hasHyperBody() -> bool {
	return getHyperBody() != 0;
}

auto PlayerActiveBuffs::isUsingGmHide() -> bool {
	return hasBuff(Skills::SuperGm::Hide);
}

auto PlayerActiveBuffs::hasShadowPartner() -> bool {
	return hasBuff(Skills::Hermit::ShadowPartner) || hasBuff(Skills::NightWalker::ShadowPartner);
}

auto PlayerActiveBuffs::hasShadowStars() -> bool {
	return hasBuff(Skills::NightLord::ShadowStars);
}

auto PlayerActiveBuffs::hasSoulArrow() -> bool {
	return hasBuff(Skills::Hunter::SoulArrow) || hasBuff(Skills::Crossbowman::SoulArrow) || hasBuff(Skills::WindArcher::SoulArrow);
}

auto PlayerActiveBuffs::hasHolyShield() -> bool {
	return (hasBuff(Skills::Bishop::HolyShield));
}

auto PlayerActiveBuffs::isCursed() -> bool {
	return (m_debuffMask & StatusEffects::Player::Curse) > 0;
}

auto PlayerActiveBuffs::isZombified() -> bool {
	return (m_debuffMask & StatusEffects::Player::Zombify) > 0;
}

auto PlayerActiveBuffs::getHolySymbolRate() -> int16_t {
	int16_t val = 0;
	if (hasHolySymbol()) {
		skill_id_t hsSkill = getHolySymbol();
		val = getActiveSkillInfo(hsSkill)->x;
	}
	return val;
}

auto PlayerActiveBuffs::getMagicGuard() -> skill_id_t {
	skill_id_t id = 0;
	if (hasBuff(Skills::Magician::MagicGuard)) {
		id = Skills::Magician::MagicGuard;
	}
	else if (hasBuff(Skills::BlazeWizard::MagicGuard)) {
		id = Skills::BlazeWizard::MagicGuard;
	}
	return id;
}

auto PlayerActiveBuffs::getMesoGuard() -> skill_id_t {
	skill_id_t id = 0;
	if (hasBuff(Skills::ChiefBandit::MesoGuard)) {
		id = Skills::ChiefBandit::MesoGuard;
	}
	return id;
}

auto PlayerActiveBuffs::getHolySymbol() -> skill_id_t {
	skill_id_t id = 0;
	if (hasBuff(Skills::Priest::HolySymbol)) {
		id = Skills::Priest::HolySymbol;
	}
	else if (hasBuff(Skills::SuperGm::HolySymbol)) {
		id = Skills::SuperGm::HolySymbol;
	}
	return id;
}

auto PlayerActiveBuffs::getPowerStance() -> skill_id_t {
	skill_id_t skillId = 0;
	if (hasBuff(Skills::Hero::PowerStance)) {
		skillId = Skills::Hero::PowerStance;
	}
	else if (hasBuff(Skills::Paladin::PowerStance)) {
		skillId = Skills::Paladin::PowerStance;
	}
	else if (hasBuff(Skills::DarkKnight::PowerStance)) {
		skillId = Skills::DarkKnight::PowerStance;
	}
	else if (hasBuff(Skills::Marauder::EnergyCharge)) {
		skillId = Skills::Marauder::EnergyCharge;
	}
	else if (hasBuff(Skills::ThunderBreaker::EnergyCharge)) {
		skillId = Skills::ThunderBreaker::EnergyCharge;
	}
	return skillId;
}

auto PlayerActiveBuffs::getHyperBody() -> skill_id_t {
	skill_id_t id = 0;
	if (hasBuff(Skills::Spearman::HyperBody)) {
		id = Skills::Spearman::HyperBody;
	}
	else if (hasBuff(Skills::SuperGm::HyperBody)) {
		id = Skills::SuperGm::HyperBody;
	}
	return id;
}

auto PlayerActiveBuffs::getHomingBeacon() -> skill_id_t {
	skill_id_t id = 0;
	if (hasBuff(Skills::Outlaw::HomingBeacon)) {
		id = Skills::Outlaw::HomingBeacon;
	}
	else if (hasBuff(Skills::Corsair::Bullseye)) {
		id = Skills::Corsair::Bullseye;
	}
	return id;
}

auto PlayerActiveBuffs::getCurrentMorph() -> skill_id_t {
	skill_id_t morphId = 0;
	if (m_activeBuffsByType.find(BuffBytes::Byte5) != std::end(m_activeBuffsByType)) {
		hash_map_t<uint8_t, skill_id_t> byte = m_activeBuffsByType[BuffBytes::Byte5];
		if (byte.find(0x02) != std::end(byte)) {
			morphId = byte[0x02];
		}
	}
	return morphId;
}

auto PlayerActiveBuffs::endMorph() -> void {
	skill_id_t morphId = getCurrentMorph();
	if (morphId != 0) {
		Skills::stopSkill(m_player, morphId);
	}
}

auto PlayerActiveBuffs::swapWeapon() -> void {
	stopBooster();
	stopCharge();
	stopBulletSkills();
}

auto PlayerActiveBuffs::getTransferPacket() const -> PacketBuilder {
	PacketBuilder builder;
	// Map entry buff info
	builder.add<int8_t>(getCombo());
	builder.add<int16_t>(getEnergyChargeLevel());
	builder.add<skill_id_t>(getCharge());
	builder.add<skill_id_t>(getBooster());
	builder.add<int32_t>(getBattleshipHp());
	builder.add<int32_t>(getDebuffMask());
	builder.add<item_id_t>(m_mapBuffs.mountId);
	builder.add<skill_id_t>(m_mapBuffs.mountSkill);
	for (int8_t i = 0; i < BuffBytes::ByteQuantity; ++i) {
		builder.add<uint8_t>(m_mapBuffs.types[i]);
		auto foundValue = m_mapBuffs.values.find(i);
		if (foundValue == std::end(m_mapBuffs.values)) {
			builder.add<uint8_t>(0);
			continue;
		}
		auto &values = foundValue->second;
		builder.add<uint8_t>(static_cast<uint8_t>(values.size()));
		for (const auto &kvp : values) {
			const MapEntryVals &info = kvp.second;
			builder.add<uint8_t>(kvp.first);
			builder.add<bool>(info.debuff);
			if (kvp.second.debuff) {
				builder.add<int16_t>(info.skill);
				builder.add<int16_t>(info.val);
			}
			else {
				builder.add<bool>(info.use);
				builder.add<int16_t>(info.val);
			}
		}
	}
	// Current buff info (IDs, times, levels)
	builder.add<uint8_t>(static_cast<uint8_t>(m_buffs.size()));
	for (const auto &buffId : m_buffs) {
		builder.add<skill_id_t>(buffId);
		builder.add<seconds_t>(getBuffSecondsRemaining(buffId));
		builder.add<skill_level_t>(getActiveSkillLevel(buffId));
	}
	// Current buffs by type info
	for (int8_t i = 0; i < BuffBytes::ByteQuantity; ++i) {
		auto foundByte = m_activeBuffsByType.find(i);
		if (foundByte == std::end(m_activeBuffsByType)) {
			builder.add<uint8_t>(0);
			continue;
		}
		auto &currentByte = foundByte->second;
		builder.add<uint8_t>(static_cast<uint8_t>(currentByte.size()));
		for (const auto &kvp : currentByte) {
			builder.add<uint8_t>(kvp.first);
			builder.add<skill_id_t>(kvp.second);
		}
	}
	return builder;
}

auto PlayerActiveBuffs::parseTransferPacket(PacketReader &reader) -> void {
	// Map entry buff info
	setCombo(reader.get<uint8_t>(), false);
	setEnergyChargeLevel(reader.get<int16_t>());
	setCharge(reader.get<skill_id_t>());
	setBooster(reader.get<skill_id_t>());
	setBattleshipHp(reader.get<int32_t>());
	setDebuffMask(reader.get<int32_t>());
	m_mapBuffs.mountId = reader.get<item_id_t>();
	m_mapBuffs.mountSkill = reader.get<skill_id_t>();
	MapEntryVals values;
	for (int8_t i = 0; i < BuffBytes::ByteQuantity; ++i) {
		m_mapBuffs.types[i] = reader.get<uint8_t>();
		uint8_t size = reader.get<uint8_t>();
		for (uint8_t f = 0; f < size; f++) {
			uint8_t type = reader.get<uint8_t>();
			values.debuff = reader.get<bool>();
			if (values.debuff) {
				values.skill = reader.get<int16_t>();
				values.val = reader.get<int16_t>();
			}
			else {
				values.use = reader.get<bool>();
				values.val = reader.get<int16_t>();
			}
			m_mapBuffs.values[i][type] = values;
		}
	}
	// Current buff info
	uint8_t nBuffs = reader.get<uint8_t>();
	for (uint8_t i = 0; i < nBuffs; ++i) {
		skill_id_t skillId = reader.get<skill_id_t>();
		seconds_t timeLeft = reader.get<seconds_t>();
		skill_level_t level = reader.get<skill_level_t>();
		addBuff(skillId, timeLeft);
		setActiveSkillLevel(skillId, level);
		Buffs::doAction(m_player, skillId, level);
	}
	// Current buffs by type
	hash_map_t<uint8_t, skill_id_t> currentByte;
	for (int8_t i = 0; i < BuffBytes::ByteQuantity; ++i) {
		uint8_t size = reader.get<uint8_t>();
		for (uint8_t f = 0; f < size; f++) {
			uint8_t key = reader.get<uint8_t>();
			skill_id_t value = reader.get<skill_id_t>();
			currentByte[key] = value;
		}
		m_activeBuffsByType[i] = currentByte;
	}

	if (hasHyperBody()) {
		skill_id_t skillId = getHyperBody();
		skill_level_t hbLevel = getActiveSkillLevel(skillId);
		auto hb = ChannelServer::getInstance().getSkillDataProvider().getSkill(skillId, hbLevel);
		m_player->getStats()->setHyperBody(hb->x, hb->y);
	}
}