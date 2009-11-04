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
#include "EquipDataProvider.h"
#include "Database.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "Randomizer.h"
#include "StringUtilities.h"
#include <iostream>
#include <string>

using std::string;
using Initializing::outputWidth;
using StringUtilities::atoli;
using StringUtilities::runFlags;

EquipDataProvider * EquipDataProvider::singleton = 0;

void EquipDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Equips... ";

	loadEquips();

	std::cout << "DONE" << std::endl;
}

namespace Functors {
	struct EquipFlags {
		void operator() (const string &cmp) {
			if (cmp == "wear_trade_block") item->tradeblockonequip = true;
		}
		EquipInfo *item;
	};
	struct JobFlags {
		void operator() (const string &cmp) {
			if (cmp == "common") item->validjobs.push_back(-1);
			else if (cmp == "beginner") item->validjobs.push_back(Jobs::JobTracks::Beginner); // Respective job tracks
			else if (cmp == "warrior") item->validjobs.push_back(Jobs::JobTracks::Warrior);
			else if (cmp == "magician") item->validjobs.push_back(Jobs::JobTracks::Magician);
			else if (cmp == "bowman") item->validjobs.push_back(Jobs::JobTracks::Bowman);
			else if (cmp == "thief") item->validjobs.push_back(Jobs::JobTracks::Thief);
			else if (cmp == "pirate") item->validjobs.push_back(Jobs::JobTracks::Pirate);
		}
		EquipInfo *item;
	};
}

void EquipDataProvider::loadEquips() {
	equips.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT *, REPLACE(FORMAT(equip_slots + 0, 0), \",\", \"\") FROM item_equip_data");
	// Ugly hack to get the integers instead of scientific notation
	// Note to users: This is MySQL's crappy behavior
	// It displays scientific notation for only very large values, meaning it's wildly inconsistent and hard to parse
	mysqlpp::UseQueryResult res = query.use();
	int32_t id;
	int64_t bits;
	int64_t ibit;
	EquipInfo equip;

	using namespace Functors;

	enum EquipData {
		EquipId = 0,
		Flags, Unused, AttackSpeed, Healing, ScrollSlots,
		ReqStr, ReqDex, ReqInt, ReqLuk, ReqFame,
		ReqJob, Hp, Mp, Str, Dex,
		Int, Luk, Hands, Watk, Wdef,
		Matk, Mdef, Acc, Avoid, Jump,
		Speed, Traction, TamingMob, LightningDamage, IceDamage,
		FireDamage, PoisonDamage, ElementalDefault, EquipSlots
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		equip = EquipInfo();

		EquipFlags whoo = {&equip};
		runFlags(row[Flags], whoo);

		JobFlags whoot = {&equip};
		runFlags(row[ReqJob], whoot);

		id = atoi(row[EquipId]);
		equip.attackspeed = atoi(row[AttackSpeed]);
		equip.healing = atoi(row[Healing]);
		equip.slots = atoi(row[ScrollSlots]);
		equip.ihp = atoi(row[Hp]);
		equip.imp = atoi(row[Mp]);
		equip.reqstr = atoi(row[ReqStr]);
		equip.reqdex = atoi(row[ReqDex]);
		equip.reqint = atoi(row[ReqInt]);
		equip.reqluk = atoi(row[ReqLuk]);
		equip.reqfame = atoi(row[ReqFame]);
		equip.istr = atoi(row[Str]);
		equip.idex = atoi(row[Dex]);
		equip.iint = atoi(row[Int]);
		equip.iluk = atoi(row[Luk]);
		equip.ihand = atoi(row[Hands]);
		equip.iwatk = atoi(row[Watk]);
		equip.iwdef = atoi(row[Wdef]);
		equip.imatk = atoi(row[Matk]);
		equip.imdef = atoi(row[Mdef]);
		equip.iacc = atoi(row[Acc]);
		equip.iavo = atoi(row[Avoid]);
		equip.ijump = atoi(row[Jump]);
		equip.ispeed = atoi(row[Speed]);
		equip.tamingmob = atoi(row[TamingMob]);
		equip.icedamage = atoi(row[IceDamage]);
		equip.firedamage = atoi(row[FireDamage]);
		equip.lightningdamage = atoi(row[LightningDamage]);
		equip.poisondamage = atoi(row[PoisonDamage]);
		equip.elementaldefault = atoi(row[ElementalDefault]);
		equip.traction = atof(row[Traction]);

		bits = atoli(row[EquipSlots]);
		for (ibit = 1; ibit <= bits; ibit <<= 1) {
			if (bits & ibit) {
				equip.validslots |= ibit;
			}
		}

		equips[id] = equip;
	}
}

