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
#include "ItemDataProvider.h"
#include "Buffs.h"
#include "Database.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "Inventory.h"
#include "MiscUtilities.h"
#include "ShopDataProvider.h"
#include <string>
#include <iostream>

using MiscUtilities::atob;
using std::string;
using Initializing::outputWidth;

ItemDataProvider *ItemDataProvider::singleton = 0;

void ItemDataProvider::loadData() {
	// Equips
	std::cout << std::setw(outputWidth) << std::left << "Initializing Items... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM equipdata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW dataRow;
	while ((dataRow = res.fetch_raw_row())) {
		// Col0 : EquipID
		//    1 : Price
		//    2 : Slots
		//    3 : HP
		//    4 : MP
		//    5 : STR
		//    6 : DEX
		//    7 : INT
		//    8 : LUK
		//    9 : WAtk
		//   10 : WDef
		//   11 : MAtk
		//   12 : MDef
		//   13 : Acc
		//   14 : Avo
		//   15 : Jump
		//   16 : Speed
		//   17 : Taming Mob
		//   18 : Only one?
		//   19 : No trading?
		//   20 : Quest item?
		EquipInfo equip = EquipInfo();
		equip.price = atoi(dataRow[1]);
		equip.slots = atoi(dataRow[2]);
		equip.ihp = atoi(dataRow[3]);
		equip.imp = atoi(dataRow[4]);
		equip.istr = atoi(dataRow[5]);
		equip.idex = atoi(dataRow[6]);
		equip.iint = atoi(dataRow[7]);
		equip.iluk = atoi(dataRow[8]);
		equip.iwatk = atoi(dataRow[9]);
		equip.iwdef = atoi(dataRow[10]);
		equip.imatk = atoi(dataRow[11]);
		equip.imdef = atoi(dataRow[12]);
		equip.iacc = atoi(dataRow[13]);
		equip.iavo = atoi(dataRow[14]);
		equip.ijump = atoi(dataRow[15]);
		equip.ispeed = atoi(dataRow[16]);
		equip.tamingmob = atoi(dataRow[17]);
		equip.onlyone = atob(dataRow[18]);
		equip.notrade = atob(dataRow[19]);
		equip.quest = atob(dataRow[20]);
		equip.ihand = 0;
		// Add equip to the equip info table
		equips[atoi(dataRow[0])] = equip;
	}

	// Items
	query << "SELECT itemdata.*, itemsummondata.mobid, itemsummondata.chance FROM itemdata LEFT JOIN itemsummondata ON itemdata.itemid=itemsummondata.itemid ORDER BY itemid ASC";
	res = query.use();

	int32_t currentid = 0;
	int32_t previousid = -1;
	ItemInfo item;
	while ((dataRow = res.fetch_raw_row())) {
		// Col0 : Item ID
		//    1 : Price
		//    2 : Max per slot
		//    3 : Not tradable?
		//    4 : Quest item?
		//    5 : HP
		//    6 : MP
		//    7 : HP Rate
		//    8 : MP Rate
		//    9 : Move to
		//   10 : Time
		//   11 : Weapon Attack
		//   12 : Magic Attack
		//   13 : Avoidability
		//   14 : Accuracy
		//   15 : Weapon Defense
		//   16 : Magic Defense
		//   17 : Speed
		//   18 : Jump
		//   19 : Morph
		//   20 : Auto Consume?
		//   21 : Success
		//   22 : Cursed
		//   23 : Item STR
		//   24 : Item DEX
		//   25 : Item INT
		//   26 : Item LUK
		//   27 : Item HP
		//   28 : Item MP
		//   29 : Item Weapon Attack
		//   30 : Item Magic Attack
		//   31 : Item Weapon Defense
		//   32 : Item Magic Defense
		//   33 : Item Accuracy
		//   34 : Item Avoid
		//   35 : Item Jump
		//   36 : Item Speed
		//   37 : Mob ID
		//   38 : Chance
		currentid = atoi(dataRow[0]);

		if (currentid != previousid && previousid != -1) { // Add the items into the cache
			addItemInfo(previousid, item);
			item.cons.mobs.clear();
		}
		item.price = atoi(dataRow[1]);
		item.maxslot = atoi(dataRow[2]);
		item.notrade = atob(dataRow[3]);
		item.quest = atob(dataRow[4]);
		item.cons.hp = atoi(dataRow[5]);
		item.cons.mp = atoi(dataRow[6]);
		item.cons.hpr = atoi(dataRow[7]);
		item.cons.mpr = atoi(dataRow[8]);
		item.cons.moveTo = atoi(dataRow[9]);
		// Buffs
		item.cons.time = atoi(dataRow[10]);
		item.cons.watk = atoi(dataRow[11]);
		item.cons.matk = atoi(dataRow[12]);
		item.cons.avo = atoi(dataRow[13]);
		item.cons.acc = atoi(dataRow[14]);
		item.cons.wdef = atoi(dataRow[15]);
		item.cons.mdef = atoi(dataRow[16]);
		item.cons.speed = atoi(dataRow[17]);
		item.cons.jump = atoi(dataRow[18]);
		item.cons.morph = atoi(dataRow[19]);
		item.cons.autoconsume = atob(dataRow[20]);
		// Scrolling
		item.cons.success = atoi(dataRow[21]);
		item.cons.cursed = atoi(dataRow[22]);
		item.cons.istr = atoi(dataRow[23]);
		item.cons.idex = atoi(dataRow[24]);
		item.cons.iint = atoi(dataRow[25]);
		item.cons.iluk = atoi(dataRow[26]);
		item.cons.ihp = atoi(dataRow[27]);
		item.cons.imp = atoi(dataRow[28]);
		item.cons.iwatk = atoi(dataRow[29]);
		item.cons.imatk = atoi(dataRow[30]);
		item.cons.iwdef = atoi(dataRow[31]);
		item.cons.imdef = atoi(dataRow[32]);
		item.cons.iacc = atoi(dataRow[33]);
		item.cons.iavo = atoi(dataRow[34]);
		item.cons.ijump = atoi(dataRow[35]);
		item.cons.ispeed = atoi(dataRow[36]);
		item.cons.ihand = 0;
		// Summoning
		if (dataRow[37] != 0) {
			SummonBag summon;
			summon.mobid = atoi(dataRow[37]);
			summon.chance = atoi(dataRow[38]);
			item.cons.mobs.push_back(summon);
		}

		previousid = currentid;
	}
	// Add the final entry
	if (previousid != -1) {
		addItemInfo(previousid, item);
		item.cons.mobs.clear();
	}

	// Item Skills (Skill/Mastery Books)
	query << "SELECT * FROM itemskilldata";
	res = query.use();

	while ((dataRow = res.fetch_raw_row())) {
		// Col0 : Item ID
		//    1 : Skill ID
		//    2 : Required Level
		//    3 : Master Level
		Skillbook skill;
		skill.skillid = atoi(dataRow[1]);
		skill.reqlevel = atoi(dataRow[2]);
		skill.maxlevel = atoi(dataRow[3]);
		items[atoi(dataRow[0])].cons.skills.push_back(skill);
	}
	std::cout << "DONE" << std::endl;
}

