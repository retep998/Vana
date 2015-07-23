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

#include "GameConstants.hpp"
#include "ItemConstants.hpp"
#include "JobConstants.hpp"
#include "Point.hpp"
#include "Rect.hpp"
#include "SkillConstants.hpp"
#include <algorithm>
#include <string>

namespace GameLogicUtilities {
	// Inventory
	inline auto getInventory(item_id_t itemId) -> inventory_t { return static_cast<inventory_t>(itemId / 1000000); }
	inline auto getItemType(item_id_t itemId) -> int32_t { return itemId / 10000; }
	inline auto getScrollType(item_id_t itemId) -> int32_t { return (itemId % 10000) - (itemId % 100); }
	inline auto itemTypeToScrollType(item_id_t itemId) -> int32_t { return (getItemType(itemId) % 100) * 100; }
	inline auto isArrow(item_id_t itemId) -> bool { return getItemType(itemId) == Items::Types::ItemArrow; }
	inline auto isStar(item_id_t itemId) -> bool { return getItemType(itemId) == Items::Types::ItemStar; }
	inline auto isBullet(item_id_t itemId) -> bool { return getItemType(itemId) == Items::Types::ItemBullet; }
	inline auto isProjectile(item_id_t itemId) -> bool { return isBullet(itemId) || isStar(itemId) || isArrow(itemId); }
	inline auto isRechargeable(item_id_t itemId) -> bool { return isBullet(itemId) || isStar(itemId); }
	inline auto isEquip(item_id_t itemId) -> bool { return getInventory(itemId) == Inventories::EquipInventory; }
	inline auto isPet(item_id_t itemId) -> bool {	return (itemId / 100 * 100) == 5000000; }
	inline auto isStackable(item_id_t itemId) -> bool { return !(isRechargeable(itemId) || isEquip(itemId) || isPet(itemId)); }
	inline auto isOverall(item_id_t itemId) -> bool { return getItemType(itemId) == Items::Types::ArmorOverall; }
	inline auto isTop(item_id_t itemId) -> bool { return getItemType(itemId) == Items::Types::ArmorTop; }
	inline auto isBottom(item_id_t itemId) -> bool { return getItemType(itemId) == Items::Types::ArmorBottom; }
	inline auto isShield(item_id_t itemId) -> bool { return getItemType(itemId) == Items::Types::ArmorShield; }
	inline auto is2hWeapon(item_id_t itemId) -> bool { return getItemType(itemId) / 10 == 14; }
	inline auto is1hWeapon(item_id_t itemId) -> bool { return getItemType(itemId) / 10 == 13; }
	inline auto isBow(item_id_t itemId) -> bool { return getItemType(itemId) == Items::Types::WeaponBow; }
	inline auto isCrossbow(item_id_t itemId) -> bool { return getItemType(itemId) == Items::Types::WeaponCrossbow; }
	inline auto isSword(item_id_t itemId) -> bool { return getItemType(itemId) == Items::Types::Weapon1hSword || getItemType(itemId) == Items::Types::Weapon2hSword; }
	inline auto isMace(item_id_t itemId) -> bool { return getItemType(itemId) == Items::Types::Weapon1hMace || getItemType(itemId) == Items::Types::Weapon2hMace; }
	inline auto isMount(item_id_t itemId) -> bool { return getItemType(itemId) == Items::Types::Mount; }
	inline auto isMedal(item_id_t itemId) -> bool { return getItemType(itemId) == Items::Types::Medal; }
	inline auto isValidInventory(inventory_t inv) -> bool { return inv > 0 && inv <= Inventories::InventoryCount; }
	inline auto isCashSlot(inventory_slot_t slot) -> bool { return abs(slot) > 100; }
	inline auto stripCashSlot(inventory_slot_t slot) -> inventory_slot_t { return static_cast<inventory_slot_t>(isCashSlot(slot) ? abs(slot) - 100 : abs(slot)); }
	inline auto isGmEquip(item_id_t itemId) -> bool { return itemId == Items::GmBottom || itemId == Items::GmHat || itemId == Items::GmTop || itemId == Items::GmWeapon; }

	// Player
	inline auto getGenderId(const string_t &gender) -> gender_id_t { return static_cast<gender_id_t>(gender == "male" ? Gender::Male : (gender == "female" ? Gender::Female : (gender == "both" ? Gender::Both : -1))); }

