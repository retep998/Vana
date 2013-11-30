/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "PlayerActiveBuffs.h"
#include "BuffsPacket.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "Randomizer.h"
#include "SkillDataProvider.h"
#include "Skills.h"
#include "SkillsPacket.h"
#include "TimeUtilities.h"
#include "Timer.h"
#include "TimerContainer.h"
#include <functional>

using std::bind;

// Buff skills
void PlayerActiveBuffs::addBuff(int32_t skill, const seconds_t &time) {
	if (time.count() > 0) {
		// Only bother with timers when there is a time
		Timer::Id id(Timer::Types::BuffTimer, skill, 0);

		if (GameLogicUtilities::isMobSkill(skill)) {
			uint8_t mobSkill = static_cast<uint8_t>(skill);
			new Timer::Timer(bind(&PlayerActiveBuffs::removeDebuff, this, mobSkill, true),
				id, m_player->getTimers(), time);
		}
		else {
			new Timer::Timer(bind(&Skills::stopSkill, m_player, skill, true),
				id, m_player->getTimers(), time);
		}
	}
	m_buffs.push_back(skill);
}

void PlayerActiveBuffs::removeBuff(int32_t skill, bool fromTimer) {
	if (!fromTimer) {
		Timer::Id id(Timer::Types::BuffTimer, skill, 0);
		m_player->getTimers()->removeTimer(id);
	}
	removeAction(skill);
	m_buffs.remove(skill);
}

void PlayerActiveBuffs::removeBuffs() {
	if (hasHyperBody()) {
		m_player->getStats()->setHyperBody(0, 0);
		m_player->getStats()->setHp(m_player->getStats()->getHp());
		m_player->getStats()->setMp(m_player->getStats()->getMp());
	}

	while (m_buffs.size() > 0) {
		int32_t skillId = *m_buffs.begin();
		removeBuff(skillId);
	}
}

seconds_t PlayerActiveBuffs::getBuffSecondsRemaining(int32_t skill) const {
	Timer::Id id(Timer::Types::BuffTimer, skill, 0);
	return m_player->getTimers()->getSecondsRemaining(id);
}

// Skill actions
struct RunAction {
	void operator()() {
		switch (act) {
			case ActHeal: Skills::heal(player, value, skill); break;
			case ActHurt: Skills::hurt(player, value, skill); break;
		}
	}
	Player *player;
	int32_t skill;
	Action act;
	int16_t value;
};

void PlayerActiveBuffs::addAction(int32_t skill, Action act, int16_t value, const milliseconds_t &time) {
	RunAction runAct;
	runAct.player = m_player;
	runAct.skill = skill;
	runAct.act = act;
	runAct.value = value;

	Timer::Id id(Timer::Types::SkillActTimer, act, 0);
	new Timer::Timer(runAct, id, getActTimer(skill), seconds_t(0), time);
}

Timer::Container * PlayerActiveBuffs::getActTimer(int32_t skill) {
	if (m_skillActs.find(skill) == m_skillActs.end()) {
		m_skillActs[skill] = shared_ptr<Timer::Container>(new Timer::Container);
	}
	return m_skillActs[skill].get();
}

void PlayerActiveBuffs::removeAction(int32_t skill) {
	m_skillActs.erase(skill);
}

// Debuffs
void PlayerActiveBuffs::addDebuff(uint8_t skill, uint8_t level) {
	if (!m_player->getStats()->isDead() && !hasHolyShield() && !isUsingHide() && !m_player->hasGmEquip()) {
		int32_t maskBit = calculateDebuffMaskBit(skill);
		if (maskBit != 0 && (m_debuffMask & maskBit) == 0) {
			// Don't have the debuff, continue processing
			m_debuffMask += maskBit;
			Buffs::addDebuff(m_player, skill, level);
		}
	}
}

void PlayerActiveBuffs::removeDebuff(uint8_t skill, bool fromTimer) {
	int32_t maskBit = calculateDebuffMaskBit(skill);
	if ((m_debuffMask & maskBit) != 0) {
		m_debuffMask -= maskBit;
		Skills::stopSkill(m_player, skill, fromTimer);
	}
}

