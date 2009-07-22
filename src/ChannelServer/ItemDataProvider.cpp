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

using Initializing::outputWidth;
using MiscUtilities::atob;
using std::string;

ItemDataProvider * ItemDataProvider::singleton = 0;

void ItemDataProvider::loadData() {
	// Equips
	std::cout << std::setw(outputWidth) << std::left << "Initializing Items... ";
	equips.clear();
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

	// Items
	items.clear();
	query << "SELECT itemdata.*, itemsummondata.mobid, itemsummondata.chance FROM itemdata LEFT JOIN itemsummondata ON itemdata.itemid = itemsummondata.itemid ORDER BY itemid ASC";
	res = query.use();

	int32_t currentid = 0;
	int32_t previousid = -1;
	ItemInfo item;

	enum ItemDataColumns {
		ItemId = 0,
		Price, SlotMax, Tradeable, Quest, AutoConsume,
		Hp, Mp, HpRate, MpRate, MoveTo,
		Ailment, MonCardBuffProb, Time, WeaponAtt, MagicAtt,
		Avoid, Accuracy, WeaponDef, MagicDef, Speed,
		Jump, Morph, MesoUp, DropUp, IgnoreWeaponDef,
		IgnoreMagicDef, IncreaseElementDef, IncreaseAilmentDef, Success, Cursed,
		RandStat, Recover, ItemStr, ItemDex, ItemInt,
		ItemLuk, ItemHp, ItemMp, ItemWeaponAtt, ItemMagicAtt,
		ItemWeaponDef, ItemMagicDef, ItemAccuracy, ItemAvoid, ItemJump,
		ItemSpeed, HasMapEffect, MobId, MobChance
	};

	while (dataRow = res.fetch_raw_row()) {
		currentid = atoi(dataRow[ItemId]);

		if (currentid != previousid && previousid != -1) { // Add the items into the cache
			addItemInfo(previousid, item);
			item.cons.mobs.clear();
		}

		item.price = atoi(dataRow[Price]);
		item.maxslot = atoi(dataRow[SlotMax]);
		item.notrade = atob(dataRow[Tradeable]);
		item.quest = atob(dataRow[Quest]);
		item.cons.autoconsume = atob(dataRow[AutoConsume]);
		item.cons.hp = atoi(dataRow[Hp]);
		item.cons.mp = atoi(dataRow[Mp]);
		item.cons.hpr = atoi(dataRow[HpRate]);
		item.cons.mpr = atoi(dataRow[MpRate]);
		item.cons.moveTo = atoi(dataRow[MoveTo]);
		item.cons.ailment = atoi(dataRow[Ailment]);
		item.cons.hasmapeffect = atob(dataRow[HasMapEffect]);

		// Buffs
		item.cons.mcprob = atoi(dataRow[MonCardBuffProb]);
		item.cons.time = atoi(dataRow[Time]);
		item.cons.watk = atoi(dataRow[WeaponAtt]);
		item.cons.matk = atoi(dataRow[MagicAtt]);
		item.cons.avo = atoi(dataRow[Avoid]);
		item.cons.acc = atoi(dataRow[Accuracy]);
		item.cons.wdef = atoi(dataRow[WeaponDef]);
		item.cons.mdef = atoi(dataRow[MagicDef]);
		item.cons.speed = atoi(dataRow[Speed]);
		item.cons.jump = atoi(dataRow[Jump]);
		item.cons.morph = atoi(dataRow[Morph]);
		item.cons.mesoup = atoi(dataRow[MesoUp]);
		item.cons.dropup = atoi(dataRow[DropUp]);

		if (atoi(dataRow[IgnoreWeaponDef]) != 0) {
			item.cons.ignorewdef = item.cons.mcprob;
		}
		if (atoi(dataRow[IgnoreMagicDef]) != 0) {
			item.cons.ignoremdef = item.cons.mcprob;
		}

		string elemattr(dataRow[IncreaseElementDef]);
		if (!(elemattr.find("S") == string::npos))
			item.cons.poisonresist = item.cons.mcprob;
		if (!(elemattr.find("I") == string::npos))
			item.cons.iceresist = item.cons.mcprob;
		if (!(elemattr.find("F") == string::npos))
			item.cons.fireresist = item.cons.mcprob;
		if (!(elemattr.find("L") == string::npos))
			item.cons.lightningresist = item.cons.mcprob;

		string ailmentdef(dataRow[IncreaseAilmentDef]);
		if (!(ailmentdef.find("W") == string::npos))
			item.cons.weaknessdef = item.cons.mcprob;
		if (!(ailmentdef.find("C") == string::npos))
			item.cons.cursedef = item.cons.mcprob;
		if (!(ailmentdef.find("S") == string::npos))
			item.cons.sealdef = item.cons.mcprob;
		if (!(ailmentdef.find("D") == string::npos))
			item.cons.darknessdef = item.cons.mcprob;
		if (!(ailmentdef.find("F") == string::npos))
			item.cons.stundef = item.cons.mcprob;

		// Scrolling
		item.cons.success = atoi(dataRow[Success]);
		item.cons.cursed = atoi(dataRow[Cursed]);
		item.cons.randstat = atob(dataRow[RandStat]);
		item.cons.recover = atob(dataRow[Recover]);
		item.cons.istr = atoi(dataRow[ItemStr]);
		item.cons.idex = atoi(dataRow[ItemDex]);
		item.cons.iint = atoi(dataRow[ItemInt]);
		item.cons.iluk = atoi(dataRow[ItemLuk]);
		item.cons.ihp = atoi(dataRow[ItemHp]);
		item.cons.imp = atoi(dataRow[ItemMp]);
		item.cons.iwatk = atoi(dataRow[ItemWeaponAtt]);
		item.cons.imatk = atoi(dataRow[ItemMagicAtt]);
		item.cons.iwdef = atoi(dataRow[ItemWeaponDef]);
		item.cons.imdef = atoi(dataRow[ItemMagicDef]);
		item.cons.iacc = atoi(dataRow[ItemAccuracy]);
		item.cons.iavo = atoi(dataRow[ItemAvoid]);
		item.cons.ijump = atoi(dataRow[ItemJump]);
		item.cons.ispeed = atoi(dataRow[ItemSpeed]);
		item.cons.ihand = 0;

		// Summoning
		if (dataRow[MobId] != 0) {
			SummonBag summon;
			summon.mobid = atoi(dataRow[MobId]);
			summon.chance = atoi(dataRow[MobChance]);
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
	item_names.clear();
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

	if (item.cons.watk > 0) {
		types.push_back(0x01);
		bytes.push_back(Byte1);
		values.push_back(item.cons.watk);
	}
	if (item.cons.wdef > 0) {
		types.push_back(0x02);
		bytes.push_back(Byte1);
		values.push_back(item.cons.wdef);
	}
	if (item.cons.matk > 0) {
		types.push_back(0x04);
		bytes.push_back(Byte1);
		values.push_back(item.cons.matk);
	}
	if (item.cons.mdef > 0) {
		types.push_back(0x08);
		bytes.push_back(Byte1);
		values.push_back(item.cons.mdef);
	}
	if (item.cons.acc > 0) {
		types.push_back(0x10);
		bytes.push_back(Byte1);
		values.push_back(item.cons.acc);
	}
	if (item.cons.avo > 0) {
		types.push_back(0x20);
		bytes.push_back(Byte1);
		values.push_back(item.cons.avo);
	}
	if (item.cons.speed > 0) {
		types.push_back(0x80);
		bytes.push_back(Byte1);
		values.push_back(item.cons.speed);
	}
	if (item.cons.jump > 0) {
		types.push_back(0x01);
		bytes.push_back(Byte2);
		values.push_back(item.cons.jump);
	}
	if (item.cons.morph > 0) {
		types.push_back(0x02);
		bytes.push_back(Byte5);
		values.push_back(item.cons.morph);
	}
	// Need some buff bytes/types for ALL of the following
	if (item.cons.iceresist > 0) {

	}
	if (item.cons.fireresist > 0) {

	}
	if (item.cons.poisonresist > 0) {

	}
	if (item.cons.lightningresist > 0) {

	}
	if (item.cons.cursedef > 0) {

	}
	if (item.cons.stundef > 0) {

	}
	if (item.cons.weaknessdef > 0) {

	}
	if (item.cons.darknessdef > 0) {

	}
	if (item.cons.sealdef > 0) {

	}
	if (item.cons.ignorewdef > 0) {

	}
	if (item.cons.ignoremdef > 0) {

	}
	if (item.cons.mesoup > 0) {

	}
	if (item.cons.dropup > 0) {
		switch (item.cons.dropup) {
			case 1: // Regular drop rate increase for all items, the only one I can parse at the moment
				break;
			//case 2: // Specific item drop rate increase
			//case 3: // Specific item range (itemid / 10000) increase
		}
	}
	
	if (bytes.size())
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

string ItemDataProvider::getItemName(int32_t itemid) {
	return item_names.find(itemid) != item_names.end() ? item_names[itemid] : "";
}