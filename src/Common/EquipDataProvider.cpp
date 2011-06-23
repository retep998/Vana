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
using Initializing::OutputWidth;
using StringUtilities::atoli;
using StringUtilities::runFlags;

EquipDataProvider * EquipDataProvider::singleton = nullptr;

void EquipDataProvider::loadData() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Equips... ";

	loadEquips();

	std::cout << "DONE" << std::endl;
}

namespace Functors {
	struct EquipFlags {
		void operator() (const string &cmp) {
			if (cmp == "wear_trade_block") item->tradeBlockOnEquip = true;
		}
		EquipInfo *item;
	};
	struct EquipJobFlags {
		void operator() (const string &cmp) {
			if (cmp == "common") item->validJobs.push_back(-1);
			else if (cmp == "beginner") item->validJobs.push_back(Jobs::JobTracks::Beginner); // Respective job tracks
			else if (cmp == "warrior") item->validJobs.push_back(Jobs::JobTracks::Warrior);
			else if (cmp == "magician") item->validJobs.push_back(Jobs::JobTracks::Magician);
			else if (cmp == "bowman") item->validJobs.push_back(Jobs::JobTracks::Bowman);
			else if (cmp == "thief") item->validJobs.push_back(Jobs::JobTracks::Thief);
			else if (cmp == "pirate") item->validJobs.push_back(Jobs::JobTracks::Pirate);
		}
		EquipInfo *item;
	};

}

void EquipDataProvider::loadEquips() {
	m_equipInfo.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT *, REPLACE(FORMAT(equip_slots + 0, 0), \",\", \"\") FROM item_equip_data");
	// Ugly hack to get the integers instead of scientific notation
	// Note to users: This is MySQL's crappy behavior
	// It displays scientific notation for only very large values, meaning it's wildly inconsistent and hard to parse
	mysqlpp::UseQueryResult res = query.use();
	int32_t id;
	EquipInfo equip;

	using namespace Functors;

	enum EquipData {
		EquipId = 0,
		Flags, Unused, AttackSpeed, Healing, ScrollSlots,
		SpecialId, ReqStr, ReqDex, ReqInt, ReqLuk,
		ReqFame, ReqJob, Hp, Mp, Str,
		Dex, Int, Luk, Hands, Watk,
		Wdef, Matk, Mdef, Acc, Avoid,
		Jump, Speed, Traction, Recovery, Knockback,
		TamingMob, LightningDamage, IceDamage, FireDamage, PoisonDamage,
		ElementalDefault, EquipSlots
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		equip = EquipInfo();
		EquipFlags whoo = {&equip};
		runFlags(row[Flags], whoo);
		EquipJobFlags whoot = {&equip};
		runFlags(row[ReqJob], whoot);

		id = atoi(row[EquipId]);
		equip.attackSpeed = atoi(row[AttackSpeed]);
		equip.healing = atoi(row[Healing]);
		equip.slots = atoi(row[ScrollSlots]);
		equip.ihp = atoi(row[Hp]);
		equip.imp = atoi(row[Mp]);
		equip.reqStr = atoi(row[ReqStr]);
		equip.reqDex = atoi(row[ReqDex]);
		equip.reqInt = atoi(row[ReqInt]);
		equip.reqLuk = atoi(row[ReqLuk]);
		equip.reqFame = atoi(row[ReqFame]);
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
		equip.tamingMob = atoi(row[TamingMob]);
		equip.iceDamage = atoi(row[IceDamage]);
		equip.fireDamage = atoi(row[FireDamage]);
		equip.lightningDamage = atoi(row[LightningDamage]);
		equip.poisonDamage = atoi(row[PoisonDamage]);
		equip.elementalDefault = atoi(row[ElementalDefault]);
		equip.traction = atof(row[Traction]);
		equip.validSlots = atoli(row[EquipSlots]);

		m_equipInfo[id] = equip;
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
	return ((e->validSlots & (1LL << (target - 1))) != 0);
}