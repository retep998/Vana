/*
Copyright (C) 2008-2009 Vana Development Team

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
#ifndef GAMELOGICUTILITIES_H
#define GAMELOGICUTILITIES_H

#include "GameConstants.h"
#include "Pos.h"

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
	inline bool isMount(int32_t itemid) { return (getItemType(itemid) == Mount); }

	// Player skills
	inline bool isBeginnerSkill(int32_t skillid) { return ((skillid / 1000000) == (skillid < 10000000 ? 0 : 10)); }
	inline bool isFourthJobSkill(int32_t skillid) { return ((skillid / 10000) % 10 == 2); }
	inline bool isPuppet(int32_t skillid) { return (skillid == Jobs::Sniper::Puppet || skillid == Jobs::Ranger::Puppet); }
	inline bool isSummon(int32_t skillid) { return (isPuppet(skillid) || skillid == Jobs::Priest::SummonDragon || skillid == Jobs::Ranger::SilverHawk || skillid == Jobs::Sniper::GoldenEagle || skillid == Jobs::DarkKnight::Beholder || skillid == Jobs::FPArchMage::Elquines || skillid == Jobs::ILArchMage::Ifrit || skillid == Jobs::Bishop::Bahamut || skillid == Jobs::Bowmaster::Phoenix || skillid == Jobs::Marksman::Frostprey || skillid == Jobs::Outlaw::Octopus || skillid == Jobs::Corsair::WrathOfTheOctopi || skillid == Jobs::Outlaw::Gaviota); }
	inline bool isInBox(const Pos &start, const Pos &lt, const Pos &rb, const Pos &test) { return ((test.y >= start.y + lt.y) && (test.y <= start.y + rb.y) && (test.x >= start.x + lt.x) && (test.x <= start.x + rb.x)); }
	inline bool isMaxDarkSight(int32_t skillid, uint8_t level) { return (skillid == Jobs::Rogue::DarkSight && level == 20); }
	inline bool skillMatchesJob(int32_t skillid, int16_t job) { return ((skillid / 1000000 == job / 100) && (skillid / 10000 <= job)); }
	inline int8_t getMasteryDisplay(int8_t level) { return ((level + 1) / 2); }

	// Mob skills
	inline bool isMobSkill(int32_t skillid) { return (skillid >= 100 && skillid <= 200); }

	// Jobs
	inline bool isRegularJob(int32_t job) { return (job == Jobs::JobIds::Beginner || (job >= 100 && job <= 910)); }
	inline bool isNonBitJob(int32_t job) { return (isRegularJob(job)); }
	inline bool isBeginnerJob(int16_t jobid) { return (jobid == 0); }
	inline int16_t getJobTrack(int16_t job) { return (job / 100); }

	// Monster card
	inline bool isMonsterCard(int32_t itemid) { return (getItemType(itemid) == ItemMonsterCard); }
	inline int16_t getCardShortId(int32_t cardid) { return (cardid % 10000); }
	inline bool isSpecialCard(int32_t cardid) { return (getCardShortId(cardid) >= 8000); }

	// Party
	inline int8_t getPartyMember1(int8_t totalmembers) { return (totalmembers >= 1 ? (0x40 >> totalmembers) : 0xFF); }
	inline int8_t getPartyMember2(int8_t totalmembers) { return (totalmembers >= 2 ? (0x80 >> totalmembers) : 0xFF); }
	inline int8_t getPartyMember3(int8_t totalmembers) { return (totalmembers >= 3 ? (0x100 >> totalmembers) : 0xFF); }
	inline int8_t getPartyMember4(int8_t totalmembers) { return (totalmembers >= 4 ? (0x200 >> totalmembers) : 0xFF); }
	inline int8_t getPartyMember5(int8_t totalmembers) { return (totalmembers >= 5 ? (0x400 >> totalmembers) : 0xFF); }
	inline int8_t getPartyMember6(int8_t totalmembers) { return (totalmembers >= 6 ? (0x800 >> totalmembers) : 0xFF); }
};

#endif