	// Player skills
	inline auto isBeginnerSkill(skill_id_t skillId) -> bool { return (skillId / 1000000) == (skillId < 10000000 ? 0 : 10); }
	inline auto isFourthJobSkill(skill_id_t skillId) -> bool { return (skillId / 10000) % 10 == 2; }
	inline auto isPuppet(skill_id_t skillId) -> bool { return skillId == Skills::Sniper::Puppet || skillId == Skills::Ranger::Puppet || skillId == Skills::WindArcher::Puppet; }
	inline auto isSummon(skill_id_t skillId) -> bool { return isPuppet(skillId) || skillId == Skills::Priest::SummonDragon || skillId == Skills::Ranger::SilverHawk || skillId == Skills::Sniper::GoldenEagle || skillId == Skills::DarkKnight::Beholder || skillId == Skills::FpArchMage::Elquines || skillId == Skills::IlArchMage::Ifrit || skillId == Skills::BlazeWizard::Ifrit || skillId == Skills::Bishop::Bahamut || skillId == Skills::Bowmaster::Phoenix || skillId == Skills::Marksman::Frostprey || skillId == Skills::Outlaw::Octopus || skillId == Skills::Corsair::WrathOfTheOctopi || skillId == Skills::Outlaw::Gaviota || skillId == Skills::DawnWarrior::Soul || skillId == Skills::BlazeWizard::Flame || skillId == Skills::WindArcher::Storm || skillId == Skills::NightWalker::Darkness || skillId == Skills::ThunderBreaker::Lightning; }
	inline auto isDarkSight(skill_id_t skillId) -> bool { return skillId == Skills::Rogue::DarkSight || skillId == Skills::NightWalker::DarkSight || skillId == Skills::WindArcher::WindWalk; }
	inline auto isBlessingOfTheFairy(skill_id_t skillId) -> bool { return skillId == Skills::Beginner::BlessingOfTheFairy || skillId == Skills::Noblesse::BlessingOfTheFairy; }
	inline auto skillMatchesJob(skill_id_t skillId, job_id_t job) -> bool { return (skillId / 1000000 == job / 100) && (skillId / 10000 <= job); }
	inline auto itemSkillMatchesJob(skill_id_t skillId, job_id_t job) -> bool { return (skillId / 10000) == job; }
	inline auto getMasteryDisplay(skill_level_t level) -> int8_t { return (level + 1) / 2; }
	inline auto getBattleshipHp(skill_level_t shipLevel, player_level_t playerLevel) -> int32_t { return std::max((2 * shipLevel + (playerLevel - 120)) * 200, 2 * shipLevel * 200); }

	// Mob skills
	inline auto isMobSkill(skill_id_t skillId) -> bool { return skillId >= 100 && skillId <= 200; }

	// Jobs
	inline auto isAdventurer(job_id_t jobId) -> bool { return jobId != 128 && jobId != 256 && (jobId == Jobs::JobIds::Beginner || (jobId >= 100 && jobId <= 910)); }
	inline auto isCygnus(job_id_t jobId) -> bool { return jobId != 1024 && jobId >= 1000 && jobId < 2000; }
	inline auto isLegend(job_id_t jobId) -> bool { return jobId != 2048 && jobId >= 2000 && jobId < 3000; }
	inline auto isResistance(job_id_t jobId) -> bool { return jobId >= 3000 && jobId < 4000; }
	inline auto getJobType(job_id_t jobId) -> Jobs::JobType {
		if (isAdventurer(jobId)) return Jobs::JobType::Adventurer;
		if (isCygnus(jobId)) return Jobs::JobType::Cygnus;
		if (isLegend(jobId)) return Jobs::JobType::Legend;
		if (isResistance(jobId)) return Jobs::JobType::Resistance;
		throw std::invalid_argument{"jobId must be a valid type"};
	}
	inline auto isBeginnerJob(job_id_t jobId) -> bool {
		for (const auto &job : Jobs::Beginners::Jobs) {
			if (jobId == job) {
				return true;
			}
		}
		return false;
	}
	inline auto getJobTrack(job_id_t jobId) -> int8_t { return jobId / 100; }
	inline auto getJobLine(job_id_t jobId) -> int8_t { return isAdventurer(jobId) ? (jobId / 100) : ((jobId / 100) % 10); }
	inline auto getJobProgression(job_id_t jobId) -> int8_t {
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
		return Jobs::JobProgressions::SecondJob + jobProgression;
	}
	inline auto getMaxLevel(job_id_t jobId) -> player_level_t { return isCygnus(jobId) ? Stats::CygnusLevels : Stats::PlayerLevels; }

	// Monster card
	inline auto isMonsterCard(item_id_t itemId) -> bool { return getItemType(itemId) == Items::Types::ItemMonsterCard; }
	inline auto getCardShortId(item_id_t cardId) -> int16_t { return cardId % 10000; }
	inline auto isSpecialCard(item_id_t cardId) -> bool { return getCardShortId(cardId) >= 8000; }

	// Map
	inline auto getMapCluster(map_id_t mapId) -> int8_t { return static_cast<int8_t>(mapId / 10000000); }

	// Party
	inline auto getPartyMember1(int8_t totalMembers) -> int8_t { return static_cast<int8_t>(totalMembers >= 1 ? (0x40 >> totalMembers) : 0xFF); }
	inline auto getPartyMember2(int8_t totalMembers) -> int8_t { return static_cast<int8_t>(totalMembers >= 2 ? (0x80 >> totalMembers) : 0xFF); }
	inline auto getPartyMember3(int8_t totalMembers) -> int8_t { return static_cast<int8_t>(totalMembers >= 3 ? (0x100 >> totalMembers) : 0xFF); }
	inline auto getPartyMember4(int8_t totalMembers) -> int8_t { return static_cast<int8_t>(totalMembers >= 4 ? (0x200 >> totalMembers) : 0xFF); }
	inline auto getPartyMember5(int8_t totalMembers) -> int8_t { return static_cast<int8_t>(totalMembers >= 5 ? (0x400 >> totalMembers) : 0xFF); }
	inline auto getPartyMember6(int8_t totalMembers) -> int8_t { return static_cast<int8_t>(totalMembers >= 6 ? (0x800 >> totalMembers) : 0xFF); }
}