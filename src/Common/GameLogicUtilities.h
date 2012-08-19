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
#pragma once

#include "GameConstants.h"
#include "ItemConstants.h"
#include "JobConstants.h"
#include "Pos.h"
#include "SkillConstants.h"
#include <string>

using std::string;

namespace GameLogicUtilities {
	// Inventory
	inline uint8_t getInventory(int32_t itemId) { return static_cast<uint8_t>(itemId / 1000000); }
	inline int32_t getItemType(int32_t itemId) { return (itemId / 10000); }
	inline int32_t getScrollType(int32_t itemId) { return ((itemId % 10000) - (itemId % 100)); }
	inline int32_t itemTypeToScrollType(int32_t itemId) { return ((getItemType(itemId) % 100) * 100); }
	inline bool isArrow(int32_t itemId) { return (getItemType(itemId) == Items::Types::ItemArrow); }
	inline bool isStar(int32_t itemId) { return (getItemType(itemId) == Items::Types::ItemStar); }
	inline bool isBullet(int32_t itemId) { return (getItemType(itemId) == Items::Types::ItemBullet); }
	inline bool isRechargeable(int32_t itemId) { return (isBullet(itemId) || isStar(itemId)); }
	inline bool isEquip(int32_t itemId) { return (getInventory(itemId) == Inventories::EquipInventory); }
	inline bool isPet(int32_t itemId) {	return ((itemId / 100 * 100) == 5000000); }
	inline bool isStackable(int32_t itemId) { return !(isRechargeable(itemId) || isEquip(itemId) || isPet(itemId)); }
	inline bool isOverall(int32_t itemId) { return (getItemType(itemId) == Items::Types::ArmorOverall); }
	inline bool isTop(int32_t itemId) { return (getItemType(itemId) == Items::Types::ArmorTop); }
	inline bool isBottom(int32_t itemId) { return (getItemType(itemId) == Items::Types::ArmorBottom); }
	inline bool isShield(int32_t itemId) { return (getItemType(itemId) == Items::Types::ArmorShield); }
	inline bool is2hWeapon(int32_t itemId) { return (getItemType(itemId) / 10 == 14); }
	inline bool is1hWeapon(int32_t itemId) { return (getItemType(itemId) / 10 == 13); }
	inline bool isBow(int32_t itemId) { return (getItemType(itemId) == Items::Types::WeaponBow); }
	inline bool isCrossbow(int32_t itemId) { return (getItemType(itemId) == Items::Types::WeaponCrossbow); }
	inline bool isSword(int32_t itemId) { return (getItemType(itemId) == Items::Types::Weapon1hSword || getItemType(itemId) == Items::Types::Weapon2hSword); }
	inline bool isMace(int32_t itemId) { return (getItemType(itemId) == Items::Types::Weapon1hMace || getItemType(itemId) == Items::Types::Weapon2hMace); }
	inline bool isMount(int32_t itemId) { return (getItemType(itemId) == Items::Types::Mount); }
	inline bool isMedal(int32_t itemId) { return (getItemType(itemId) == Items::Types::Medal); }
	inline bool isValidInventory(int8_t inv) { return (inv > 0 && inv <= Inventories::InventoryCount); }
	inline bool isCashSlot(int16_t slot) { return (abs(slot) > 100); }
	inline int16_t stripCashSlot(int16_t slot) { return (isCashSlot(slot) ? abs(slot) - 100 : abs(slot)); }

	// Player
	inline int8_t getGenderId(const string &gender) { return static_cast<int8_t>(gender == "male" ? Gender::Male : (gender == "female" ? Gender::Female : (gender == "both" ? Gender::Both : -1))); }