void EquipDataProvider::setEquipStats(Item *equip, bool random) {
	EquipInfo *ei = getEquipInfo(equip->id);
	equip->slots = ei->slots;
	if (!random) {
		equip->istr = ei->istr;
		equip->idex = ei->idex;
		equip->iint = ei->iint;
		equip->iluk = ei->iluk;
		equip->ihp = ei->ihp;
		equip->imp = ei->imp;
		equip->iwatk = ei->iwatk;
		equip->imatk = ei->imatk;
		equip->iwdef = ei->iwdef;
		equip->imdef = ei->imdef;
		equip->iacc = ei->iacc;
		equip->iavo = ei->iavo;
		equip->ihand = ei->ihand;
		equip->ijump = ei->ijump;
		equip->ispeed = ei->ispeed;
	}
	else {
		equip->istr = ei->istr > 0 ? ei->istr + Randomizer::Instance()->randShort(2) - 1 : 0;
		equip->idex = ei->idex > 0 ? ei->idex + Randomizer::Instance()->randShort(2) - 1 : 0;
		equip->iint = ei->iint > 0 ? ei->iint + Randomizer::Instance()->randShort(2) - 1 : 0;
		equip->iluk = ei->iluk > 0 ? ei->iluk + Randomizer::Instance()->randShort(2) - 1 : 0;
		equip->ihp = ei->ihp > 0 ? ei->ihp + Randomizer::Instance()->randShort(10) - 5 : 0;
		equip->imp = ei->imp > 0 ? ei->imp + Randomizer::Instance()->randShort(10) - 5 : 0;
		equip->iwatk = ei->iwatk > 0 ? ei->iwatk + Randomizer::Instance()->randShort(10) - 5 : 0;
		equip->imatk = ei->imatk > 0 ? ei->imatk + Randomizer::Instance()->randShort(10) - 5 : 0;
		equip->iwdef = ei->iwdef > 0 ? ei->iwdef + Randomizer::Instance()->randShort(10) - 5 : 0;
		equip->imdef = ei->imdef > 0 ? ei->imdef + Randomizer::Instance()->randShort(10) - 5 : 0;
		equip->iacc = ei->iacc > 0 ? ei->iacc + Randomizer::Instance()->randShort(2) - 1 : 0;
		equip->iavo = ei->iavo > 0 ? ei->iavo + Randomizer::Instance()->randShort(2) - 1 : 0;
		equip->ihand = ei->ihand > 0 ? ei->ihand + Randomizer::Instance()->randShort(2) - 1 : 0;
		equip->ijump = ei->ijump > 0 ? ei->ijump + Randomizer::Instance()->randShort(2) - 1 : 0;
		equip->ispeed = ei->ispeed > 0 ? ei->ispeed + Randomizer::Instance()->randShort(2) - 1 : 0;
	}
}

bool EquipDataProvider::canEquip(int32_t itemid, int16_t job, int16_t str, int16_t dex, int16_t intt, int16_t luk, int16_t fame) {
	EquipInfo *e = getEquipInfo(itemid);
	return (str >= e->reqstr && dex >= e->reqdex && intt >= e->reqint && luk >= e->reqluk && fame >= e->reqfame);
}

bool EquipDataProvider::validSlot(int32_t equipid, int16_t target) {
	EquipInfo *e = getEquipInfo(equipid);
	return ((e->validslots & (1LL << (target - 1))) != 0);
}