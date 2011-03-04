/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "Pos.h"
#include <string>

using std::string;

namespace GameLogicUtilities {
	// Inventory
	inline uint8_t getInventory(int32_t itemid) { return static_cast<uint8_t>(itemid / 1000000); }
	inline int32_t getItemType(int32_t itemid) { return (itemid / 10000); }
	inline int32_t getScrollType(int32_t itemid) { return ((itemid % 10000) - (itemid % 100)); }
	inline int32_t itemTypeToScrollType(int32_t itemid) { return ((getItemType(itemid) % 100) * 100); }
	inline bool isArrow(int32_t itemid) { return (getItemType(itemid) == ItemArrow); }
	inline bool isStar(int32_t itemid) { return (getItemType(itemid) == ItemStar); }
	inline bool isBullet(int32_t itemid) { return (getItemType(itemid) == ItemBullet); }
	inline bool isRechargeable(int32_t itemid) { return (isBullet(itemid) || isStar(itemid)); }
	inline bool isEquip(int32_t itemid) { return (getInventory(itemid) == Inventories::EquipInventory); }
	inline bool isPet(int32_t itemid) {	return ((itemid / 100 * 100) == 5000000); }
	inline bool isOverall(int32_t itemid) { return (getItemType(itemid) == ArmorOverall); }
	inline bool isTop(int32_t itemid) { return (getItemType(itemid) == ArmorTop); }
	inline bool isBottom(int32_t itemid) { return (getItemType(itemid) == ArmorBottom); }
	inline bool isShield(int32_t itemid) { return (getItemType(itemid) == ArmorShield); }
	inline bool is2hWeapon(int32_t itemid) { return (getItemType(itemid) / 10 == 14); }
	inline bool is1hWeapon(int32_t itemid) { return (getItemType(itemid) / 10 == 13); }
	inline bool isBow(int32_t itemid) { return (getItemType(itemid) == WeaponBow); }
	inline bool isCrossbow(int32_t itemid) { return (getItemType(itemid) == WeaponCrossbow); }
	inline bool isSword(int32_t itemid) { return (getItemType(itemid) == Weapon1hSword || getItemType(itemid) == Weapon2hSword); }
	inline bool isMace(int32_t itemid) { return (getItemType(itemid) == Weapon1hMace || getItemType(itemid) == Weapon2hMace); }
	inline bool isMount(int32_t itemid) { return (getItemType(itemid) == Mount); }
	inline bool isMedal(int32_t itemid) { return (getItemType(itemid) == Medal); }
	inline bool isValidInventory(int8_t inv) { return (inv > 0 && inv <= Inventories::InventoryCount); }

	// Player
	inline int8_t getGenderId(const string &gender) { return static_cast<int8_t>(gender == "male" ? Gender::Male : (gender == "female" ? Gender::Female : (gender == "both" ? Gender::Both : -1))); }

	// Player skills
	inline bool isBeginnerSkill(int32_t skillId) { return ((skillId / 1000000) == (skillId < 10000000 ? 0 : 10)); }
	inline bool isFourthJobSkill(int32_t skillId) { return ((skillId / 10000) % 10 == 2); }
	inline bool isPuppet(int32_t skillId) { return (skillId == Jobs::Sniper::Puppet || skillId == Jobs::Ranger::Puppet || skillId == Jobs::WindArcher::Puppet); }
	inline bool isSummon(int32_t skillId) { return (isPuppet(skillId) || skillId == Jobs::Priest::SummonDragon || skillId == Jobs::Ranger::SilverHawk || skillId == Jobs::Sniper::GoldenEagle || skillId == Jobs::DarkKnight::Beholder || skillId == Jobs::FPArchMage::Elquines || skillId == Jobs::ILArchMage::Ifrit || skillId == Jobs::BlazeWizard::Ifrit || skillId == Jobs::Bishop::Bahamut || skillId == Jobs::Bowmaster::Phoenix || skillId == Jobs::Marksman::Frostprey || skillId == Jobs::Outlaw::Octopus || skillId == Jobs::Corsair::WrathOfTheOctopi || skillId == Jobs::Outlaw::Gaviota || skillId == Jobs::DawnWarrior::Soul || skillId == Jobs::BlazeWizard::Flame || skillId == Jobs::WindArcher::Storm || skillId == Jobs::NightWalker::Darkness || skillId == Jobs::ThunderBreaker::Lightning); }
	inline bool isInBox(const Pos &start, const Pos &lt, const Pos &rb, const Pos &test) { return ((test.y >= start.y + lt.y) && (test.y <= start.y + rb.y) && (test.x >= start.x + lt.x) && (test.x <= start.x + rb.x)); }
	inline bool isMaxDarkSight(int32_t skillId, uint8_t level) { return (skillId == Jobs::Rogue::DarkSight && level == 20 || skillId == Jobs::NightWalker::DarkSight && level == 10); }
	inline bool skillMatchesJob(int32_t skillId, int16_t job) { return ((skillId / 1000000 == job / 100) && (skillId / 10000 <= job)); }
	inline bool itemSkillMatchesJob(int32_t skillId, int16_t job) { return ((skillId / 10000) == job); }
	inline int8_t getMasteryDisplay(int8_t level) { return ((level + 1) / 2); }

