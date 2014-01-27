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
#pragma once

#include "GameConstants.hpp"
#include "ItemConstants.hpp"
#include "JobConstants.hpp"
#include "Pos.hpp"
#include "Rect.hpp"
#include "SkillConstants.hpp"
#include <string>

namespace GameLogicUtilities {
	// Inventory
	inline auto getInventory(int32_t itemId) -> uint8_t { return static_cast<uint8_t>(itemId / 1000000); }
	inline auto getItemType(int32_t itemId) -> int32_t { return itemId / 10000; }
	inline auto getScrollType(int32_t itemId) -> int32_t { return (itemId % 10000) - (itemId % 100); }
	inline auto itemTypeToScrollType(int32_t itemId) -> int32_t { return (getItemType(itemId) % 100) * 100; }
	inline auto isArrow(int32_t itemId) -> bool { return getItemType(itemId) == Items::Types::ItemArrow; }
	inline auto isStar(int32_t itemId) -> bool { return getItemType(itemId) == Items::Types::ItemStar; }
	inline auto isBullet(int32_t itemId) -> bool { return getItemType(itemId) == Items::Types::ItemBullet; }
	inline auto isRechargeable(int32_t itemId) -> bool { return isBullet(itemId) || isStar(itemId); }
	inline auto isEquip(int32_t itemId) -> bool { return getInventory(itemId) == Inventories::EquipInventory; }
	inline auto isPet(int32_t itemId) -> bool {	return (itemId / 100 * 100) == 5000000; }
	inline auto isStackable(int32_t itemId) -> bool { return !(isRechargeable(itemId) || isEquip(itemId) || isPet(itemId)); }
	inline auto isOverall(int32_t itemId) -> bool { return getItemType(itemId) == Items::Types::ArmorOverall; }
	inline auto isTop(int32_t itemId) -> bool { return getItemType(itemId) == Items::Types::ArmorTop; }
	inline auto isBottom(int32_t itemId) -> bool { return getItemType(itemId) == Items::Types::ArmorBottom; }
	inline auto isShield(int32_t itemId) -> bool { return getItemType(itemId) == Items::Types::ArmorShield; }
	inline auto is2hWeapon(int32_t itemId) -> bool { return getItemType(itemId) / 10 == 14; }
	inline auto is1hWeapon(int32_t itemId) -> bool { return getItemType(itemId) / 10 == 13; }
	inline auto isBow(int32_t itemId) -> bool { return getItemType(itemId) == Items::Types::WeaponBow; }
	inline auto isCrossbow(int32_t itemId) -> bool { return getItemType(itemId) == Items::Types::WeaponCrossbow; }
	inline auto isSword(int32_t itemId) -> bool { return getItemType(itemId) == Items::Types::Weapon1hSword || getItemType(itemId) == Items::Types::Weapon2hSword; }
	inline auto isMace(int32_t itemId) -> bool { return getItemType(itemId) == Items::Types::Weapon1hMace || getItemType(itemId) == Items::Types::Weapon2hMace; }
	inline auto isMount(int32_t itemId) -> bool { return getItemType(itemId) == Items::Types::Mount; }
	inline auto isMedal(int32_t itemId) -> bool { return getItemType(itemId) == Items::Types::Medal; }
	inline auto isValidInventory(int8_t inv) -> bool { return inv > 0 && inv <= Inventories::InventoryCount; }
	inline auto isCashSlot(int16_t slot) -> bool { return abs(slot) > 100; }
	inline auto stripCashSlot(int16_t slot) -> int16_t { return static_cast<int16_t>(isCashSlot(slot) ? abs(slot) - 100 : abs(slot)); }
	inline auto isGmEquip(int32_t itemId) -> bool { return itemId == Items::GmBottom || itemId == Items::GmHat || itemId == Items::GmTop || itemId == Items::GmWeapon; }

	// Player
	inline auto getGenderId(const string_t &gender) -> int8_t { return static_cast<int8_t>(gender == "male" ? Gender::Male : (gender == "female" ? Gender::Female : (gender == "both" ? Gender::Both : -1))); }

