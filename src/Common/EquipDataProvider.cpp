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

using std::string;
using Initializing::OutputWidth;
using StringUtilities::atoli;
using StringUtilities::runFlags;

EquipDataProvider * EquipDataProvider::singleton = nullptr;

void EquipDataProvider::loadData() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Equips... ";

	loadEquips();

	std::cout << "DONE" << std::endl;
}

void EquipDataProvider::loadEquips() {
	m_equipInfo.clear();
	int32_t itemId;
	EquipInfo equip;
	string flags;
	// Ugly hack to get the integers instead of scientific notation
	// Note: This is MySQL's crappy behavior
	// It displays scientific notation for only very large values, meaning it's wildly inconsistent and hard to parse
	// We just use the string and send it to a translation function
	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT *, REPLACE(FORMAT(equip_slots + 0, 0), \",\", \"\") AS equip_slot_flags FROM item_equip_data");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;
		equip = EquipInfo();

		runFlags(row.get<opt_string>("flags"), [&equip](const string &cmp) {
			if (cmp == "wear_trade_block") equip.tradeBlockOnEquip = true;
		});
		runFlags(row.get<opt_string>("req_job"), [&equip](const string &cmp) {
			if (cmp == "common") equip.validJobs.push_back(-1);
			else if (cmp == "beginner") equip.validJobs.push_back(Jobs::JobTracks::Beginner);
			else if (cmp == "warrior") equip.validJobs.push_back(Jobs::JobTracks::Warrior);
			else if (cmp == "magician") equip.validJobs.push_back(Jobs::JobTracks::Magician);
			else if (cmp == "bowman") equip.validJobs.push_back(Jobs::JobTracks::Bowman);
			else if (cmp == "thief") equip.validJobs.push_back(Jobs::JobTracks::Thief);
			else if (cmp == "pirate") equip.validJobs.push_back(Jobs::JobTracks::Pirate);
			else if (cmp == "dual_blade") equip.validJobs.push_back(Jobs::JobTracks::Thief); // TODO: Make this better?
		});

		itemId = row.get<int32_t>("itemid");
		equip.attackSpeed = row.get<int8_t>("attack_speed", 0);
		equip.healing = row.get<int8_t>("heal_hp", 0);
		equip.slots = row.get<int8_t>("scroll_slots", 0);
		equip.ihp = row.get<int16_t>("hp", 0);
		equip.imp = row.get<int16_t>("mp", 0);
		equip.reqStr = row.get<int16_t>("req_str", 0);
		equip.reqDex = row.get<int16_t>("req_dex", 0);
		equip.reqInt = row.get<int16_t>("req_int", 0);
		equip.reqLuk = row.get<int16_t>("req_luk", 0);
		equip.reqFame = row.get<int16_t>("req_fame", 0);
		equip.istr = row.get<int16_t>("strength", 0);
		equip.idex = row.get<int16_t>("dexterity", 0);
		equip.iint = row.get<int16_t>("intelligence", 0);
		equip.iluk = row.get<int16_t>("luck", 0);
		equip.ihand = row.get<int16_t>("hands", 0);
		equip.iwatk = row.get<int16_t>("weapon_attack", 0);
		equip.iwdef = row.get<int16_t>("weapon_defense", 0);
		equip.imatk = row.get<int16_t>("magic_attack", 0);
		equip.imdef = row.get<int16_t>("magic_defense", 0);
		equip.iacc = row.get<int16_t>("accuracy", 0);
		equip.iavo = row.get<int16_t>("avoid", 0);
		equip.ijump = row.get<int16_t>("jump", 0);
		equip.ispeed = row.get<int16_t>("speed", 0);
		equip.tamingMob = row.get<uint8_t>("taming_mob", 0);
		equip.iceDamage = row.get<uint8_t>("inc_ice_damage", 100);
		equip.fireDamage = row.get<uint8_t>("inc_fire_damage", 100);
		equip.lightningDamage = row.get<uint8_t>("inc_lightning_damage", 100);
		equip.poisonDamage = row.get<uint8_t>("inc_poison_damage", 100);
		equip.elementalDefault = row.get<uint8_t>("elemental_default", 100);
		equip.traction = row.get<double>("traction", 0.0f);
		equip.validSlots = atoli(row.get<string>("equip_slot_flags").c_str());

		m_equipInfo[itemId] = equip;
	}
}

void EquipDataProvider::setEquipStats(Item *equip, bool random) {
	EquipInfo *ei = getEquipInfo(equip->getId());
	equip->setSlots(ei->slots);
	if (!random) {
		equip->setStr(ei->istr);
		equip->setDex(ei->idex);
		equip->setInt(ei->iint);
		equip->setLuk(ei->iluk);
		equip->setHp(ei->ihp);
		equip->setMp(ei->imp);
		equip->setWatk(ei->iwatk);
		equip->setMatk(ei->imatk);
		equip->setWdef(ei->iwdef);
		equip->setMdef(ei->imdef);
		equip->setAccuracy(ei->iacc);
		equip->setAvoid(ei->iavo);
		equip->setHands(ei->ihand);
		equip->setJump(ei->ijump);
		equip->setSpeed(ei->ispeed);
	}
	else {
		equip->setStr(getRandomStat(ei->istr, Items::StatVariance::Str));
		equip->setDex(getRandomStat(ei->idex, Items::StatVariance::Dex));
		equip->setInt(getRandomStat(ei->iint, Items::StatVariance::Int));
		equip->setLuk(getRandomStat(ei->iluk, Items::StatVariance::Luk));
		equip->setHp(getRandomStat(ei->ihp, Items::StatVariance::Hp));
		equip->setMp(getRandomStat(ei->imp, Items::StatVariance::Mp));
		equip->setWatk(getRandomStat(ei->iwatk, Items::StatVariance::Watk));
		equip->setMatk(getRandomStat(ei->imatk, Items::StatVariance::Matk));
		equip->setWdef(getRandomStat(ei->iwdef, Items::StatVariance::Wdef));
		equip->setMdef(getRandomStat(ei->imdef, Items::StatVariance::Mdef));
		equip->setAccuracy(getRandomStat(ei->iacc, Items::StatVariance::Acc));
		equip->setAvoid(getRandomStat(ei->iavo, Items::StatVariance::Avoid));
		equip->setHands(getRandomStat(ei->ihand, Items::StatVariance::Hands));
		equip->setJump(getRandomStat(ei->ijump, Items::StatVariance::Jump));
		equip->setSpeed(getRandomStat(ei->ispeed, Items::StatVariance::Speed));
	}
}

int16_t EquipDataProvider::getRandomStat(int16_t equipAmount, uint16_t variance) {
	return (equipAmount > 0 ? equipAmount + getStatVariance(variance) : 0);
}

int16_t EquipDataProvider::getStatVariance(uint16_t amount) {
	int16_t s = Randomizer::Instance()->randShort(amount);
	s -= (amount / 2);
	return s;
}

bool EquipDataProvider::canEquip(int32_t itemId, int16_t job, int16_t str, int16_t dex, int16_t intt, int16_t luk, int16_t fame) {
	EquipInfo *e = getEquipInfo(itemId);
	return (str >= e->reqStr && dex >= e->reqDex && intt >= e->reqInt && luk >= e->reqLuk && fame >= e->reqFame);
}

bool EquipDataProvider::validSlot(int32_t equipId, int16_t target) {
	EquipInfo *e = getEquipInfo(equipId);
	return ((e->validSlots & (1ULL << (target - 1))) != 0);
}