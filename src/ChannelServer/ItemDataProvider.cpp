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
#include <iostream>
#include <string>

using boost::bimap;
using Initializing::outputWidth;
using MiscUtilities::atob;
using std::string;

ItemDataProvider * ItemDataProvider::singleton = 0;

void ItemDataProvider::loadData() {
	// Equips
	std::cout << std::setw(outputWidth) << std::left << "Initializing Items... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM equipdata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW dataRow;
	while (dataRow = res.fetch_raw_row()) {
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

	// Monster cards
	query << "SELECT monstercarddata.cardid, monstercarddata.mobid FROM monstercarddata";
	res = query.use();
	while (dataRow = res.fetch_raw_row()) {
		// Col0 : Card ID
		//    1 : Mob ID
		int32_t cardid = atoi(dataRow[0]);
		int32_t mobid = atoi(dataRow[1]);
		cards.insert(card_info(cardid, mobid));
	}

	// Items
	query << "SELECT itemdata.*, itemsummondata.mobid, itemsummondata.chance FROM itemdata LEFT JOIN itemsummondata ON itemdata.itemid = itemsummondata.itemid ORDER BY itemid ASC";
	res = query.use();

	int32_t currentid = 0;
	int32_t previousid = -1;
	ItemInfo item;
	while (dataRow = res.fetch_raw_row()) {
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
		//   19 : Ailment
		//   20 : Morph
		//   21 : Auto Consume?
		//   22 : Success
		//   23 : Cursed
		//   24 : Randstat
		//   25 : Recover
		//   26 : Item STR
		//   27 : Item DEX
		//   28 : Item INT
		//   29 : Item LUK
		//   30 : Item HP
		//   31 : Item MP
		//   32 : Item Weapon Attack
		//   33 : Item Magic Attack
		//   34 : Item Weapon Defense
		//   35 : Item Magic Defense
		//   36 : Item Accuracy
		//   37 : Item Avoid
		//   38 : Item Jump
		//   39 : Item Speed
		//   40 : Has map effect?
		//   41 : Mob ID
		//   42 : Chance
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
		item.cons.ailment = atoi(dataRow[19]);
		item.cons.hasmapeffect = atob(dataRow[40]);
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
		item.cons.morph = atoi(dataRow[20]);
		item.cons.autoconsume = atob(dataRow[21]);
		// Scrolling
		item.cons.success = atoi(dataRow[22]);
		item.cons.cursed = atoi(dataRow[23]);
		item.cons.randstat = atob(dataRow[24]);
		item.cons.recover = atob(dataRow[25]);
		item.cons.istr = atoi(dataRow[26]);
		item.cons.idex = atoi(dataRow[27]);
		item.cons.iint = atoi(dataRow[28]);
		item.cons.iluk = atoi(dataRow[29]);
		item.cons.ihp = atoi(dataRow[30]);
		item.cons.imp = atoi(dataRow[31]);
		item.cons.iwatk = atoi(dataRow[32]);
		item.cons.imatk = atoi(dataRow[33]);
		item.cons.iwdef = atoi(dataRow[34]);
		item.cons.imdef = atoi(dataRow[35]);
		item.cons.iacc = atoi(dataRow[36]);
		item.cons.iavo = atoi(dataRow[37]);
		item.cons.ijump = atoi(dataRow[38]);
		item.cons.ispeed = atoi(dataRow[39]);
		item.cons.ihand = 0;
		// Summoning
		if (dataRow[41] != 0) {
			SummonBag summon;
			summon.mobid = atoi(dataRow[41]);
			summon.chance = atoi(dataRow[42]);
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

	while (dataRow = res.fetch_raw_row()) {
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

	// Item names
	query << "SELECT objectid, name FROM stringdata WHERE type = 1";
	res = query.use();

	while (dataRow = res.fetch_raw_row()) {
		// Col0 : Object/Item ID
		//    1 : Name
		item_names[atoi(dataRow[0])] = (string) dataRow[1];
	}
	std::cout << "DONE" << std::endl;
}

void ItemDataProvider::addItemInfo(int32_t id, ItemInfo item) {
	vector<uint8_t> types;
	vector<int8_t> bytes;
	vector<int16_t> values;
	bool buff = false;

	if (item.cons.watk > 0) {
		types.push_back(0x01);
		bytes.push_back(Byte1);
		values.push_back(item.cons.watk);
		buff = true;
	}
	if (item.cons.wdef > 0) {
		types.push_back(0x02);
		bytes.push_back(Byte1);
		values.push_back(item.cons.wdef);
		buff = true;
	}
	if (item.cons.matk > 0) {
		types.push_back(0x04);
		bytes.push_back(Byte1);
		values.push_back(item.cons.matk);
		buff = true;
	}
	if (item.cons.mdef > 0) {
		types.push_back(0x08);
		bytes.push_back(Byte1);
		values.push_back(item.cons.mdef);
		buff = true;
	}
	if (item.cons.acc > 0) {
		types.push_back(0x10);
		bytes.push_back(Byte1);
		values.push_back(item.cons.acc);
		buff = true;
	}
	if (item.cons.avo > 0) {
		types.push_back(0x20);
		bytes.push_back(Byte1);
		values.push_back(item.cons.avo);
		buff = true;
	}
	if (item.cons.speed > 0) {
		types.push_back(0x80);
		bytes.push_back(Byte1);
		values.push_back(item.cons.speed);
		buff = true;
	}
	if (item.cons.jump > 0) {
		types.push_back(0x01);
		bytes.push_back(Byte2);
		values.push_back(item.cons.jump);
		buff = true;
	}
	if (item.cons.morph > 0) {
		types.push_back(0x02);
		bytes.push_back(Byte5);
		values.push_back(item.cons.morph);
		buff = true;
	}

	if (buff)
		Buffs::Instance()->addItemInfo(id, types, bytes, values);

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

int16_t ItemDataProvider::getMaxSlot(int32_t itemid) {
	if (GameLogicUtilities::isEquip(itemid))
		return 1;
	else
		return items.find(itemid) != items.end() ? items[itemid].maxslot : 0;
}

int32_t ItemDataProvider::getCardId(int32_t mobid) {
	try {
		return cards.right.at(mobid);
	}
	catch (std::out_of_range) {

	}
	return 0;
}

int32_t ItemDataProvider::getMobId(int32_t cardid) {
	try {
		return cards.left.at(cardid);
	}
	catch (std::out_of_range) {

	}
	return 0;
}

string ItemDataProvider::getItemName(int32_t itemid) {
	return item_names.find(itemid) != item_names.end() ? item_names[itemid] : "";
}