	// Mob skills
	inline bool isMobSkill(int32_t skillId) { return (skillId >= 100 && skillId <= 200); }

	// Jobs
	inline bool isRegularJob(int32_t job) { return (job == Jobs::JobIds::Beginner || (job >= 100 && job <= 910)); }
	inline bool isCygnusJob(int32_t job) { return (job == Jobs::JobIds::Noblesse || job == Jobs::JobIds::DawnWarrior1 || job == Jobs::JobIds::DawnWarrior2 || job == Jobs::JobIds::DawnWarrior3 || job == Jobs::JobIds::BlazeWizard1 || job == Jobs::JobIds::BlazeWizard2 || job == Jobs::JobIds::BlazeWizard3 || job == Jobs::JobIds::WindArcher1 || job == Jobs::JobIds::WindArcher2 || job == Jobs::JobIds::WindArcher3 || job == Jobs::JobIds::NightWalker1 || job == Jobs::JobIds::NightWalker2 || job == Jobs::JobIds::NightWalker3 || job == Jobs::JobIds::ThunderBreaker1 || job == Jobs::JobIds::ThunderBreaker2 || job == Jobs::JobIds::ThunderBreaker3); }
	inline bool isNonBitJob(int32_t job) { return (isRegularJob(job) || isCygnusJob(job)); }
	inline bool isCygnus(int16_t jobid) { return (jobid >= 1000); }
	inline bool isBeginnerJob(int16_t jobid) { return (jobid == 0 || jobid == 1000); }
	inline int16_t getJobTrack(int16_t job, bool flattencygnus = false) { return (flattencygnus && isCygnus(job) ? ((job / 100) % 10) : (job / 100)); }

	// Monster card
	inline bool isMonsterCard(int32_t itemid) { return (getItemType(itemid) == ItemMonsterCard); }
	inline int16_t getCardShortId(int32_t cardid) { return (cardid % 10000); }
	inline bool isSpecialCard(int32_t cardid) { return (getCardShortId(cardid) >= 8000); }

	// Party
	inline int8_t getPartyMember1(int8_t totalmembers) { return static_cast<int8_t>(totalmembers >= 1 ? (0x40 >> totalmembers) : 0xFF); }
	inline int8_t getPartyMember2(int8_t totalmembers) { return static_cast<int8_t>(totalmembers >= 2 ? (0x80 >> totalmembers) : 0xFF); }
	inline int8_t getPartyMember3(int8_t totalmembers) { return static_cast<int8_t>(totalmembers >= 3 ? (0x100 >> totalmembers) : 0xFF); }
	inline int8_t getPartyMember4(int8_t totalmembers) { return static_cast<int8_t>(totalmembers >= 4 ? (0x200 >> totalmembers) : 0xFF); }
	inline int8_t getPartyMember5(int8_t totalmembers) { return static_cast<int8_t>(totalmembers >= 5 ? (0x400 >> totalmembers) : 0xFF); }
	inline int8_t getPartyMember6(int8_t totalmembers) { return static_cast<int8_t>(totalmembers >= 6 ? (0x800 >> totalmembers) : 0xFF); }
}