void ItemDataProvider::addItemInfo(int32_t id, ItemInfo item) {
	if (GameLogicUtilities::isRechargeable(id))
		ShopDataProvider::Instance()->addRechargable(id);
	// Set all types to 0 initially
	memset(item.cons.types, 0, sizeof(item.cons.types));

	if (item.cons.watk > 0) {
		item.cons.types[Type_1] += 0x01;
		item.cons.vals.push_back(item.cons.watk);
	}
	if (item.cons.wdef > 0) {
		item.cons.types[Type_1] += 0x02;
		item.cons.vals.push_back(item.cons.wdef);
	}
	if (item.cons.matk > 0) {
		item.cons.types[Type_1] += 0x04;
		item.cons.vals.push_back(item.cons.matk);
	}
	if (item.cons.mdef > 0) {
		item.cons.types[Type_1] += 0x08;
		item.cons.vals.push_back(item.cons.mdef);
	}
	if (item.cons.acc > 0) {
		item.cons.types[Type_1] += 0x10;
		item.cons.vals.push_back(item.cons.acc);
	}
	if (item.cons.avo > 0) {
		item.cons.types[Type_1] += 0x20;
		item.cons.vals.push_back(item.cons.avo);
	}
	if (item.cons.speed > 0) {
		item.cons.types[Type_1] += 0x80;
		item.cons.vals.push_back(item.cons.speed);
	}
	if (item.cons.jump > 0) {
		item.cons.types[Type_2] = 0x01;
		item.cons.vals.push_back(item.cons.jump);
	}
	if (item.cons.morph > 0) {
		item.cons.types[Type_5] = 0x02;
		item.cons.vals.push_back(item.cons.morph);
	}

	items[id] = item;
}

bool ItemDataProvider::itemExists(int32_t id) {
	return (equips.find(id) != equips.end() || items.find(id) != items.end());
}

int32_t ItemDataProvider::getPrice(int32_t itemid) {
	if (GameLogicUtilities::isEquip(itemid))
		return equips.find(itemid) != equips.end() ? equips[itemid].price : 0;
	else
		return items.find(itemid) != items.end() ? items[itemid].price : 0;
}

int16_t ItemDataProvider::getMaxslot(int32_t itemid) {
	if (GameLogicUtilities::isEquip(itemid))
		return 1;
	else
		return items.find(itemid) != items.end() ? items[itemid].maxslot : 0;
}
