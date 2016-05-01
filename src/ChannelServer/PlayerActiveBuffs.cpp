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
#include "PlayerActiveBuffs.hpp"
#include "Common/GameConstants.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "Common/PacketReader.hpp"
#include "Common/Randomizer.hpp"
#include "Common/SkillDataProvider.hpp"
#include "Common/TimeUtilities.hpp"
#include "Common/Timer.hpp"
#include "Common/TimerContainer.hpp"
#include "ChannelServer/BuffsPacket.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/Skills.hpp"
#include "ChannelServer/SkillsPacket.hpp"
#include <functional>

namespace Vana {
namespace ChannelServer {

struct BuffRunAction {
	BuffRunAction(BuffSource source) :
		source{source}
	{
	}

	auto operator()(const time_point_t &now) -> void {
		switch (act) {
			case BuffAction::Heal: Skills::heal(player, value, source); break;
			case BuffAction::Hurt: Skills::hurt(player, value, source); break;
			default: throw NotImplementedException{"Action type"};
		}
	}

	int64_t value = 0;
	BuffSource source;
	ref_ptr_t<Player> player = nullptr;
	BuffAction act;
};

auto PlayerActiveBuffs::LocalBuffInfo::toSource() const -> BuffSource {
	switch (type) {
		case BuffSourceType::Item:
			return BuffSource::fromItem(identifier);
		case BuffSourceType::Skill:
			return BuffSource::fromSkill(
				static_cast<skill_id_t>(identifier),
				static_cast<skill_level_t>(level));
		case BuffSourceType::MobSkill:
			return BuffSource::fromMobSkill(
				static_cast<mob_skill_id_t>(identifier),
				static_cast<mob_skill_level_t>(level));
	}
	throw NotImplementedException{"BuffSourceType"};
}

PlayerActiveBuffs::PlayerActiveBuffs(Player *player) :
	m_player{player}
{
}

// Buff skills
auto PlayerActiveBuffs::translateToSource(int32_t buffId) const -> BuffSource {
	if (buffId < 0) {
		return BuffSource::fromItem(-buffId);
	}
	if (GameLogicUtilities::isMobSkill(buffId)) {
		return BuffSource::fromMobSkill(
			buffId,
			static_cast<mob_skill_level_t>(getBuffLevel(BuffSourceType::MobSkill, buffId)));
	}
	return BuffSource::fromSkill(
		buffId,
		static_cast<skill_level_t>(getBuffLevel(BuffSourceType::Skill, buffId)));
}

auto PlayerActiveBuffs::translateToPacket(const BuffSource &source) const -> int32_t {
	switch (source.getType()) {
		case BuffSourceType::Item: return -source.getId();
		case BuffSourceType::Skill: return source.getId();
		case BuffSourceType::MobSkill: return (source.getMobSkillLevel() << 16) | source.getId();
	}
	throw NotImplementedException{"BuffSourceType"};
}

auto PlayerActiveBuffs::addBuff(const BuffSource &source, const Buff &buff, const seconds_t &time) -> Result {
	bool hasTimer = true;
	bool displaces = true;
	const auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	auto &skillProvider = ChannelServer::getInstance().getSkillDataProvider();
	auto skill = source.getSkillData(skillProvider);
	auto mobSkill = source.getMobSkillData(skillProvider);

	switch (source.getType()) {
		case BuffSourceType::Item:
			// Intentionally blank
			break;
		case BuffSourceType::MobSkill: {
			mob_skill_id_t skillId = source.getMobSkillId();
			int32_t maskBit = calculateDebuffMaskBit(skillId);
			m_debuffMask |= maskBit;
			break;
		}
		case BuffSourceType::Skill: {
			skill_id_t skillId = source.getSkillId();
			skill_level_t skillLevel = source.getSkillLevel();
			switch (source.getSkillId()) {
				case Vana::Skills::Beginner::MonsterRider:
				case Vana::Skills::Noblesse::MonsterRider: {
					m_mountItemId = m_player->getInventory()->getEquippedId(EquipSlots::Mount);
					if (m_mountItemId == 0) {
						// Hacking
						return Result::Failure;
					}
					item_id_t saddle = m_player->getInventory()->getEquippedId(EquipSlots::Saddle);
					if (saddle == 0) {
						// Hacking
						return Result::Failure;
					}
					break;
				}
				case Vana::Skills::Corsair::Battleship:
					m_mountItemId = Items::BattleshipMount;
					break;
				case Vana::Skills::Hero::Enrage:
					if (m_combo != Vana::Skills::MaxAdvancedComboOrbs) {
						// Hacking
						return Result::Failure;
					}
					break;
			}

			break;
		}
		default: throw NotImplementedException{"BuffSourceType"};
	}

	// Extract any useful bits for us
	for (const auto &info : buff.getBuffInfo()) {
		if (info == basics.homingBeacon) {
			hasTimer = false;
			displaces = false;
		}
		else if (info == basics.combo) {
			m_combo = 0;
		}
		else if (info == basics.zombify) {
			if (mobSkill == nullptr) throw NotImplementedException{"Zombify BuffSourceType"};
			m_zombifyPotency = mobSkill->x;
		}
		else if (info == basics.mapleWarrior) {
			if (skill == nullptr) throw NotImplementedException{"Maple Warrior BuffSourceType"};
			// Take into account Maple Warrior for tracking stats if things are equippable, damage calculations, etc.
			m_player->getStats()->setMapleWarrior(skill->x);
		}
		else if (info == basics.hyperBodyHp) {
			if (skill == nullptr) throw NotImplementedException{"Hyper Body HP BuffSourceType"};
			m_player->getStats()->setHyperBodyHp(skill->x);
		}
		else if (info == basics.hyperBodyMp) {
			if (skill == nullptr) throw NotImplementedException{"Hyper Body MP BuffSourceType"};
			m_player->getStats()->setHyperBodyMp(skill->y);
		}
	}

	Vana::Timer::Id buffTimerId{TimerType::BuffTimer, static_cast<int32_t>(source.getType()), source.getId()};
	if (hasTimer) {
		// Get rid of timers/same buff if they currently exist
		m_player->getTimerContainer()->removeTimer(buffTimerId);
	}

	if (buff.anyActs()) {
		for (const auto &info : buff.getBuffInfo()) {
			if (!info.hasAct()) continue;
			Vana::Timer::Id actId{TimerType::SkillActTimer, info.getBitPosition()};
			m_player->getTimerContainer()->removeTimer(actId);
		}
	}

	for (size_t i = 0; i < m_buffs.size(); i++) {
		auto &existing = m_buffs[i];
		if (existing.type == source.getType() && existing.identifier == source.getId()) {
			m_buffs.erase(std::begin(m_buffs) + i);
			break;
		}
	}

	if (displaces) {
		// Displace bit positions
		// It is implicitly assumed in the buffs system that only one buff may "own" a particular bit position at once
		// Therefore, if you use Haste and then a Speed Potion, Haste will still apply to jump while the potion will apply to speed
		// This means that we should be keeping track of which bit positions are currently applicable to any given buff
		for (size_t i = 0; i < m_buffs.size(); i++) {
			auto &existing = m_buffs[i];
			const auto &existingBuffInfo = existing.raw.getBuffInfo();
			vector_t<uint8_t> displacedBits;
			for (const auto &existingInfo : existingBuffInfo) {
				for (const auto &info : buff.getBuffInfo()) {
					if (info == existingInfo) {
						// NOTE
						// This code assumes that there will not be two of a particular bit position allocated currently
						displacedBits.push_back(info.getBitPosition());
					}
				}
			}

			if (displacedBits.size() > 0) {
				vector_t<BuffInfo> applicable;
				vector_t<uint8_t> displacedActBitPositions;

				for (const auto &existingInfo : existingBuffInfo) {
					bool found = false;
					for (auto bit : displacedBits) {
						if (bit == existingInfo) {
							found = true;
							break;
						}
					}
					if (!found) {
						applicable.push_back(existingInfo);
					}
					else if (existingInfo.hasAct()) {
						displacedActBitPositions.push_back(existingInfo.getBitPosition());
					}
				}

				for (const auto &bit : displacedActBitPositions) {
					Vana::Timer::Id actId{TimerType::SkillActTimer, bit};
					m_player->getTimerContainer()->removeTimer(actId);
				}

				if (applicable.size() == 0) {
					Vana::Timer::Id id{TimerType::BuffTimer, static_cast<int32_t>(existing.type), existing.identifier};
					m_player->getTimerContainer()->removeTimer(id);

					m_buffs.erase(std::begin(m_buffs) + i);
					i--;
				}
				else {
					existing.raw = existing.raw.withBuffs(applicable);
				}
			}
		}
	}

	if (hasTimer) {
		Vana::Timer::Timer::create(
			[this, source](const time_point_t &now) {
				Skills::stopSkill(m_player, source, true);
			},
			buffTimerId,
			m_player->getTimerContainer(),
			time);

		if (buff.anyActs()) {
			for (const auto &info : buff.getBuffInfo()) {
				if (!info.hasAct()) continue;

				BuffRunAction runAct{source};
				runAct.player = m_player;
				runAct.act = info.getAct();
				runAct.value = Buffs::getValue(
					m_player,
					source,
					seconds_t{0},
					info.getBitPosition(),
					info.getActValue(),
					2).value;

				Vana::Timer::Id actId{TimerType::SkillActTimer, info.getBitPosition()};
				Vana::Timer::Timer::create(
					runAct,
					actId,
					m_player->getTimerContainer(),
					seconds_t{0},
					duration_cast<milliseconds_t>(info.getActInterval()));
			}
		}
	}

	LocalBuffInfo local;
	local.raw = buff;
	local.type = source.getType();
	local.identifier = source.getId();
	switch (source.getType()) {
		case BuffSourceType::Item:
		case BuffSourceType::Skill: local.level = source.getSkillLevel(); break;
		case BuffSourceType::MobSkill: local.level = source.getMobSkillLevel(); break;
		default: throw NotImplementedException{"BuffSourceType"};
	}

	m_buffs.push_back(local);

	m_player->sendMap(
		Packets::addBuff(
			m_player->getId(),
			translateToPacket(source),
			time,
			Buffs::convertToPacket(m_player, source, time, buff),
			0));

	return Result::Successful;
}

auto PlayerActiveBuffs::removeBuff(const BuffSource &source, const Buff &buff, bool fromTimer) -> void {
	if (!fromTimer) {
		Vana::Timer::Id id{TimerType::BuffTimer, static_cast<int32_t>(source.getType()), source.getId()};
		m_player->getTimerContainer()->removeTimer(id);
	}

	auto basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	size_t size = m_buffs.size();
	for (size_t i = 0; i < size; i++) {
		const auto &info = m_buffs[i];
		if (info.type == source.getType() && info.identifier == source.getId()) {
			m_player->sendMap(
				Packets::endBuff(
					m_player->getId(),
					Buffs::convertToPacketTypes(info.raw)));

			for (const auto &actInfo : info.raw.getBuffInfo()) {
				if (!actInfo.hasAct()) continue;
				Vana::Timer::Id actId{TimerType::SkillActTimer, actInfo.getBitPosition()};
				m_player->getTimerContainer()->removeTimer(actId);
			}

			for (const auto &buffInfo : info.raw.getBuffInfo()) {
				if (buffInfo == basics.mount) {
					m_mountItemId = 0;
				}
				else if (buffInfo == basics.energyCharge) {
					m_energyCharge = 0;
				}
				else if (buffInfo == basics.combo) {
					m_combo = 0;
				}
				else if (buffInfo == basics.zombify) {
					m_zombifyPotency = 0;
				}
				else if (buffInfo == basics.homingBeacon) {
					resetHomingBeaconMob();
				}
				else if (buffInfo == basics.mapleWarrior) {
					m_player->getStats()->setMapleWarrior(0);
				}
				else if (buffInfo == basics.hyperBodyHp) {
					m_player->getStats()->setHyperBodyHp(0);
				}
				else if (buffInfo == basics.hyperBodyMp) {
					m_player->getStats()->setHyperBodyMp(0);
				}
			}

			switch (info.type) {
				case BuffSourceType::MobSkill: {
					mob_skill_id_t skillId = source.getMobSkillId();
					int32_t maskBit = calculateDebuffMaskBit(skillId);
					m_debuffMask -= maskBit;
					break;
				}
			}

			m_buffs.erase(m_buffs.begin() + i);
			break;
		}
	}
}

auto PlayerActiveBuffs::removeBuffs() -> void {
	while (m_buffs.size() > 0) {
		LocalBuffInfo &buff = *std::begin(m_buffs);
		removeBuff(buff.toSource(), buff.raw);
	}
}

auto PlayerActiveBuffs::getBuffSecondsRemaining(BuffSourceType type, int32_t buffId) const -> seconds_t {
	Vana::Timer::Id id{TimerType::BuffTimer, static_cast<int32_t>(type), buffId};
	return m_player->getTimerContainer()->getRemainingTime<seconds_t>(id);
}

auto PlayerActiveBuffs::getBuffSecondsRemaining(const BuffSource &source) const -> seconds_t {
	return getBuffSecondsRemaining(source.getType(), source.getId());
}

// Debuffs
auto PlayerActiveBuffs::removeDebuff(mob_skill_id_t skillId) -> void {
	int32_t maskBit = calculateDebuffMaskBit(skillId);
	if ((m_debuffMask & maskBit) != 0) {
		Skills::stopSkill(
			m_player,
			BuffSource::fromMobSkill(
				skillId,
				getBuffLevel(BuffSourceType::MobSkill, skillId)),
			false);
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

auto PlayerActiveBuffs::getZombifiedPotency(int16_t basePotency) -> int16_t {
	if ((m_debuffMask & StatusEffects::Player::Zombify) != 0) {
		return static_cast<int16_t>(
			static_cast<int32_t>(basePotency)
			* m_zombifyPotency
			/ 100);
	}
	return basePotency;
}

auto PlayerActiveBuffs::usePlayerDispel() -> void {
	removeDebuff(MobSkills::Seal);
	removeDebuff(MobSkills::Slow);
	removeDebuff(MobSkills::Darkness);
	removeDebuff(MobSkills::Weakness);
	removeDebuff(MobSkills::Curse);
	removeDebuff(MobSkills::Poison);
}

auto PlayerActiveBuffs::calculateDebuffMaskBit(mob_skill_id_t skillId) const -> int32_t {
	switch (skillId) {
		case MobSkills::Seal: return StatusEffects::Player::Seal;
		case MobSkills::Darkness: return StatusEffects::Player::Darkness;
		case MobSkills::Weakness: return StatusEffects::Player::Weakness;
		case MobSkills::Stun: return StatusEffects::Player::Stun;
		case MobSkills::Curse: return StatusEffects::Player::Curse;
		case MobSkills::Poison: return StatusEffects::Player::Poison;
		case MobSkills::Slow: return StatusEffects::Player::Slow;
		case MobSkills::Seduce: return StatusEffects::Player::Seduce;
		case MobSkills::Zombify: return StatusEffects::Player::Zombify;
		case MobSkills::CrazySkull: return StatusEffects::Player::CrazySkull;
	}
	return 0;
}

auto PlayerActiveBuffs::getMapBuffValues() -> BuffPacketStructure {
	auto &buffProvider = ChannelServer::getInstance().getBuffDataProvider();
	auto &basics = buffProvider.getBuffsByEffect();
	BuffPacketStructure result;

	using tuple_type = tuple_t<uint8_t, const BuffInfo *, BuffSource>;
	vector_t<tuple_type> mapBuffs;

	for (const auto &buff : m_buffs) {
		for (const auto &info : buff.raw.getBuffInfo()) {
			if (info.hasMapInfo()) {
				mapBuffs.emplace_back(
					info.getBitPosition(),
					&info,
					buff.toSource());
			}
		}
	}

	std::sort(std::begin(mapBuffs), std::end(mapBuffs), [](const tuple_type &a, const tuple_type &b) -> bool {
		return std::get<0>(a) < std::get<0>(b);
	});

	for (const auto &tup : mapBuffs) {
		const auto &info = std::get<1>(tup);
		const auto &source = std::get<2>(tup);

		result.types[info->getBuffByte()] |= info->getBuffType();
		result.values.push_back(Buffs::getValue(
			m_player,
			source,
			getBuffSecondsRemaining(source),
			info->getBitPosition(),
			info->getMapInfo()));
	}

	return result;
}

// Active skill levels
auto PlayerActiveBuffs::getBuffLevel(BuffSourceType type, int32_t buffId) const -> skill_level_t {
	for (const auto &buff : m_buffs) {
		if (buff.type != type) continue;
		if (buff.identifier != buffId) continue;
		return static_cast<skill_level_t>(buff.level);
	}
	return 0;
}

auto PlayerActiveBuffs::getBuffSkillInfo(const BuffSource &source) const -> const SkillLevelInfo * const {
	if (source.getType() != BuffSourceType::Skill) throw std::invalid_argument{"source must be BuffSourceType::Skill"};
	return source.getSkillData(ChannelServer::getInstance().getSkillDataProvider());
}

auto PlayerActiveBuffs::stopSkill(const BuffSource &source) -> void {
	Skills::stopSkill(m_player, source);
}

// Buff addition/removal
auto PlayerActiveBuffs::dispelBuffs() -> void {
	if (m_player->hasGmBenefits()) {
		return;
	}

	vector_t<BuffSource> stopSkills;
	for (const auto &buff : m_buffs) {
		if (buff.type == BuffSourceType::Skill) {
			stopSkills.push_back(buff.toSource());
		}
	}

	for (const auto &skill : stopSkills) {
		Skills::stopSkill(m_player, skill);
	}
}

// Specific skill stuff
auto PlayerActiveBuffs::getBattleshipHp() const -> int32_t {
	return m_battleshipHp;
}

auto PlayerActiveBuffs::resetBattleshipHp() -> void {
	skill_level_t shipLevel = m_player->getSkills()->getSkillLevel(Vana::Skills::Corsair::Battleship);
	player_level_t playerLevel = m_player->getStats()->getLevel();
	m_battleshipHp = GameLogicUtilities::getBattleshipHp(shipLevel, playerLevel);
}

auto PlayerActiveBuffs::getHomingBeaconMob() const -> map_object_t {
	return m_markedMob;
}

auto PlayerActiveBuffs::resetHomingBeaconMob(map_object_t mapMobId) -> void {
	Map *map = m_player->getMap();
	if (m_markedMob != 0) {
		if (ref_ptr_t<Mob> mob = map->getMob(mapMobId)) {
			auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
			auto source = getBuffSource(basics.homingBeacon);
			auto &buffSource = source.get();

			mob->removeMarker(m_player);
			m_player->sendMap(
				Packets::endBuff(
					m_player->getId(),
					Buffs::convertToPacketTypes(
						Buffs::preprocessBuff(
							m_player,
							buffSource,
							seconds_t{0}))));
		}
	}
	m_markedMob = mapMobId;
	if (mapMobId != 0) {
		map->getMob(mapMobId)->addMarker(m_player);
	}
}

auto PlayerActiveBuffs::resetCombo() -> void {
	setCombo(0);
}

auto PlayerActiveBuffs::setCombo(uint8_t combo) -> void {
	m_combo = combo;

	auto source = getComboSource();
	auto &buffSource = source.get();
	seconds_t timeLeft = getBuffSecondsRemaining(buffSource);

	m_player->sendMap(
		Packets::addBuff(
			m_player->getId(),
			buffSource.getId(),
			timeLeft,
			Buffs::convertToPacket(
				m_player,
				buffSource,
				timeLeft,
				Buffs::preprocessBuff(m_player, buffSource, timeLeft)),
			0));
}

auto PlayerActiveBuffs::addCombo() -> void {
	auto source = getComboSource();
	if (source.is_initialized()) {
		auto &buffSource = source.get();
		skill_id_t advSkill = m_player->getSkills()->getAdvancedCombo();
		skill_level_t advCombo = m_player->getSkills()->getSkillLevel(advSkill);
		auto skill = ChannelServer::getInstance().getSkillDataProvider().getSkill(
			advCombo > 0 ? advSkill : buffSource.getSkillId(),
			advCombo > 0 ? advCombo : buffSource.getSkillLevel());

		int8_t maxCombo = static_cast<int8_t>(skill->x);
		if (m_combo == maxCombo) {
			return;
		}

		if (advCombo > 0 && Randomizer::percentage<uint16_t>() < skill->prop) {
			m_combo += 1;
		}
		m_combo += 1;
		if (m_combo > maxCombo) {
			m_combo = maxCombo;
		}

		setCombo(m_combo);
	}
}

auto PlayerActiveBuffs::getCombo() const -> uint8_t {
	return m_combo;
}

auto PlayerActiveBuffs::getBerserk() const -> bool {
	return m_berserk;
}

auto PlayerActiveBuffs::checkBerserk(bool display) -> void {
	if (m_player->getStats()->getJob() == Jobs::JobIds::DarkKnight) {
		// Berserk calculations
		skill_id_t skillId = Vana::Skills::DarkKnight::Berserk;
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
				m_player->sendMap(Packets::Skills::showBerserk(m_player->getId(), level, m_berserk));
			}
		}
	}
}

auto PlayerActiveBuffs::getEnergyChargeLevel() const -> int16_t {
	return m_energyCharge;
}

auto PlayerActiveBuffs::increaseEnergyChargeLevel(int8_t targets) -> void {
	if (m_energyCharge == Stats::MaxEnergyChargeLevel) {
		// Buff is currently engaged
		return;
	}

	if (targets > 0) {
		stopEnergyChargeTimer();

		skill_id_t skillId = m_player->getSkills()->getEnergyCharge();
		auto info = m_player->getSkills()->getSkillInfo(skillId);
		m_energyCharge += info->x * targets;
		m_energyCharge = std::min(m_energyCharge, Stats::MaxEnergyChargeLevel);

		if (m_energyCharge == Stats::MaxEnergyChargeLevel) {
			Buffs::addBuff(m_player, skillId, m_player->getSkills()->getSkillLevel(skillId), 0);
		}
		else {
			startEnergyChargeTimer();
			BuffSource source = BuffSource::fromSkill(skillId, info->level);
			Buff buff{{ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect().energyCharge}};
			m_player->send(
				Packets::addBuff(
					m_player->getId(),
					translateToPacket(source),
					seconds_t{0},
					Buffs::convertToPacket(m_player, source, seconds_t{0}, buff),
					0));
		}
	}
}

auto PlayerActiveBuffs::decreaseEnergyChargeLevel() -> void {
	m_energyCharge -= Stats::EnergyChargeDecay;
	m_energyCharge = std::max<int16_t>(m_energyCharge, 0);
	if (m_energyCharge > 0) {
		startEnergyChargeTimer();
	}

	skill_id_t skillId = m_player->getSkills()->getEnergyCharge();
	auto info = m_player->getSkills()->getSkillInfo(skillId);
	BuffSource source = BuffSource::fromSkill(skillId, info->level);
	Buff buff{{ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect().energyCharge}};
	m_player->send(
		Packets::addBuff(
			m_player->getId(),
			translateToPacket(source),
			seconds_t{0},
			Buffs::convertToPacket(m_player, source, seconds_t{0}, buff),
			0));
}

auto PlayerActiveBuffs::startEnergyChargeTimer() -> void {
	skill_id_t skillId = m_player->getSkills()->getEnergyCharge();
	m_energyChargeTimerCounter++;
	Vana::Timer::Id id{TimerType::EnergyChargeTimer, skillId, m_energyChargeTimerCounter};
	Vana::Timer::Timer::create(
		[this](const time_point_t &now) {
			this->decreaseEnergyChargeLevel();
		},
		id,
		m_player->getTimerContainer(),
		seconds_t{10});
}

auto PlayerActiveBuffs::stopEnergyChargeTimer() -> void {
	skill_id_t skillId = m_player->getSkills()->getEnergyCharge();
	Vana::Timer::Id id{TimerType::EnergyChargeTimer, skillId, m_energyChargeTimerCounter};
	m_player->getTimerContainer()->removeTimer(id);
}

auto PlayerActiveBuffs::stopBooster() -> void {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	auto source = getBuffSource(basics.booster);
	if (source.is_initialized()) {
		stopSkill(source.get());
	}
}

auto PlayerActiveBuffs::stopCharge() -> void {
	auto source = getChargeSource();
	if (source.is_initialized()) {
		stopSkill(source.get());
	}
}

auto PlayerActiveBuffs::stopBulletSkills() -> void {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();

	auto soulArrow = getBuffSource(basics.soulArrow);
	if (soulArrow.is_initialized()) {
		stopSkill(soulArrow.get());
	}

	auto shadowStars = getBuffSource(basics.shadowStars);
	if (shadowStars.is_initialized()) {
		stopSkill(shadowStars.get());
	}
}

auto PlayerActiveBuffs::hasBuff(BuffSourceType type, int32_t buffId) const -> bool {
	for (const auto &buff : m_buffs) {
		if (buff.type != type) continue;
		if (buff.identifier != buffId) continue;
		return true;
	}
	return false;
}

auto PlayerActiveBuffs::hasBuff(const BuffInfo &buff) const -> bool {
	return hasBuff(buff.getBitPosition());
}

auto PlayerActiveBuffs::hasBuff(uint8_t bitPosition) const -> bool {
	for (const auto &buff : m_buffs) {
		for (const auto &info : buff.raw.getBuffInfo()) {
			if (bitPosition == info) {
				return true;
			}
		}
	}
	return false;
}

auto PlayerActiveBuffs::getBuffSource(const BuffInfo &buff) const -> optional_t<BuffSource> {
	return getBuff(buff.getBitPosition());
}

auto PlayerActiveBuffs::getBuff(uint8_t bitPosition) const -> optional_t<BuffSource> {
	for (const auto &buff : m_buffs) {
		for (const auto &info : buff.raw.getBuffInfo()) {
			if (bitPosition == info) {
				return buff.toSource();
			}
		}
	}
	return optional_t<BuffSource>{};
}

auto PlayerActiveBuffs::hasIceCharge() const -> bool {
	auto source = getChargeSource();
	if (!source.is_initialized()) return false;
	auto &buffSource = source.get();
	if (buffSource.getType() != BuffSourceType::Skill) throw NotImplementedException{"Ice charge BuffSourceType"};
	skill_id_t skillId = buffSource.getSkillId();
	return
		skillId == Vana::Skills::WhiteKnight::BwIceCharge ||
		skillId == Vana::Skills::WhiteKnight::SwordIceCharge;
}

auto PlayerActiveBuffs::getPickpocketCounter() -> int32_t {
	return ++m_pickpocketCounter;
}

auto PlayerActiveBuffs::hasInfinity() const -> bool {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return hasBuff(basics.infinity);
}

auto PlayerActiveBuffs::isUsingGmHide() const -> bool {
	return hasBuff(BuffSourceType::Skill, Vana::Skills::SuperGm::Hide);
}

auto PlayerActiveBuffs::hasShadowPartner() const -> bool {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return hasBuff(basics.shadowPartner);
}

auto PlayerActiveBuffs::hasShadowStars() const -> bool {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return hasBuff(basics.shadowStars);
}

auto PlayerActiveBuffs::hasSoulArrow() const -> bool {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return hasBuff(basics.soulArrow);
}

auto PlayerActiveBuffs::hasHolyShield() const -> bool {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return hasBuff(basics.holyShield);
}

auto PlayerActiveBuffs::isCursed() const -> bool {
	return (m_debuffMask & StatusEffects::Player::Curse) > 0;
}

auto PlayerActiveBuffs::getHolySymbolRate() const -> int16_t {
	int16_t val = 0;
	auto source = getHolySymbolSource();
	if (source.is_initialized()) {
		auto &buffSource = source.get();
		if (buffSource.getType() != BuffSourceType::Skill) throw NotImplementedException{"Holy Symbol BuffSourceType"};
		val = getBuffSkillInfo(buffSource)->x;
	}
	return val;
}

auto PlayerActiveBuffs::getMagicGuardSource() const -> optional_t<BuffSource> {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return getBuffSource(basics.magicGuard);
}

auto PlayerActiveBuffs::getMesoGuardSource() const -> optional_t<BuffSource> {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return getBuffSource(basics.mesoGuard);
}

auto PlayerActiveBuffs::getMesoUpSource() const -> optional_t<BuffSource> {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return getBuffSource(basics.mesoUp);
}

auto PlayerActiveBuffs::getHomingBeaconSource() const -> optional_t<BuffSource> {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return getBuffSource(basics.homingBeacon);
}

auto PlayerActiveBuffs::getComboSource() const -> optional_t<BuffSource> {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return getBuffSource(basics.combo);
}

auto PlayerActiveBuffs::getChargeSource() const -> optional_t<BuffSource> {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return getBuffSource(basics.charge);
}

auto PlayerActiveBuffs::getDarkSightSource() const -> optional_t<BuffSource> {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	auto darkSight = getBuffSource(basics.darkSight);
	if (darkSight.is_initialized()) return darkSight;
	return getBuffSource(basics.windWalk);
}

auto PlayerActiveBuffs::getPickpocketSource() const -> optional_t<BuffSource> {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return getBuffSource(basics.pickpocket);
}

auto PlayerActiveBuffs::getHamstringSource() const -> optional_t<BuffSource> {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return getBuffSource(basics.hamstring);
}

auto PlayerActiveBuffs::getBlindSource() const -> optional_t<BuffSource> {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return getBuffSource(basics.blind);
}

auto PlayerActiveBuffs::getConcentrateSource() const -> optional_t<BuffSource> {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return getBuffSource(basics.concentrate);
}

auto PlayerActiveBuffs::getHolySymbolSource() const -> optional_t<BuffSource> {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return getBuffSource(basics.holySymbol);
}

auto PlayerActiveBuffs::getPowerStanceSource() const -> optional_t<BuffSource> {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	auto ret = getBuffSource(basics.powerStance);
	if (ret.is_initialized()) return ret;
	ret = getBuffSource(basics.energyCharge);
	return ret;
}

auto PlayerActiveBuffs::getHyperBodyHpSource() const -> optional_t<BuffSource> {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return getBuffSource(basics.hyperBodyHp);
}

auto PlayerActiveBuffs::getHyperBodyMpSource() const -> optional_t<BuffSource> {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	return getBuffSource(basics.hyperBodyMp);
}

auto PlayerActiveBuffs::getMountItemId() const -> item_id_t {
	return m_mountItemId;
}

auto PlayerActiveBuffs::endMorph() -> void {
	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	auto source = getBuffSource(basics.morph);
	if (source.is_initialized()) {
		stopSkill(source.get());
	}
}

auto PlayerActiveBuffs::swapWeapon() -> void {
	stopBooster();
	stopCharge();
	stopBulletSkills();
}

auto PlayerActiveBuffs::takeDamage(damage_t damage) -> void {
	if (damage <= 0) return;

	auto &basics = ChannelServer::getInstance().getBuffDataProvider().getBuffsByEffect();
	auto source = getBuffSource(basics.morph);
	if (source.is_initialized()) {
		auto &buffSource = source.get();
		if (buffSource.getType() == BuffSourceType::Item) {
			stopSkill(buffSource);
		}
	}

	auto battleshipLevel = getBuffLevel(BuffSourceType::Skill, Vana::Skills::Corsair::Battleship);
	if (battleshipLevel > 0) {
		m_battleshipHp -= damage / 10;
		auto source = BuffSource::fromSkill(Vana::Skills::Corsair::Battleship, battleshipLevel);

		if (m_battleshipHp <= 0) {
			m_battleshipHp = 0;
			seconds_t coolTime = getBuffSkillInfo(source)->coolTime;
			Skills::startCooldown(m_player, source.getSkillId(), coolTime);
			stopSkill(source);
		}
		else {
			Packets::addBuff(
				m_player->getId(),
				source.getSkillId(),
				seconds_t{0},
				Buffs::convertToPacket(
					m_player,
					source,
					seconds_t{0},
					Buffs::preprocessBuff(m_player, source, seconds_t{0})),
				0);
		}
	}
}

auto PlayerActiveBuffs::getTransferPacket() const -> PacketBuilder {
	PacketBuilder builder;
	builder
		.add<int8_t>(m_combo)
		.add<int16_t>(m_energyCharge)
		.add<int32_t>(m_battleshipHp)
		.add<int32_t>(m_debuffMask)
		.add<item_id_t>(m_mountItemId);

	// Current buff info (IDs, times, levels)
	builder.add<uint16_t>(static_cast<uint16_t>(m_buffs.size()));
	for (const auto &buff : m_buffs) {
		auto &raw = buff.raw;

		builder.add<BuffSourceType>(buff.type);
		builder.add<int32_t>(buff.identifier);
		switch (buff.type) {
			case BuffSourceType::Item:
			case BuffSourceType::Skill:
				builder.add<skill_level_t>(buff.level);
				break;
			case BuffSourceType::MobSkill:
				builder.add<mob_skill_level_t>(static_cast<mob_skill_level_t>(buff.level));
				break;
			default: throw NotImplementedException{"BuffSourceType"};
		}
		builder.add<seconds_t>(getBuffSecondsRemaining(buff.type, buff.identifier));

		auto &buffs = raw.getBuffInfo();
		builder.add<uint8_t>(static_cast<uint8_t>(buffs.size()));
		for (const auto &info : buffs) {
			builder.add<uint8_t>(info.getBitPosition());
		}
	}

	return builder;
}

auto PlayerActiveBuffs::parseTransferPacket(PacketReader &reader) -> void {
	// Map entry buff info
	m_combo = reader.get<uint8_t>();
	m_energyCharge = reader.get<int16_t>();
	m_battleshipHp = reader.get<int32_t>();
	m_debuffMask = reader.get<int32_t>();
	m_mountItemId = reader.get<item_id_t>();

	// Current player skill/item buff info
	size_t size = reader.get<uint16_t>();
	for (size_t i = 0; i < size; ++i) {
		BuffSourceType type = reader.get<BuffSourceType>();
		int32_t identifier = reader.get<int32_t>();
		int32_t level = 0;
		switch (type) {
			case BuffSourceType::Item:
			case BuffSourceType::Skill: {
				level = reader.get<skill_level_t>();
				break;
			}
			case BuffSourceType::MobSkill: {
				level = reader.get<mob_skill_level_t>();
				break;
			}
			default: throw NotImplementedException{"BuffSourceType"};
		}

		LocalBuffInfo buff;
		buff.type = type;
		buff.identifier = identifier;
		buff.level = level;

		seconds_t timeLeft = reader.get<seconds_t>();
		vector_t<uint8_t> validBits;
		uint8_t validBitSize = reader.get<uint8_t>();
		for (uint8_t i = 0; i < validBitSize; i++) {
			validBits.push_back(reader.get<uint8_t>());
		}

		BuffSource source = buff.toSource();
		int32_t packetSkillId = translateToPacket(source);
		buff.raw = Buffs::preprocessBuff(
			Buffs::preprocessBuff(
				m_player,
				source,
				timeLeft),
			validBits);

		m_buffs.push_back(buff);

		Vana::Timer::Id id{TimerType::BuffTimer, static_cast<int32_t>(buff.type), buff.identifier};
		Vana::Timer::Timer::create(
			[this, packetSkillId](const time_point_t &now) {
				Skills::stopSkill(m_player, translateToSource(packetSkillId), true);
			},
			id,
			m_player->getTimerContainer(),
			timeLeft);
	}

	if (m_energyCharge > 0 && m_energyCharge != Stats::MaxEnergyChargeLevel) {
		startEnergyChargeTimer();
	}

	auto hyperBodyHpSource = getHyperBodyHpSource();
	if (hyperBodyHpSource.is_initialized()) {
		auto skill = getBuffSkillInfo(hyperBodyHpSource.get());
		m_player->getStats()->setHyperBodyHp(skill->x);
	}
	auto hyperBodyMpSource = getHyperBodyMpSource();
	if (hyperBodyMpSource.is_initialized()) {
		auto skill = getBuffSkillInfo(hyperBodyMpSource.get());
		m_player->getStats()->setHyperBodyMp(skill->y);
	}
}

}
}