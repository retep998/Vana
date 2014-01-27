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
#include "EquipDataProvider.h"
#include "Database.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "Randomizer.h"
#include "StringUtilities.h"
#include <iomanip>
#include <iostream>
#include <string>

auto EquipDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Equips... ";

	loadEquips();

	std::cout << "DONE" << std::endl;
}

auto EquipDataProvider::loadEquips() -> void {
	m_equipInfo.clear();
	int32_t itemId;
	EquipInfo equip;
	string_t flags;
	// Ugly hack to get the integers instead of scientific notation
	// Note: This is MySQL's crappy behavior
	// It displays scientific notation for only very large values, meaning it's wildly inconsistent and hard to parse
	// We just use the string and send it to a translation function
	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT *, REPLACE(FORMAT(equip_slots + 0, 0), \",\", \"\") AS equip_slot_flags FROM item_equip_data");

	for (const auto &row : rs) {
		equip = EquipInfo();

		itemId = row.get<int32_t>("itemid");
		equip.attackSpeed = row.get<int8_t>("attack_speed");
		equip.healing = row.get<int8_t>("heal_hp");
		equip.slots = row.get<int8_t>("scroll_slots");
		equip.ihp = row.get<int16_t>("hp");
		equip.imp = row.get<int16_t>("mp");
		equip.reqStr = row.get<int16_t>("req_str");
		equip.reqDex = row.get<int16_t>("req_dex");
		equip.reqInt = row.get<int16_t>("req_int");
		equip.reqLuk = row.get<int16_t>("req_luk");
		equip.reqFame = row.get<int16_t>("req_fame");
		equip.istr = row.get<int16_t>("strength");
		equip.idex = row.get<int16_t>("dexterity");
		equip.iint = row.get<int16_t>("intelligence");
		equip.iluk = row.get<int16_t>("luck");
		equip.ihand = row.get<int16_t>("hands");
		equip.iwAtk = row.get<int16_t>("weapon_attack");
		equip.iwDef = row.get<int16_t>("weapon_defense");
		equip.imAtk = row.get<int16_t>("magic_attack");
		equip.imDef = row.get<int16_t>("magic_defense");
		equip.iacc = row.get<int16_t>("accuracy");
		equip.iavo = row.get<int16_t>("avoid");
		equip.ijump = row.get<int16_t>("jump");
		equip.ispeed = row.get<int16_t>("speed");
		equip.tamingMob = row.get<uint8_t>("taming_mob");
		equip.iceDamage = row.get<uint8_t>("inc_ice_damage");
		equip.fireDamage = row.get<uint8_t>("inc_fire_damage");
		equip.lightningDamage = row.get<uint8_t>("inc_lightning_damage");
		equip.poisonDamage = row.get<uint8_t>("inc_poison_damage");
		equip.elementalDefault = row.get<uint8_t>("elemental_default");
		equip.traction = row.get<double>("traction");
		equip.validSlots = StringUtilities::atoli(row.get<string_t>("equip_slot_flags").c_str());
		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&equip](const string_t &cmp) {
			if (cmp == "wear_trade_block") equip.tradeBlockOnEquip = true;
		});
		StringUtilities::runFlags(row.get<opt_string_t>("req_job"), [&equip](const string_t &cmp) {
			if (cmp == "common") equip.validJobs.push_back(-1);
			else if (cmp == "beginner") equip.validJobs.push_back(Jobs::JobTracks::Beginner);
			else if (cmp == "warrior") equip.validJobs.push_back(Jobs::JobTracks::Warrior);
			else if (cmp == "magician") equip.validJobs.push_back(Jobs::JobTracks::Magician);
			else if (cmp == "bowman") equip.validJobs.push_back(Jobs::JobTracks::Bowman);
			else if (cmp == "thief") equip.validJobs.push_back(Jobs::JobTracks::Thief);
			else if (cmp == "pirate") equip.validJobs.push_back(Jobs::JobTracks::Pirate);
		});

		m_equipInfo[itemId] = equip;
	}
}

auto EquipDataProvider::setEquipStats(Item *equip, bool random, bool isGm) -> void {
	const EquipInfo &ei = getEquipInfo(equip->getId());
	equip->setSlots(ei.slots);

	auto getStat = [random, isGm](int16_t equipAmount, int16_t variance) -> int16_t {
		if (!random) return equipAmount;
		if (equipAmount == 0) return 0;

		int16_t mod = isGm ? variance : Randomizer::rand<int16_t>(variance, -variance);
		return equipAmount + mod;
	};

	equip->setStr(getStat(ei.istr, Items::StatVariance::Str));
	equip->setDex(getStat(ei.idex, Items::StatVariance::Dex));
	equip->setInt(getStat(ei.iint, Items::StatVariance::Int));
	equip->setLuk(getStat(ei.iluk, Items::StatVariance::Luk));
	equip->setHp(getStat(ei.ihp, Items::StatVariance::Hp));
	equip->setMp(getStat(ei.imp, Items::StatVariance::Mp));
	equip->setWatk(getStat(ei.iwAtk, Items::StatVariance::Watk));
	equip->setMatk(getStat(ei.imAtk, Items::StatVariance::Matk));
	equip->setWdef(getStat(ei.iwDef, Items::StatVariance::Wdef));
	equip->setMdef(getStat(ei.imDef, Items::StatVariance::Mdef));
	equip->setAccuracy(getStat(ei.iacc, Items::StatVariance::Acc));
	equip->setAvoid(getStat(ei.iavo, Items::StatVariance::Avoid));
	equip->setHands(getStat(ei.ihand, Items::StatVariance::Hands));
	equip->setJump(getStat(ei.ijump, Items::StatVariance::Jump));
	equip->setSpeed(getStat(ei.ispeed, Items::StatVariance::Speed));
}

auto EquipDataProvider::canEquip(int32_t itemId, int16_t job, int16_t str, int16_t dex, int16_t intt, int16_t luk, int16_t fame) -> bool {
	const EquipInfo &ei = getEquipInfo(itemId);
	return str >= ei.reqStr && dex >= ei.reqDex && intt >= ei.reqInt && luk >= ei.reqLuk && fame >= ei.reqFame;
}

auto EquipDataProvider::validSlot(int32_t equipId, int16_t target) -> bool {
	const EquipInfo &ei = getEquipInfo(equipId);
	return (ei.validSlots & (1ULL << (target - 1))) != 0;
}