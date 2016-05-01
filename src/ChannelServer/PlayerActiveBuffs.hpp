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

#include "Common/BuffInfo.hpp"
#include "Common/IPacket.hpp"
#include "Common/Types.hpp"
#include "ChannelServer/Buffs.hpp"
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

namespace Vana {
	class PacketBuilder;
	class PacketReader;
	namespace Timer {
		class Container;
	}
	struct SkillLevelInfo;

	namespace ChannelServer {
		class Player;

		class PlayerActiveBuffs {
			NONCOPYABLE(PlayerActiveBuffs);
			NO_DEFAULT_CONSTRUCTOR(PlayerActiveBuffs);
		public:
			PlayerActiveBuffs(Player *player);

			// Buff handling
			auto translateToSource(int32_t buffId) const -> BuffSource;
			auto addBuff(const BuffSource &source, const Buff &buff, const seconds_t &time) -> Result;
			auto removeBuff(const BuffSource &source, const Buff &buff, bool fromTimer = false) -> void;
			auto removeBuffs() -> void;
			auto dispelBuffs() -> void;

			// Buff info
			auto getBuffSkillInfo(const BuffSource &source) const -> const SkillLevelInfo * const;
			auto getMapBuffValues() -> BuffPacketStructure;

			// Debuffs
			auto usePlayerDispel() -> void;
			auto useDebuffHealingItem(int32_t mask) -> void;
			auto getZombifiedPotency(int16_t basePotency) -> int16_t;
			auto removeDebuff(mob_skill_id_t skillId) -> void;

			// Combo Attack
			auto resetCombo() -> void;
			auto addCombo() -> void;
			auto getCombo() const -> uint8_t;

			// Berserk
			auto getBerserk() const -> bool;
			auto checkBerserk(bool display = false) -> void;

			// Energy Charge
			auto getEnergyChargeLevel() const -> int16_t;
			auto increaseEnergyChargeLevel(int8_t targets = 1) -> void;

			// White Knight/Paladin charges
			auto stopCharge() -> void;
			auto hasIceCharge() const -> bool;

			// Pickpocket
			auto getPickpocketCounter() -> int32_t;

			// Battleship
			auto getBattleshipHp() const -> int32_t;
			auto resetBattleshipHp() -> void;

			// Homing Beacon
			auto getHomingBeaconMob() const -> map_object_t;
			auto resetHomingBeaconMob(map_object_t mapMobId = 0) -> void;

			// Commonly referred to (de)buffs on the server end
			auto hasInfinity() const -> bool;
			auto hasHolyShield() const -> bool;
			auto hasShadowPartner() const -> bool;
			auto hasShadowStars() const -> bool;
			auto hasSoulArrow() const -> bool;
			auto hasBuff(BuffSourceType type, int32_t buffId) const -> bool;
			auto isUsingGmHide() const -> bool;
			auto isCursed() const -> bool;
			auto getHolySymbolRate() const -> int16_t;
			auto getMountItemId() const -> item_id_t;
			auto getHolySymbolSource() const -> optional_t<BuffSource>;
			auto getPowerStanceSource() const -> optional_t<BuffSource>;
			auto getHyperBodyHpSource() const -> optional_t<BuffSource>;
			auto getHyperBodyMpSource() const -> optional_t<BuffSource>;
			auto getMagicGuardSource() const -> optional_t<BuffSource>;
			auto getMesoGuardSource() const -> optional_t<BuffSource>;
			auto getMesoUpSource() const -> optional_t<BuffSource>;
			auto getHomingBeaconSource() const -> optional_t<BuffSource>;
			auto getComboSource() const -> optional_t<BuffSource>;
			auto getChargeSource() const -> optional_t<BuffSource>;
			auto getDarkSightSource() const -> optional_t<BuffSource>;
			auto getPickpocketSource() const -> optional_t<BuffSource>;
			auto getHamstringSource() const -> optional_t<BuffSource>;
			auto getBlindSource() const -> optional_t<BuffSource>;
			auto getConcentrateSource() const -> optional_t<BuffSource>;
			auto getBuffSource(const BuffInfo &buff) const -> optional_t<BuffSource>;
			auto endMorph() -> void;
			auto swapWeapon() -> void;
			auto takeDamage(damage_t damage) -> void;

			auto getTransferPacket() const -> PacketBuilder;
			auto parseTransferPacket(PacketReader &reader) -> void;
		private:
			struct LocalBuffInfo {
				Buff raw;
				BuffSourceType type;
				int32_t identifier;
				int32_t level;

				auto toSource() const -> BuffSource;
			};

			auto translateToPacket(const BuffSource &source) const -> int32_t;
			auto hasBuff(const BuffInfo &buff) const -> bool;
			auto hasBuff(uint8_t bitPosition) const -> bool;
			auto getBuff(uint8_t bitPosition) const -> optional_t<BuffSource>;
			auto getBuffLevel(BuffSourceType type, int32_t buffId) const -> skill_level_t;
			auto getBuffSecondsRemaining(BuffSourceType type, int32_t buffId) const -> seconds_t;
			auto getBuffSecondsRemaining(const BuffSource &source) const -> seconds_t;
			auto calculateDebuffMaskBit(mob_skill_id_t skillId) const -> int32_t;
			auto decreaseEnergyChargeLevel() -> void;
			auto startEnergyChargeTimer() -> void;
			auto stopEnergyChargeTimer() -> void;
			auto stopBooster() -> void;
			auto stopBulletSkills() -> void;
			auto stopSkill(const BuffSource &source) -> void;
			auto setCombo(uint8_t combo) -> void;

			bool m_berserk = false;
			uint8_t m_combo = 0;
			uint8_t m_energyChargeTimerCounter = 0;
			int16_t m_energyCharge = 0;
			int16_t m_zombifyPotency = 0;
			item_id_t m_mountItemId = 0;
			int32_t m_pickpocketCounter = 0;
			int32_t m_battleshipHp = 0;
			map_object_t m_markedMob = 0;
			uint32_t m_debuffMask = 0;
			ref_ptr_t<Player> m_player = nullptr;
			vector_t<LocalBuffInfo> m_buffs;
		};
	}
}