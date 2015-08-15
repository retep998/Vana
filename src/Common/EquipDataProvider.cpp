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
#include "EquipDataProvider.hpp"
#include "Database.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "InitializeCommon.hpp"
#include "Randomizer.hpp"
#include "StringUtilities.hpp"
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

auto EquipDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Equips... ";

	loadEquips();

	std::cout << "DONE" << std::endl;
}

auto EquipDataProvider::loadEquips() -> void {
	m_equipInfo.clear();

	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	// Ugly hack to get the integers instead of scientific notation
	// Note: This is MySQL's crappy behavior
	// It displays scientific notation for only very large values, meaning it's wildly inconsistent and hard to parse
	// We just use the string and send it to a translation function
	soci::rowset<> rs = (sql.prepare
		<< "SELECT *, REPLACE(FORMAT(equip_slots + 0, 0), \",\", \"\") AS equip_slot_flags "
		<< "FROM " << db.makeTable("item_equip_data"));

	for (const auto &row : rs) {
		EquipInfo equip;

		item_id_t itemId = row.get<item_id_t>("itemid");
		equip.attackSpeed = row.get<int8_t>("attack_speed");
		equip.healing = row.get<int8_t>("heal_hp");
		equip.slots = row.get<int8_t>("scroll_slots");
		equip.ihp = row.get<health_t>("hp");
		equip.imp = row.get<health_t>("mp");
		equip.reqStr = row.get<stat_t>("req_str");
		equip.reqDex = row.get<stat_t>("req_dex");
		equip.reqInt = row.get<stat_t>("req_int");
		equip.reqLuk = row.get<stat_t>("req_luk");
		equip.reqFame = row.get<fame_t>("req_fame");
		equip.istr = row.get<stat_t>("strength");
		equip.idex = row.get<stat_t>("dexterity");
		equip.iint = row.get<stat_t>("intelligence");
		equip.iluk = row.get<stat_t>("luck");
		equip.ihand = row.get<stat_t>("hands");
		equip.iwAtk = row.get<stat_t>("weapon_attack");
		equip.iwDef = row.get<stat_t>("weapon_defense");
		equip.imAtk = row.get<stat_t>("magic_attack");
		equip.imDef = row.get<stat_t>("magic_defense");
		equip.iacc = row.get<stat_t>("accuracy");
		equip.iavo = row.get<stat_t>("avoid");
		equip.ijump = row.get<stat_t>("jump");
		equip.ispeed = row.get<stat_t>("speed");
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

auto EquipDataProvider::setEquipStats(Item *equip, Items::StatVariance variancePolicy, bool isGm, bool isItemInitialization) const -> void {
	const EquipInfo &ei = getEquipInfo(equip->getId());
	if (isItemInitialization) {
		equip->setSlots(ei.slots);
	}

	auto getStat = [variancePolicy, isItemInitialization, isGm](int16_t baseEquipAmount, int16_t equipAmount) -> int16_t {
		int16_t amount = isItemInitialization ? baseEquipAmount : equipAmount;

		if (amount == 0 || variancePolicy == Items::StatVariance::None) {
			return amount;
		}

		bool increaseOnly = false;
		if (variancePolicy == Items::StatVariance::OnlyIncreaseWithGreatChance) {
			if (!isGm && Randomizer::rand<int8_t>(10, 1) <= 3) {
				return amount;
			}
			increaseOnly = true;
		}
		else if (variancePolicy == Items::StatVariance::OnlyIncreaseWithAmazingChance) {
			if (!isGm && Randomizer::rand<int8_t>(10, 1) == 1) {
				return amount;
			}
			increaseOnly = true;
		}

		int16_t variance = -1;
		switch (variancePolicy) {
			case Items::StatVariance::Gachapon:
				variance = std::min<int16_t>(amount / 5 + 1, 7);
				break;
			case Items::StatVariance::ChaosNormal:
				variance = 5;
				break;
			case Items::StatVariance::ChaosHigh:
				variance = 7;
				break;
			default:
				variance = std::min<int16_t>(amount / 10 + 1, 5);
		}

		if (!isGm) {
			// This code turns a single event into a series of events to give a normal distribution
			// e.g. 1 event of [0, 5] is turned into 7 events of [0, 1]
			// This makes it like flipping 7 coins instead of rolling a single die

			std::binomial_distribution<> dist{variance + 2, .5};
			variance = Randomizer::rand(dist) - 2;
		}

		if (variance <= 0) {
			return amount;
		}

		if (isGm || increaseOnly || Randomizer::rand<bool>()) {
			return amount + variance;
		}

		return amount - variance;
	};

	equip->setStr(getStat(ei.istr, equip->getStr()));
	equip->setDex(getStat(ei.idex, equip->getDex()));
	equip->setInt(getStat(ei.iint, equip->getInt()));
	equip->setLuk(getStat(ei.iluk, equip->getLuk()));
	equip->setHp(getStat(ei.ihp, equip->getHp()));
	equip->setMp(getStat(ei.imp, equip->getMp()));
	equip->setWatk(getStat(ei.iwAtk, equip->getWatk()));
	equip->setMatk(getStat(ei.imAtk, equip->getMatk()));
	equip->setWdef(getStat(ei.iwDef, equip->getWdef()));
	equip->setMdef(getStat(ei.imDef, equip->getMdef()));
	equip->setAccuracy(getStat(ei.iacc, equip->getAccuracy()));
	equip->setAvoid(getStat(ei.iavo, equip->getAvoid()));
	equip->setHands(getStat(ei.ihand, equip->getHands()));
	equip->setJump(getStat(ei.ijump, equip->getJump()));
	equip->setSpeed(getStat(ei.ispeed, equip->getSpeed()));
}

auto EquipDataProvider::canEquip(item_id_t itemId, gender_id_t gender, job_id_t job, stat_t str, stat_t dex, stat_t intt, stat_t luk, fame_t fame) const -> bool {
	const EquipInfo &ei = getEquipInfo(itemId);
	return str >= ei.reqStr && dex >= ei.reqDex && intt >= ei.reqInt && luk >= ei.reqLuk && fame >= ei.reqFame;
}

auto EquipDataProvider::isValidSlot(item_id_t equipId, inventory_slot_t target) const -> bool {
	const EquipInfo &ei = getEquipInfo(equipId);
	return (ei.validSlots & (1ULL << (target - 1))) != 0;
}

auto EquipDataProvider::getSlots(item_id_t equipId) const -> int8_t {
	return getEquipInfo(equipId).slots;
}

auto EquipDataProvider::getEquipInfo(item_id_t equipId) const -> const EquipInfo & {
	return m_equipInfo.find(equipId)->second;
}