	// Player skills
	inline auto isBeginnerSkill(int32_t skillId) -> bool { return (skillId / 1000000) == (skillId < 10000000 ? 0 : 10); }
	inline auto isFourthJobSkill(int32_t skillId) -> bool { return (skillId / 10000) % 10 == 2; }
	inline auto isPuppet(int32_t skillId) -> bool { return skillId == Skills::Sniper::Puppet || skillId == Skills::Ranger::Puppet || skillId == Skills::WindArcher::Puppet; }
	inline auto isSummon(int32_t skillId) -> bool { return isPuppet(skillId) || skillId == Skills::Priest::SummonDragon || skillId == Skills::Ranger::SilverHawk || skillId == Skills::Sniper::GoldenEagle || skillId == Skills::DarkKnight::Beholder || skillId == Skills::FpArchMage::Elquines || skillId == Skills::IlArchMage::Ifrit || skillId == Skills::BlazeWizard::Ifrit || skillId == Skills::Bishop::Bahamut || skillId == Skills::Bowmaster::Phoenix || skillId == Skills::Marksman::Frostprey || skillId == Skills::Outlaw::Octopus || skillId == Skills::Corsair::WrathOfTheOctopi || skillId == Skills::Outlaw::Gaviota || skillId == Skills::DawnWarrior::Soul || skillId == Skills::BlazeWizard::Flame || skillId == Skills::WindArcher::Storm || skillId == Skills::NightWalker::Darkness || skillId == Skills::ThunderBreaker::Lightning; }
	inline auto isDarkSight(int32_t skillId) -> bool { return skillId == Skills::Rogue::DarkSight || skillId == Skills::NightWalker::DarkSight; }
	inline auto skillMatchesJob(int32_t skillId, int16_t job) -> bool { return (skillId / 1000000 == job / 100) && (skillId / 10000 <= job); }
	inline auto itemSkillMatchesJob(int32_t skillId, int16_t job) -> bool { return (skillId / 10000) == job; }
	inline auto getMasteryDisplay(int8_t level) -> int8_t { return (level + 1) / 2; }
	inline auto getBattleshipHp(uint8_t shipLevel, uint8_t playerLevel) -> int32_t { return (4000 * shipLevel) + ((playerLevel - 120) * 2000); }

	// Mob skills
	inline auto isMobSkill(int32_t skillId) -> bool { return skillId >= 100 && skillId <= 200; }

	// Jobs
	inline auto isAdventurer(int16_t jobId) -> bool { return jobId != 128 && jobId != 256 && (jobId == Jobs::JobIds::Beginner || (jobId >= 100 && jobId <= 910)); }
	inline auto isCygnus(int16_t jobId) -> bool { return jobId != 1024 && jobId >= 1000 && jobId < 2000; }
	inline auto isLegend(int16_t jobId) -> bool { return jobId != 2048 && jobId >= 2000 && jobId < 3000; }
	inline auto isResistance(int16_t jobId) -> bool { return jobId >= 3000 && jobId < 4000; }
	inline auto isBeginnerJob(int16_t jobId) -> bool {
		for (const auto &job : Jobs::Beginners::Jobs) {
			if (jobId == job) {
				return true;
			}
		}
		return false;
	}
	inline auto getJobTrack(int16_t jobId) -> int16_t { return jobId / 100; }
	inline auto getJobLine(int16_t jobId) -> int16_t { return isAdventurer(jobId) ? (jobId / 100) : ((jobId / 100) % 10); }
	inline auto getJobProgression(int16_t jobId) -> int16_t {
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
	inline auto getMaxLevel(int16_t jobId) -> uint8_t { return isCygnus(jobId) ? Stats::CygnusLevels : Stats::PlayerLevels; }

	// Monster card
	inline auto isMonsterCard(int32_t itemId) -> bool { return getItemType(itemId) == Items::Types::ItemMonsterCard; }
	inline auto getCardShortId(int32_t cardId) -> int16_t { return cardId % 10000; }
	inline auto isSpecialCard(int32_t cardId) -> bool { return getCardShortId(cardId) >= 8000; }

	// Map
	inline auto getMapCluster(int32_t mapId) -> int8_t { return static_cast<int8_t>(mapId / 10000000); }

	// Party
	inline auto getPartyMember1(int8_t totalMembers) -> int8_t { return static_cast<int8_t>(totalMembers >= 1 ? (0x40 >> totalMembers) : 0xFF); }
	inline auto getPartyMember2(int8_t totalMembers) -> int8_t { return static_cast<int8_t>(totalMembers >= 2 ? (0x80 >> totalMembers) : 0xFF); }
	inline auto getPartyMember3(int8_t totalMembers) -> int8_t { return static_cast<int8_t>(totalMembers >= 3 ? (0x100 >> totalMembers) : 0xFF); }
	inline auto getPartyMember4(int8_t totalMembers) -> int8_t { return static_cast<int8_t>(totalMembers >= 4 ? (0x200 >> totalMembers) : 0xFF); }
	inline auto getPartyMember5(int8_t totalMembers) -> int8_t { return static_cast<int8_t>(totalMembers >= 5 ? (0x400 >> totalMembers) : 0xFF); }
	inline auto getPartyMember6(int8_t totalMembers) -> int8_t { return static_cast<int8_t>(totalMembers >= 6 ? (0x800 >> totalMembers) : 0xFF); }
}