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
#pragma once

#include "Buffs.hpp"
#include "IPacket.hpp"
#include "Types.hpp"
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

class PacketBuilder;
class PacketReader;
class Player;
struct SkillLevelInfo;

namespace Timer {
	class Container;
}

struct MapEntryBuffs {
	MapEntryBuffs()
	{
		for (int8_t i = 0; i < BuffBytes::ByteQuantity; ++i) {
			types[i] = 0;
		}
	}
	
	item_id_t mountId = 0;
	skill_id_t mountSkill = 0;
	array_t<uint8_t, BuffBytes::ByteQuantity> types;
	hash_map_t<int8_t, hash_map_t<uint8_t, MapEntryVals>> values;
};

class PlayerActiveBuffs {
	NONCOPYABLE(PlayerActiveBuffs);
	NO_DEFAULT_CONSTRUCTOR(PlayerActiveBuffs);
public:
	PlayerActiveBuffs(Player *player) :
		m_player(player)
	{
	}

	// Buff handling
	auto addBuff(skill_id_t skill, const seconds_t &time) -> void;
	auto removeBuff(skill_id_t skill, bool fromTimer = false) -> void;
	auto removeBuffs() -> void;
	auto getBuffSecondsRemaining(skill_id_t skill) const -> seconds_t;
	auto dispelBuffs() -> void;

	// Buff info
	auto addBuffInfo(skill_id_t skillId, const vector_t<Buff> &buffs) -> void;
	auto setActiveSkillLevel(skill_id_t skillId, skill_level_t level) -> void;
	auto getActiveSkillLevel(skill_id_t skillId) const -> skill_level_t;
	auto removeBuffInfo(skill_id_t skillId, const vector_t<Buff> &buffs) -> ActiveBuff;
	auto getActiveSkillInfo(skill_id_t skillId) const -> const SkillLevelInfo * const;

	// Buff map info
	auto addMapEntryBuffInfo(const ActiveMapBuff &buff) -> void;
	auto deleteMapEntryBuffInfo(const ActiveMapBuff &buff) -> void;
	auto setMountInfo(skill_id_t skillId, item_id_t mountId) -> void;
	auto getMapEntryBuffs() -> MapEntryBuffs;

	// Skill actions
	auto addAction(skill_id_t skill, Action act, int16_t value, const milliseconds_t &time) -> void;
	auto getActTimer(skill_id_t skill) -> ref_ptr_t<Timer::Container>;
	auto removeAction(skill_id_t skill) -> void;

	// Debuffs
	auto addDebuff(mob_skill_id_t skill, mob_skill_level_t level) -> void;
	auto useDispel() -> void;
	auto useDebuffHealingItem(int32_t mask) -> void;
	auto removeDebuff(mob_skill_id_t skill, bool fromTimer = false) -> void;
	auto getDebuffMask() const -> int32_t { return m_debuffMask; }
	auto setDebuffMask(int32_t newMask) -> void { m_debuffMask = newMask; }

	// Combo Attack
	auto setCombo(uint8_t combo, bool sendPacket) -> void;
	auto addCombo() -> void;
	auto getCombo() const -> uint8_t { return m_combo; }

	// Berserk
	auto getBerserk() const -> bool { return m_berserk; }
	auto checkBerserk(bool display = false) -> void;

	// Energy Charge
	auto getEnergyChargeLevel() const -> int16_t { return m_energyCharge; }
	auto increaseEnergyChargeLevel(int8_t targets = 1) -> void;
	auto decreaseEnergyChargeLevel() -> void;
	auto setEnergyChargeLevel(int16_t chargeLevel, bool startTimer = false) -> void;
	auto resetEnergyChargeLevel() -> void;
	auto startEnergyChargeTimer() -> void;
	auto stopEnergyChargeTimer() -> void;

	// Boosters
	auto setBooster(skill_id_t skillId) -> void { m_activeBooster = skillId; }
	auto stopBooster() -> void;
	auto getBooster() const -> skill_id_t { return m_activeBooster; }

	// White Knight/Paladin charges
	auto setCharge(skill_id_t skillId) -> void { m_activeCharge = skillId; }
	auto stopCharge() -> void;
	auto hasIceCharge() const -> bool;
	auto getCharge() const -> skill_id_t { return m_activeCharge; }

	// Soul Arrow/Shadow Stars
	auto stopBulletSkills() -> void;

	// Pickpocket
	auto getPickpocketCounter() -> int32_t { return ++m_pickpocketCounter; }

	// Battleship
	auto getBattleshipHp() const -> int32_t { return m_battleshipHp; }
	auto setBattleshipHp(int32_t amount) -> void { m_battleshipHp = amount; }
	auto reduceBattleshipHp(uint16_t amount) -> void;
	auto resetBattleshipHp() -> void;

	// Homing Beacon
	auto getMarkedMonster() const -> map_object_t { return m_markedMob; }
	auto hasMarkedMonster() const -> bool { return m_markedMob != 0; }
	auto setMarkedMonster(map_object_t mapMobId) -> void { m_markedMob = mapMobId; }

	// Commonly referred to (de)buffs on the server end
	auto hasInfinity() -> bool;
	auto hasMesoUp() -> bool;
	auto hasHolySymbol() -> bool;
	auto hasPowerStance() -> bool;
	auto hasMagicGuard() -> bool;
	auto hasMesoGuard() -> bool;
	auto hasHyperBody() -> bool;
	auto hasHolyShield() -> bool;
	auto hasShadowPartner() -> bool;
	auto hasShadowStars() -> bool;
	auto hasSoulArrow() -> bool;
	auto isUsingGmHide() -> bool;
	auto isCursed() -> bool;
	auto isZombified() -> bool;
	auto getHolySymbolRate() -> int16_t;
	auto getHolySymbol() -> skill_id_t;
	auto getPowerStance() -> skill_id_t;
	auto getHyperBody() -> skill_id_t;
	auto getCurrentMorph() -> skill_id_t;
	auto getMagicGuard() -> skill_id_t;
	auto getMesoGuard() -> skill_id_t;
	auto getHomingBeacon() -> skill_id_t;

	auto endMorph() -> void;
	auto swapWeapon() -> void;

	auto getTransferPacket() const -> PacketBuilder;
	auto parseTransferPacket(PacketReader &reader) -> void;
private:
	auto hasBuff(skill_id_t skillId) -> bool;
	auto calculateDebuffMaskBit(mob_skill_id_t skill) -> int32_t;

	bool m_berserk = false;
	uint8_t m_combo = 0;
	int16_t m_energyCharge = 0;
	skill_id_t m_activeCharge = 0;
	skill_id_t m_activeBooster = 0;
	int32_t m_pickpocketCounter = 0;
	int32_t m_battleshipHp = 0;
	map_object_t m_markedMob = 0;
	uint32_t m_timeSeed = 0;
	uint32_t m_debuffMask = 0;
	Player *m_player = nullptr;
	MapEntryBuffs m_mapBuffs;
	queue_t<skill_id_t> m_buffs;
	hash_map_t<int8_t, hash_map_t<uint8_t, skill_id_t>> m_activeBuffsByType;
	hash_map_t<skill_id_t, skill_level_t> m_activeLevels;
	hash_map_t<skill_id_t, ref_ptr_t<Timer::Container>> m_skillActs;
};