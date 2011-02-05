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
#include "ItemConstants.h"
#include "SkillConstants.h"
#include "Pos.h"
#include <string>

using std::string;

namespace GameLogicUtilities {
	// Inventory
	inline uint8_t getInventory(int32_t itemid) { return static_cast<uint8_t>(itemid / 1000000); }
	inline int32_t getItemType(int32_t itemid) { return (itemid / 10000); }
	inline int32_t getScrollType(int32_t itemid) { return ((itemid % 10000) - (itemid % 100)); }
	inline int32_t itemTypeToScrollType(int32_t itemid) { return ((getItemType(itemid) % 100) * 100); }
	inline bool isArrow(int32_t itemid) { return (getItemType(itemid) == Items::Types::ItemArrow); }
	inline bool isStar(int32_t itemid) { return (getItemType(itemid) == Items::Types::ItemStar); }
	inline bool isBullet(int32_t itemid) { return (getItemType(itemid) == Items::Types::ItemBullet); }
	inline bool isRechargeable(int32_t itemid) { return (isBullet(itemid) || isStar(itemid)); }
	inline bool isEquip(int32_t itemid) { return (getInventory(itemid) == Inventories::EquipInventory); }
	inline bool isPet(int32_t itemid) {	return ((itemid / 100 * 100) == 5000000); }
	inline bool isStackable(int32_t itemid) { return !(isRechargeable(itemid) || isEquip(itemid) || isPet(itemid)); }
	inline bool isOverall(int32_t itemid) { return (getItemType(itemid) == Items::Types::ArmorOverall); }
	inline bool isTop(int32_t itemid) { return (getItemType(itemid) == Items::Types::ArmorTop); }
	inline bool isBottom(int32_t itemid) { return (getItemType(itemid) == Items::Types::ArmorBottom); }
	inline bool isShield(int32_t itemid) { return (getItemType(itemid) == Items::Types::ArmorShield); }
	inline bool is2hWeapon(int32_t itemid) { return (getItemType(itemid) / 10 == 14); }
	inline bool is1hWeapon(int32_t itemid) { return (getItemType(itemid) / 10 == 13); }
	inline bool isBow(int32_t itemid) { return (getItemType(itemid) == Items::Types::WeaponBow); }
	inline bool isCrossbow(int32_t itemid) { return (getItemType(itemid) == Items::Types::WeaponCrossbow); }
	inline bool isSword(int32_t itemid) { return (getItemType(itemid) == Items::Types::Weapon1hSword || getItemType(itemid) == Items::Types::Weapon2hSword); }
	inline bool isMace(int32_t itemid) { return (getItemType(itemid) == Items::Types::Weapon1hMace || getItemType(itemid) == Items::Types::Weapon2hMace); }
	inline bool isMount(int32_t itemid) { return (getItemType(itemid) == Items::Types::Mount); }
	inline bool isMedal(int32_t itemid) { return (getItemType(itemid) == Items::Types::Medal); }
	inline bool isValidInventory(int8_t inv) { return (inv > 0 && inv <= Inventories::InventoryCount); }

	// Player
	inline int8_t getGenderId(const string &gender) { return static_cast<int8_t>(gender == "male" ? Gender::Male : (gender == "female" ? Gender::Female : (gender == "both" ? Gender::Both : -1))); }

	// Player skills
	inline bool isBeginnerSkill(int32_t skillid) { return ((skillid / 1000000) == (skillid < 10000000 ? 0 : 10)); }
	inline bool isFourthJobSkill(int32_t skillid) { return ((skillid / 10000) % 10 == 2); }
	inline bool isPuppet(int32_t skillid) { return (skillid == Jobs::Sniper::Puppet || skillid == Jobs::Ranger::Puppet || skillid == Jobs::WindArcher::Puppet); }
	inline bool isSummon(int32_t skillid) { return (isPuppet(skillid) || skillid == Jobs::Priest::SummonDragon || skillid == Jobs::Ranger::SilverHawk || skillid == Jobs::Sniper::GoldenEagle || skillid == Jobs::DarkKnight::Beholder || skillid == Jobs::FPArchMage::Elquines || skillid == Jobs::ILArchMage::Ifrit || skillid == Jobs::BlazeWizard::Ifrit || skillid == Jobs::Bishop::Bahamut || skillid == Jobs::Bowmaster::Phoenix || skillid == Jobs::Marksman::Frostprey || skillid == Jobs::Outlaw::Octopus || skillid == Jobs::Corsair::WrathOfTheOctopi || skillid == Jobs::Outlaw::Gaviota || skillid == Jobs::DawnWarrior::Soul || skillid == Jobs::BlazeWizard::Flame || skillid == Jobs::WindArcher::Storm || skillid == Jobs::NightWalker::Darkness || skillid == Jobs::ThunderBreaker::Lightning); }
	inline bool isInBox(const Pos &start, const Pos &lt, const Pos &rb, const Pos &test) { return ((test.y >= start.y + lt.y) && (test.y <= start.y + rb.y) && (test.x >= start.x + lt.x) && (test.x <= start.x + rb.x)); }
	inline bool isMaxDarkSight(int32_t skillid, uint8_t level) { return (skillid == Jobs::Rogue::DarkSight && level == 20 || skillid == Jobs::NightWalker::DarkSight && level == 10); }
	inline bool skillMatchesJob(int32_t skillid, int16_t job) { return ((skillid / 1000000 == job / 100) && (skillid / 10000 <= job)); }
	inline bool itemSkillMatchesJob(int32_t skillid, int16_t job) { return ((skillid / 10000) == job); }
	inline int8_t getMasteryDisplay(int8_t level) { return ((level + 1) / 2); }

