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
	inline bool isPet(int32_t itemId) {	return ((itemId / 10000) == 500); }
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
	inline bool isPuppet(int32_t skillId) { return (skillId == Jobs::Sniper::Puppet || skillId == Jobs::Ranger::Puppet || skillId == Jobs::WindArcher::Puppet); }
	inline bool isSummon(int32_t skillId) { return (isPuppet(skillId) || skillId == Jobs::Priest::SummonDragon || skillId == Jobs::Ranger::SilverHawk || skillId == Jobs::Sniper::GoldenEagle || skillId == Jobs::DarkKnight::Beholder || skillId == Jobs::FpArchMage::Elquines || skillId == Jobs::IlArchMage::Ifrit || skillId == Jobs::BlazeWizard::Ifrit || skillId == Jobs::Bishop::Bahamut || skillId == Jobs::Bowmaster::Phoenix || skillId == Jobs::Marksman::Frostprey || skillId == Jobs::Outlaw::Octopus || skillId == Jobs::Corsair::WrathOfTheOctopi || skillId == Jobs::Outlaw::Gaviota || skillId == Jobs::DawnWarrior::Soul || skillId == Jobs::BlazeWizard::Flame || skillId == Jobs::WindArcher::Storm || skillId == Jobs::NightWalker::Darkness || skillId == Jobs::ThunderBreaker::Lightning); }
	inline bool isInBox(const Pos &start, const Pos &lt, const Pos &rb, const Pos &test) { return ((test.y >= start.y + lt.y) && (test.y <= start.y + rb.y) && (test.x >= start.x + lt.x) && (test.x <= start.x + rb.x)); }
	inline bool isMaxDarkSight(int32_t skillId, uint8_t level) { return (skillId == Jobs::Rogue::DarkSight && level == 20 || skillId == Jobs::NightWalker::DarkSight && level == 10); }
	inline bool skillMatchesJob(int32_t skillId, int16_t job) { return ((skillId / 1000000 == job / 100) && (skillId / 10000 <= job)); }
	inline bool itemSkillMatchesJob(int32_t skillId, int16_t job) { return ((skillId / 10000) == job); }
	inline int8_t getMasteryDisplay(int8_t level) { return ((level + 1) / 2); }
	inline int32_t getBattleshipHp(uint8_t shipLevel, uint8_t playerLevel) { return ((4000 * shipLevel) + ((playerLevel - 120) * 2000)); }

	// Mob skills
	inline bool isMobSkill(int32_t skillId) { return (skillId >= 100 && skillId <= 200); }

	// Jobs
	inline bool isRegularJob(int32_t job) { return (job == Jobs::JobIds::Beginner || (job >= 100 && job <= 910)); }
	inline bool isCygnusJob(int32_t job) { return (job == Jobs::JobIds::Noblesse || job == Jobs::JobIds::DawnWarrior1 || job == Jobs::JobIds::DawnWarrior2 || job == Jobs::JobIds::DawnWarrior3 || job == Jobs::JobIds::BlazeWizard1 || job == Jobs::JobIds::BlazeWizard2 || job == Jobs::JobIds::BlazeWizard3 || job == Jobs::JobIds::WindArcher1 || job == Jobs::JobIds::WindArcher2 || job == Jobs::JobIds::WindArcher3 || job == Jobs::JobIds::NightWalker1 || job == Jobs::JobIds::NightWalker2 || job == Jobs::JobIds::NightWalker3 || job == Jobs::JobIds::ThunderBreaker1 || job == Jobs::JobIds::ThunderBreaker2 || job == Jobs::JobIds::ThunderBreaker3); }
	inline bool isLegendJob(int32_t job) { return (job == Jobs::JobIds::Legend || Jobs::JobIds::Aran1 || Jobs::JobIds::Aran2 || Jobs::JobIds::Aran3 || Jobs::JobIds::Aran4 || job == Jobs::JobIds::Evan || job == Jobs::JobIds::Evan1 || job == Jobs::JobIds::Evan2 || job == Jobs::JobIds::Evan3 || job == Jobs::JobIds::Evan4 || job == Jobs::JobIds::Evan5 || job == Jobs::JobIds::Evan6 || job == Jobs::JobIds::Evan7 || job == Jobs::JobIds::Evan8 || job == Jobs::JobIds::Evan9 || job == Jobs::JobIds::Evan10); }
	inline bool isResistanceJob(int32_t job) { return (job == Jobs::JobIds::Citizen || Jobs::JobIds::BattleMage1 || Jobs::JobIds::BattleMage2 || Jobs::JobIds::BattleMage3 || Jobs::JobIds::BattleMage4 || Jobs::JobIds::WildHunter1 || Jobs::JobIds::WildHunter2 || Jobs::JobIds::WildHunter3 || Jobs::JobIds::WildHunter4 || Jobs::JobIds::Mechanic1 || Jobs::JobIds::Mechanic2 || Jobs::JobIds::Mechanic3 || Jobs::JobIds::Mechanic4); }
	inline bool isNonBitJob(int32_t job) { return (isRegularJob(job) || isCygnusJob(job) || isLegendJob(job) || isResistanceJob(job)); }
	inline bool isCygnus(int16_t jobId) { return (jobId >= 1000 && jobId < 2000); }
	inline bool isLegend(int16_t jobId) { return (jobId >= 2000 && jobId < 3000); }
	inline bool isResistance(int16_t jobId) { return (jobId >= 3000 && jobId < 4000); }
	inline bool isBeginnerJob(int16_t jobId) { return (jobId == Jobs::JobIds::Beginner || jobId == Jobs::JobIds::Noblesse || jobId == Jobs::JobIds::Legend || jobId == Jobs::JobIds::Evan || jobId == Jobs::JobIds::Citizen); }
	inline int16_t getJobTrack(int16_t job, bool flatten = false) { return (flatten && !isRegularJob(job) ? ((job / 100) % 10) : (job / 100)); }
	inline uint8_t getMaxLevel(int16_t job) { return (isCygnus(job) ? Stats::CygnusLevels : Stats::PlayerLevels); }
	inline bool isWildHunter(int16_t job) { return job / 100 == Jobs::JobTracks::WildHunter; }
	inline bool isExtendedSpJob(int16_t job) { return job / 1000 == Jobs::JobTracks::LegendJobTrack || job / 100 == Jobs::JobTracks::Evan || job == Jobs::JobIds::Evan || job == Jobs::JobIds::Mercedes || job / 100 == Jobs::JobTracks::Mercedes; }

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