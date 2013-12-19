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
#pragma once

#include "Buffs.h"
#include "IPacket.h"
#include "noncopyable.hpp"
#include "Types.h"
#include <list>
#include <memory>
#include <vector>
#include <unordered_map>

using std::list;
using std::shared_ptr;
using std::unordered_map;
using std::vector;

class PacketCreator;
class PacketReader;
class Player;
struct SkillLevelInfo;

namespace Timer {
	class Container;
}

struct MapEntryBuffs {
	MapEntryBuffs() : mountId(0), mountSkill(0) {
		for (int8_t i = 0; i < BuffBytes::ByteQuantity; ++i) {
			types[i] = 0;
		}
	}
	std::array<uint8_t, BuffBytes::ByteQuantity> types;
	unordered_map<int8_t, unordered_map<uint8_t, MapEntryVals>> values;

	int32_t mountId;
	int32_t mountSkill;
};

typedef unordered_map<int8_t, unordered_map<uint8_t, int32_t>> ActiveBuffsByType; // Used to determine which buffs are affecting which bytes so they can be properly overwritten

class PlayerActiveBuffs : public IPacketSerializable, boost::noncopyable {
public:
	PlayerActiveBuffs(Player *player) :
		m_player(player),
		m_combo(0),
		m_energyCharge(0),
		m_activeCharge(0),
		m_activeBooster(0),
		m_pickpocketCounter(0),
		m_battleshipHp(0),
		m_debuffMask(0),
		m_markedMob(0),
		m_berserk(false)
		{ }

	// Buff handling
	void addBuff(int32_t skill, const seconds_t &time);
	void removeBuff(int32_t skill, bool fromTimer = false);
	void removeBuffs();
	seconds_t getBuffSecondsRemaining(int32_t skill) const;
	list<int32_t> getBuffs() const { return m_buffs; }
	void dispelBuffs();

	// Buff info
	void addBuffInfo(int32_t skillId, const vector<Buff> &buffs);
	void setActiveSkillLevel(int32_t skillId, uint8_t level);
	uint8_t getActiveSkillLevel(int32_t skillId) const;
	ActiveBuff removeBuffInfo(int32_t skillId, const vector<Buff> &buffs);
	ActiveBuffsByType getBuffTypes() const { return m_activeBuffsByType; }
	SkillLevelInfo * getActiveSkillInfo(int32_t skillId);

	// Buff map info
	void addMapEntryBuffInfo(const ActiveMapBuff &buff);
	void deleteMapEntryBuffInfo(const ActiveMapBuff &buff);
	void setMountInfo(int32_t skillId, int32_t mountId);
	MapEntryBuffs getMapEntryBuffs();

	// Skill actions
	void addAction(int32_t skill, Action act, int16_t value, const milliseconds_t &time);
	Timer::Container * getActTimer(int32_t skill);
	void removeAction(int32_t skill);

	// Debuffs
	void addDebuff(uint8_t skill, uint8_t level);
	void useDispel();
	void useDebuffHealingItem(int32_t mask);
	void removeDebuff(uint8_t skill, bool fromTimer = false);
	int32_t getDebuffMask() const { return m_debuffMask; }
	void setDebuffMask(int32_t newMask) { m_debuffMask = newMask; }

	// Combo Attack
	void setCombo(uint8_t combo, bool sendPacket);
	void addCombo();
	uint8_t getCombo() const { return m_combo; }

	// Berserk
	bool getBerserk() const { return m_berserk; }
	void checkBerserk(bool display = false);

	// Energy Charge
	int16_t getEnergyChargeLevel() const { return m_energyCharge; }
	void increaseEnergyChargeLevel(int8_t targets = 1);
	void decreaseEnergyChargeLevel();
	void setEnergyChargeLevel(int16_t chargeLevel, bool startTimer = false);
	void resetEnergyChargeLevel();
	void startEnergyChargeTimer();
	void stopEnergyChargeTimer();

	// Boosters
	void setBooster(int32_t skillId) { m_activeBooster = skillId; }
	void stopBooster();
	int32_t getBooster() const { return m_activeBooster; }

	// White Knight/Paladin charges
	void setCharge(int32_t skillId) { m_activeCharge = skillId; }
	void stopCharge();
	bool hasIceCharge() const;
	int32_t getCharge() const { return m_activeCharge; }

	// Soul Arrow/Shadow Stars
	void stopBulletSkills();

	// Pickpocket
	int32_t getPickpocketCounter() { return ++m_pickpocketCounter; }

	// Battleship
	int32_t getBattleshipHp() const { return m_battleshipHp; }
	void setBattleshipHp(int32_t amount) { m_battleshipHp = amount; }
	void reduceBattleshipHp(uint16_t amount);
	void resetBattleshipHp();

	// Homing Beacon
	int32_t getMarkedMonster() const { return m_markedMob; }
	bool hasMarkedMonster() const { return (m_markedMob != 0); }
	void setMarkedMonster(int32_t mapMobId) { m_markedMob = mapMobId; }

	// Commonly referred to (de)buffs on the server end
	bool hasInfinity();
	bool hasMesoUp();
	bool hasHolySymbol();
	bool hasPowerStance();
	bool hasMagicGuard();
	bool hasMesoGuard();
	bool hasHyperBody();
	bool hasHolyShield();
	bool hasShadowPartner();
	bool hasShadowStars();
	bool hasSoulArrow();
	bool isUsingGmHide();
	bool isCursed();
	bool isZombified();
	int16_t getHolySymbolRate();
	int32_t getHolySymbol();
	int32_t getPowerStance();
	int32_t getHyperBody();
	int32_t getCurrentMorph();
	int32_t getMagicGuard();
	int32_t getMesoGuard();
	int32_t getHomingBeacon();

	void endMorph();
	void swapWeapon();

	void write(PacketCreator &packet) const override;
	void read(PacketReader &packet) override;
private:
	bool hasBuff(int32_t skillId);

	Player *m_player;
	uint8_t m_combo;
	int16_t m_energyCharge;
	int32_t m_activeCharge;
	int32_t m_activeBooster;
	int32_t m_pickpocketCounter;
	int32_t m_battleshipHp;
	int32_t m_markedMob;
	uint32_t m_timeSeed;
	uint32_t m_debuffMask;
	bool m_berserk;
	list<int32_t> m_buffs;
	ActiveBuffsByType m_activeBuffsByType;
	MapEntryBuffs m_mapBuffs;
	unordered_map<int32_t, uint8_t> m_activeLevels;
	unordered_map<int32_t, shared_ptr<Timer::Container>> m_skillActs;

	int32_t calculateDebuffMaskBit(uint8_t skill);
};