	// Mob skills
	inline bool isMobSkill(int32_t skillid) { return (skillid >= 100 && skillid <= 200); }

	// Jobs
	inline bool isRegularJob(int32_t job) { return (job == Jobs::JobIds::Beginner || (job >= 100 && job <= 910)); }
	inline bool isCygnusJob(int32_t job) { return (job == Jobs::JobIds::Noblesse || job == Jobs::JobIds::DawnWarrior1 || job == Jobs::JobIds::DawnWarrior2 || job == Jobs::JobIds::DawnWarrior3 || job == Jobs::JobIds::BlazeWizard1 || job == Jobs::JobIds::BlazeWizard2 || job == Jobs::JobIds::BlazeWizard3 || job == Jobs::JobIds::WindArcher1 || job == Jobs::JobIds::WindArcher2 || job == Jobs::JobIds::WindArcher3 || job == Jobs::JobIds::NightWalker1 || job == Jobs::JobIds::NightWalker2 || job == Jobs::JobIds::NightWalker3 || job == Jobs::JobIds::ThunderBreaker1 || job == Jobs::JobIds::ThunderBreaker2 || job == Jobs::JobIds::ThunderBreaker3); }
	inline bool isLegendJob(int32_t job) { return (job == Jobs::JobIds::Legend || Jobs::JobIds::Aran1 || Jobs::JobIds::Aran2 || Jobs::JobIds::Aran3 || Jobs::JobIds::Aran4 || job == Jobs::JobIds::Evan1 || job == Jobs::JobIds::Evan2 || job == Jobs::JobIds::Evan3 || job == Jobs::JobIds::Evan4 || job == Jobs::JobIds::Evan5 || job == Jobs::JobIds::Evan6 || job == Jobs::JobIds::Evan7 || job == Jobs::JobIds::Evan8 || job == Jobs::JobIds::Evan9 || job == Jobs::JobIds::Evan10); }
	inline bool isNonBitJob(int32_t job) { return (isRegularJob(job) || isCygnusJob(job) || isLegendJob(job)); }
	inline bool isCygnus(int16_t jobid) { return (jobid >= 1000 && jobid < 2000); }
	inline bool isLegend(int16_t jobid) { return (jobid >= 2000 && jobid < 3000); }
	inline bool isBeginnerJob(int16_t jobid) { return (jobid == Jobs::JobIds::Beginner || jobid == Jobs::JobIds::Noblesse || jobid == Jobs::JobIds::Legend); }
	inline int16_t getJobTrack(int16_t job, bool flatten = false) { return (flatten && !isRegularJob(job) ? ((job / 100) % 10) : (job / 100)); }

	// Monster card
	inline bool isMonsterCard(int32_t itemid) { return (getItemType(itemid) == Items::Types::ItemMonsterCard); }
	inline int16_t getCardShortId(int32_t cardid) { return (cardid % 10000); }
	inline bool isSpecialCard(int32_t cardid) { return (getCardShortId(cardid) >= 8000); }

	// Party
	inline int8_t getPartyMember1(int8_t totalmembers) { return static_cast<int8_t>(totalmembers >= 1 ? (0x40 >> totalmembers) : 0xFF); }
	inline int8_t getPartyMember2(int8_t totalmembers) { return static_cast<int8_t>(totalmembers >= 2 ? (0x80 >> totalmembers) : 0xFF); }
	inline int8_t getPartyMember3(int8_t totalmembers) { return static_cast<int8_t>(totalmembers >= 3 ? (0x100 >> totalmembers) : 0xFF); }
	inline int8_t getPartyMember4(int8_t totalmembers) { return static_cast<int8_t>(totalmembers >= 4 ? (0x200 >> totalmembers) : 0xFF); }
	inline int8_t getPartyMember5(int8_t totalmembers) { return static_cast<int8_t>(totalmembers >= 5 ? (0x400 >> totalmembers) : 0xFF); }
	inline int8_t getPartyMember6(int8_t totalmembers) { return static_cast<int8_t>(totalmembers >= 6 ? (0x800 >> totalmembers) : 0xFF); }

	// NPC
	// Temp solution for checking if an NPC is an imitator...
	inline bool isImitatorNpcId(int32_t id) { return (id >= 9901000); } // Every NPC ID that is above or equal to is an imitator.
};