	// Player skills
	inline bool isBeginnerSkill(int32_t skillId) { return ((skillId / 1000000) == (skillId < 10000000 ? 0 : 10)); }
	inline bool isFourthJobSkill(int32_t skillId) { return ((skillId / 10000) % 10 == 2); }
	inline bool isPuppet(int32_t skillId) { return (skillId == Skills::Sniper::Puppet || skillId == Skills::Ranger::Puppet || skillId == Skills::WindArcher::Puppet); }
	inline bool isSummon(int32_t skillId) { return (isPuppet(skillId) || skillId == Skills::Priest::SummonDragon || skillId == Skills::Ranger::SilverHawk || skillId == Skills::Sniper::GoldenEagle || skillId == Skills::DarkKnight::Beholder || skillId == Skills::FpArchMage::Elquines || skillId == Skills::IlArchMage::Ifrit || skillId == Skills::BlazeWizard::Ifrit || skillId == Skills::Bishop::Bahamut || skillId == Skills::Bowmaster::Phoenix || skillId == Skills::Marksman::Frostprey || skillId == Skills::Outlaw::Octopus || skillId == Skills::Corsair::WrathOfTheOctopi || skillId == Skills::Outlaw::Gaviota || skillId == Skills::DawnWarrior::Soul || skillId == Skills::BlazeWizard::Flame || skillId == Skills::WindArcher::Storm || skillId == Skills::NightWalker::Darkness || skillId == Skills::ThunderBreaker::Lightning); }
	inline bool isInBox(const Pos &start, const Pos &lt, const Pos &rb, const Pos &test) { return ((test.y >= start.y + lt.y) && (test.y <= start.y + rb.y) && (test.x >= start.x + lt.x) && (test.x <= start.x + rb.x)); }
	inline bool isMaxDarkSight(int32_t skillId, uint8_t level) { return (skillId == Skills::Rogue::DarkSight && level == 20 || skillId == Skills::NightWalker::DarkSight && level == 10); }
	inline bool skillMatchesJob(int32_t skillId, int16_t job) { return ((skillId / 1000000 == job / 100) && (skillId / 10000 <= job)); }
	inline bool itemSkillMatchesJob(int32_t skillId, int16_t job) { return ((skillId / 10000) == job); }
	inline int8_t getMasteryDisplay(int8_t level) { return ((level + 1) / 2); }
	inline int32_t getBattleshipHp(uint8_t shipLevel, uint8_t playerLevel) { return ((4000 * shipLevel) + ((playerLevel - 120) * 2000)); }

	// Mob skills
	inline bool isMobSkill(int32_t skillId) { return (skillId >= 100 && skillId <= 200); }

	// Jobs
	inline bool isAdventurer(int16_t jobId) { return (jobId != 128 && jobId != 256 && (jobId == Jobs::JobIds::Beginner || (jobId >= 100 && jobId <= 910))); }
	inline bool isCygnus(int16_t jobId) { return (jobId != 1024 && jobId >= 1000 && jobId < 2000); }
	inline bool isLegend(int16_t jobId) { return (jobId != 2048 && jobId >= 2000 && jobId < 3000); }
	inline bool isResistance(int16_t jobId) { return (jobId >= 3000 && jobId < 4000); }
	inline bool isBeginnerJob(int16_t jobId) {
		for (int32_t i = 0; i < Jobs::Beginners::JobCount; ++i) {
			if (jobId == Jobs::Beginners::Jobs[i]) {
				return true;
			}
		}
		return false;
	}
	inline int16_t getJobTrack(int16_t jobId) { return (isAdventurer(jobId) ? (jobId / 10) : (jobId / 100)); }
	inline int16_t getJobLine(int16_t jobId) { return (isAdventurer(jobId) ? (jobId / 100) : ((jobId / 100) % 10)); }
	inline int16_t getJobProgression(int16_t jobId) {
		if (isBeginnerJob(jobId)) {
			return Jobs::JobProgressions::Beginner;
		}

		int16_t jobProgression = (jobId % 10);
		if (jobProgression == 0) {
			// Might be first job or second job
			if (getJobTrack(jobId) == 0) {
				return Jobs::JobProgressions::FirstJob;
			}
		}
		return Jobs::JobProgressions::SecondJob + (jobId % 10);
	}
	inline uint8_t getMaxLevel(int16_t jobId) { return (isCygnus(jobId) ? Stats::CygnusLevels : Stats::PlayerLevels); }

	// Monster card
	inline bool isMonsterCard(int32_t itemId) { return (getItemType(itemId) == Items::Types::ItemMonsterCard); }
	inline int16_t getCardShortId(int32_t cardId) { return (cardId % 10000); }
	inline bool isSpecialCard(int32_t cardId) { return (getCardShortId(cardId) >= 8000); }

	// Map
	inline int8_t getMapCluster(int32_t mapId) { return static_cast<int8_t>(mapId / 10000000); }

	// Party
	inline int8_t getPartyMember1(int8_t totalMembers) { return static_cast<int8_t>(totalMembers >= 1 ? (0x40 >> totalMembers) : 0xFF); }
	inline int8_t getPartyMember2(int8_t totalMembers) { return static_cast<int8_t>(totalMembers >= 2 ? (0x80 >> totalMembers) : 0xFF); }
	inline int8_t getPartyMember3(int8_t totalMembers) { return static_cast<int8_t>(totalMembers >= 3 ? (0x100 >> totalMembers) : 0xFF); }
	inline int8_t getPartyMember4(int8_t totalMembers) { return static_cast<int8_t>(totalMembers >= 4 ? (0x200 >> totalMembers) : 0xFF); }
	inline int8_t getPartyMember5(int8_t totalMembers) { return static_cast<int8_t>(totalMembers >= 5 ? (0x400 >> totalMembers) : 0xFF); }
	inline int8_t getPartyMember6(int8_t totalMembers) { return static_cast<int8_t>(totalMembers >= 6 ? (0x800 >> totalMembers) : 0xFF); }
}