void PlayerActiveBuffs::useDebuffHealingItem(int32_t mask) {
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

void PlayerActiveBuffs::useDispel() {
	removeDebuff(MobSkills::Seal);
	removeDebuff(MobSkills::Slow);
	removeDebuff(MobSkills::Darkness);
	removeDebuff(MobSkills::Weakness);
	removeDebuff(MobSkills::Curse);
	removeDebuff(MobSkills::Poison);
}

int32_t PlayerActiveBuffs::calculateDebuffMaskBit(uint8_t skill) {
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
void PlayerActiveBuffs::deleteMapEntryBuffInfo(const ActiveMapBuff &buff) {
	size_t vals = 0;
	for (size_t i = 0; i < buff.bytes.size(); ++i) {
		uint8_t byte = buff.bytes[i];
		m_mapBuffs.types[byte] -= buff.types[i];
		if (m_mapBuffs.values[byte].find(buff.types[i]) != m_mapBuffs.values[byte].end()) {
			m_mapBuffs.values[byte].erase(buff.types[i]);
		}
	}
}

void PlayerActiveBuffs::addMapEntryBuffInfo(const ActiveMapBuff &buff) {
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

MapEntryBuffs PlayerActiveBuffs::getMapEntryBuffs() {
	return m_mapBuffs;
}

void PlayerActiveBuffs::setMountInfo(int32_t skillId, int32_t mountId) {
	m_mapBuffs.mountSkill = skillId;
	m_mapBuffs.mountId = mountId;
}

// Active skill levels
uint8_t PlayerActiveBuffs::getActiveSkillLevel(int32_t skillId) const {
	return m_activeLevels.find(skillId) != m_activeLevels.end() ? m_activeLevels.find(skillId)->second : 0;
}

void PlayerActiveBuffs::setActiveSkillLevel(int32_t skillId, uint8_t level) {
	m_activeLevels[skillId] = level;
}

SkillLevelInfo * PlayerActiveBuffs::getActiveSkillInfo(int32_t skillId) {
	uint8_t level = getActiveSkillLevel(skillId);
	return (level != 0 ? SkillDataProvider::Instance()->getSkill(skillId, level) : 0);
}

// Buff addition/removal
void PlayerActiveBuffs::addBuffInfo(int32_t skillId, const vector<Buff> &buffs) {
	for (size_t i = 0; i < buffs.size(); ++i) {
		Buff cur = buffs[i];
		m_activeBuffsByType[cur.byte][cur.type] = skillId;
	}
}

ActiveBuff PlayerActiveBuffs::removeBuffInfo(int32_t skillId, const vector<Buff> &buffs) {
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

void PlayerActiveBuffs::dispelBuffs() {
	if (isUsingHide()) {
		return;
	}
	
	vector<int32_t> stopSkills;
	for (auto iter = m_activeLevels.cbegin(); iter != m_activeLevels.cend(); ++iter) {
		if (iter->first > 0 && !GameLogicUtilities::isMobSkill(iter->first)) {
			// Only want active skills and skill buffs - no item buffs or debuffs
			stopSkills.push_back(iter->first);
		}
	}

	for (auto skill : stopSkills) {
		Skills::stopSkill(m_player, skill);
	}
}

// Specific skill stuff
void PlayerActiveBuffs::reduceBattleshipHp(uint16_t amount) {
	m_battleshipHp -= amount;
	if (m_battleshipHp <= 0) {
		m_battleshipHp = 0;
		int32_t skillId = Skills::Corsair::Battleship;
		int16_t coolTime = getActiveSkillInfo(skillId)->coolTime;
		Skills::startCooldown(m_player, skillId, coolTime);
		Skills::stopSkill(m_player, skillId);
	}
}

void PlayerActiveBuffs::resetBattleshipHp() {
	uint8_t shipLevel = m_player->getSkills()->getSkillLevel(Skills::Corsair::Battleship);
	uint8_t playerLevel = m_player->getStats()->getLevel();
	m_battleshipHp = GameLogicUtilities::getBattleshipHp(shipLevel, playerLevel);
}

void PlayerActiveBuffs::setCombo(uint8_t combo, bool sendPacket) {
	m_combo = combo;
	if (sendPacket) {
		int32_t skillId = m_player->getSkills()->getComboAttack();
		seconds_t timeLeft = getBuffSecondsRemaining(skillId);
		uint8_t level = getActiveSkillLevel(skillId);
		ActiveBuff playerSkill = Buffs::parseBuffInfo(m_player, skillId, level);
		ActiveMapBuff mapSkill = Buffs::parseBuffMapInfo(m_player, skillId, level);
		BuffsPacket::useSkill(m_player, skillId, timeLeft, playerSkill, mapSkill, 0);
	}
}

void PlayerActiveBuffs::addCombo() {
	int32_t skillId = m_player->getSkills()->getComboAttack();
	uint8_t cLevel = getActiveSkillLevel(skillId);
	if (cLevel > 0) {
		int32_t advSkill = m_player->getSkills()->getAdvancedCombo();
		uint8_t advCombo = m_player->getSkills()->getSkillLevel(advSkill);
		SkillLevelInfo *combo;
		uint16_t prop = 0;
		if (advCombo > 0) {
			combo = SkillDataProvider::Instance()->getSkill(advSkill, advCombo);
			prop = combo->prop;
		}
		else {
			combo = SkillDataProvider::Instance()->getSkill(skillId, cLevel);
		}
		int8_t maxCombo = static_cast<int8_t>(combo->x);
		if (m_combo == maxCombo) {
			return;
		}
		if (advCombo > 0 && Randomizer::rand<uint16_t>(99) < prop) {
			m_combo += 1;
		}
		m_combo += 1;
		if (m_combo > maxCombo) {
			m_combo = maxCombo;
		}
		setCombo(m_combo, true);
	}
}

void PlayerActiveBuffs::checkBerserk(bool display) {
	if (m_player->getStats()->getJob() == Jobs::JobIds::DarkKnight) {
		// Berserk calculations
		int32_t skillId = Skills::DarkKnight::Berserk;
		int8_t level = m_player->getSkills()->getSkillLevel(skillId);
		if (level > 0) {
			int16_t hpPercentage = m_player->getStats()->getMaxHp() * SkillDataProvider::Instance()->getSkill(skillId, level)->x / 100;
			int16_t hp = m_player->getStats()->getHp();
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
				SkillsPacket::showBerserk(m_player, level, m_berserk);
			}
		}
	}
}

void PlayerActiveBuffs::increaseEnergyChargeLevel(int8_t targets) {
	if (m_energyCharge != Stats::MaxEnergyChargeLevel && targets > 0) {
		int32_t skillId = m_player->getSkills()->getEnergyCharge();
		Timer::Id id(Timer::Types::BuffTimer, skillId, m_timeSeed);
		if (m_player->getTimers()->isTimerRunning(id)) {
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

void PlayerActiveBuffs::decreaseEnergyChargeLevel() {
	m_energyCharge -= Stats::EnergyChargeDecay;
	int32_t skillId = m_player->getSkills()->getEnergyCharge();
	if (m_energyCharge < 0) {
		m_energyCharge = 0;
	}
	else {
		startEnergyChargeTimer();
	}
	Buffs::addBuff(m_player, skillId, m_player->getSkills()->getSkillLevel(skillId), 0);
}

void PlayerActiveBuffs::resetEnergyChargeLevel() {
	m_energyCharge = 0;
}

void PlayerActiveBuffs::startEnergyChargeTimer() {
	m_timeSeed = static_cast<uint32_t>(clock());
	int32_t skillId = m_player->getSkills()->getEnergyCharge();
	Timer::Id id(Timer::Types::BuffTimer, skillId, m_timeSeed); // Causes heap errors when it's a static number, but we need it for ID
	new Timer::Timer(bind(&PlayerActiveBuffs::decreaseEnergyChargeLevel, this),
		id, m_player->getTimers(), seconds_t(10));
}

void PlayerActiveBuffs::setEnergyChargeLevel(int16_t chargeLevel, bool startTimer) {
	m_energyCharge = chargeLevel;
	if (startTimer) {
		startEnergyChargeTimer();
	}
}

void PlayerActiveBuffs::stopEnergyChargeTimer() {
	int32_t skillId = m_player->getSkills()->getEnergyCharge();
	Timer::Id id(Timer::Types::BuffTimer, skillId, m_timeSeed);
	m_player->getTimers()->removeTimer(id);
}

void PlayerActiveBuffs::stopBooster() {
	if (m_activeBooster != 0) {
		Skills::stopSkill(m_player, m_activeBooster);
	}
}

void PlayerActiveBuffs::stopCharge() {
	if (m_activeCharge != 0) {
		Skills::stopSkill(m_player, m_activeCharge);
	}
}

void PlayerActiveBuffs::stopBulletSkills() {
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

bool PlayerActiveBuffs::hasBuff(int32_t skillId) {
	return (getActiveSkillLevel(skillId) > 0);
}

bool PlayerActiveBuffs::hasIceCharge() const {
	return (m_activeCharge == Skills::WhiteKnight::BwIceCharge || m_activeCharge == Skills::WhiteKnight::SwordIceCharge);
}

bool PlayerActiveBuffs::hasInfinity() {
	return (hasBuff(Skills::FpArchMage::Infinity) || hasBuff(Skills::IlArchMage::Infinity) || hasBuff(Skills::Bishop::Infinity));
}

bool PlayerActiveBuffs::hasMesoUp() {
	return hasBuff(Skills::Hermit::MesoUp);
}

bool PlayerActiveBuffs::hasMagicGuard() {
	return (getMagicGuard() != 0);
}

bool PlayerActiveBuffs::hasMesoGuard() {
	return (getMesoGuard() != 0);
}

bool PlayerActiveBuffs::hasHolySymbol() {
	return (getHolySymbol() != 0);
}

bool PlayerActiveBuffs::hasPowerStance() {
	return (getPowerStance() != 0);
}

bool PlayerActiveBuffs::hasHyperBody() {
	return (getHyperBody() != 0);
}

bool PlayerActiveBuffs::isUsingHide() {
	return (hasBuff(Skills::SuperGm::Hide));
}

bool PlayerActiveBuffs::hasShadowPartner() {
	return (hasBuff(Skills::Hermit::ShadowPartner) || hasBuff(Skills::NightWalker::ShadowPartner));
}

bool PlayerActiveBuffs::hasShadowStars() {
	return (hasBuff(Skills::NightLord::ShadowStars));
}

bool PlayerActiveBuffs::hasSoulArrow() {
	return (hasBuff(Skills::Hunter::SoulArrow) || hasBuff(Skills::Crossbowman::SoulArrow) || hasBuff(Skills::WindArcher::SoulArrow));
}

bool PlayerActiveBuffs::hasHolyShield() {
	return (hasBuff(Skills::Bishop::HolyShield));
}

bool PlayerActiveBuffs::isCursed() {
	return ((m_debuffMask & StatusEffects::Player::Curse) > 0);
}

bool PlayerActiveBuffs::isZombified() {
	return ((m_debuffMask & StatusEffects::Player::Zombify) > 0);
}

int16_t PlayerActiveBuffs::getHolySymbolRate() {
	int16_t val = 0;
	if (hasHolySymbol()) {
		int32_t hsSkill = getHolySymbol();
		val = getActiveSkillInfo(hsSkill)->x;
	}
	return val;
}

int32_t PlayerActiveBuffs::getMagicGuard() {
	int32_t id = 0;
	if (hasBuff(Skills::Magician::MagicGuard)) {
		id = Skills::Magician::MagicGuard;
	}
	else if (hasBuff(Skills::BlazeWizard::MagicGuard)) {
		id = Skills::BlazeWizard::MagicGuard;
	}
	return id;
}

int32_t PlayerActiveBuffs::getMesoGuard() {
	int32_t id = 0;
	if (hasBuff(Skills::ChiefBandit::MesoGuard)) {
		id = Skills::ChiefBandit::MesoGuard;
	}
	return id;
}

int32_t PlayerActiveBuffs::getHolySymbol() {
	int32_t id = 0;
	if (hasBuff(Skills::Priest::HolySymbol)) {
		id = Skills::Priest::HolySymbol;
	}
	else if (hasBuff(Skills::SuperGm::HolySymbol)) {
		id = Skills::SuperGm::HolySymbol;
	}
	return id;
}

int32_t PlayerActiveBuffs::getPowerStance() {
	int32_t skillId = 0;
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

int32_t PlayerActiveBuffs::getHyperBody() {
	int32_t id = 0;
	if (hasBuff(Skills::Spearman::HyperBody)) {
		id = Skills::Spearman::HyperBody;
	}
	else if (hasBuff(Skills::SuperGm::HyperBody)) {
		id = Skills::SuperGm::HyperBody;
	}
	return id;
}

int32_t PlayerActiveBuffs::getHomingBeacon() {
	int32_t id = 0;
	if (hasBuff(Skills::Outlaw::HomingBeacon)) {
		id = Skills::Outlaw::HomingBeacon;
	}
	else if (hasBuff(Skills::Corsair::Bullseye)) {
		id = Skills::Corsair::Bullseye;
	}
	return id;
}

int32_t PlayerActiveBuffs::getCurrentMorph() {
	int32_t morphId = 0;
	if (m_activeBuffsByType.find(BuffBytes::Byte5) != m_activeBuffsByType.end()) {
		unordered_map<uint8_t, int32_t> byte = m_activeBuffsByType[BuffBytes::Byte5];
		if (byte.find(0x02) != byte.end()) {
			morphId = byte[0x02];
		}
	}
	return morphId;
}

void PlayerActiveBuffs::endMorph() {
	int32_t morphId = getCurrentMorph();
	if (morphId != 0) {
		Skills::stopSkill(m_player, morphId);
	}
}

void PlayerActiveBuffs::swapWeapon() {
	stopBooster();
	stopCharge();
	stopBulletSkills();
}

void PlayerActiveBuffs::write(PacketCreator &packet) const {
	// Map entry buff info
	packet.add<int8_t>(getCombo());
	packet.add<int16_t>(getEnergyChargeLevel());
	packet.add<int32_t>(getCharge());
	packet.add<int32_t>(getBooster());
	packet.add<int32_t>(getBattleshipHp());
	packet.add<int32_t>(getDebuffMask());
	packet.add<int32_t>(m_mapBuffs.mountId);
	packet.add<int32_t>(m_mapBuffs.mountSkill);
	for (int8_t i = 0; i < BuffBytes::ByteQuantity; ++i) {
		packet.add<uint8_t>(m_mapBuffs.types[i]);
		auto foundValue = m_mapBuffs.values.find(i);
		if (foundValue == m_mapBuffs.values.end()) {
			packet.add<uint8_t>(0);
			continue;
		}
		auto &values = foundValue->second;
		packet.add<uint8_t>(values.size());
		for (unordered_map<uint8_t, MapEntryVals>::const_iterator iter = values.begin(); iter != values.end(); ++iter) {
			packet.add<uint8_t>(iter->first);
			packet.add<bool>(iter->second.debuff);
			if (iter->second.debuff) {
				packet.add<int16_t>(iter->second.skill);
				packet.add<int16_t>(iter->second.val);
			}
			else {
				packet.add<bool>(iter->second.use);
				packet.add<int16_t>(iter->second.val);
			}
		}
	}
	// Current buff info (IDs, times, levels)
	packet.add<uint8_t>(m_buffs.size());
	for (list<int32_t>::const_iterator iter = m_buffs.begin(); iter != m_buffs.end(); ++iter) {
		int32_t buffId = *iter;
		packet.add<int32_t>(buffId);
		packet.add<int32_t>(static_cast<int32_t>(getBuffSecondsRemaining(buffId).count()));
		packet.add<uint8_t>(getActiveSkillLevel(buffId));
	}
	// Current buffs by type info
	for (int8_t i = 0; i < BuffBytes::ByteQuantity; ++i) {
		auto foundByte = m_activeBuffsByType.find(i);
		if (foundByte == m_activeBuffsByType.end()) {
			packet.add<uint8_t>(0);
			continue;
		}
		auto &currentByte = foundByte->second;
		packet.add<uint8_t>(currentByte.size());
		for (unordered_map<uint8_t, int32_t>::const_iterator iter = currentByte.begin(); iter != currentByte.end(); ++iter) {
			packet.add<uint8_t>(iter->first);
			packet.add<int32_t>(iter->second);
		}
	}
}

void PlayerActiveBuffs::read(PacketReader &packet) {
	// Map entry buff info
	setCombo(packet.get<uint8_t>(), false);
	setEnergyChargeLevel(packet.get<int16_t>());
	setCharge(packet.get<int32_t>());
	setBooster(packet.get<int32_t>());
	setBattleshipHp(packet.get<int32_t>());
	setDebuffMask(packet.get<int32_t>());
	m_mapBuffs.mountId = packet.get<int32_t>();
	m_mapBuffs.mountSkill = packet.get<int32_t>();
	MapEntryVals values;
	for (int8_t i = 0; i < BuffBytes::ByteQuantity; ++i) {
		m_mapBuffs.types[i] = packet.get<uint8_t>();
		uint8_t size = packet.get<uint8_t>();
		for (uint8_t f = 0; f < size; f++) {
			uint8_t type = packet.get<uint8_t>();
			values.debuff = packet.get<bool>();
			if (values.debuff) {
				values.skill = packet.get<int16_t>();
				values.val = packet.get<int16_t>();
			}
			else {
				values.use = packet.get<bool>();
				values.val = packet.get<int16_t>();
			}
			m_mapBuffs.values[i][type] = values;
		}
	}
	// Current buff info
	uint8_t nBuffs = packet.get<uint8_t>();
	for (uint8_t i = 0; i < nBuffs; ++i) {
		int32_t skillId = packet.get<int32_t>();
		seconds_t timeLeft(packet.get<int32_t>());
		uint8_t level = packet.get<uint8_t>();
		addBuff(skillId, timeLeft);
		setActiveSkillLevel(skillId, level);
		Buffs::doAction(m_player, skillId, level);
	}
	// Current buffs by type
	unordered_map<uint8_t, int32_t> currentByte;
	for (int8_t i = 0; i < BuffBytes::ByteQuantity; ++i) {
		uint8_t size = packet.get<uint8_t>();
		for (uint8_t f = 0; f < size; f++) {
			uint8_t key = packet.get<uint8_t>();
			int32_t value = packet.get<int32_t>();
			currentByte[key] = value;
		}
		m_activeBuffsByType[i] = currentByte;
	}
}