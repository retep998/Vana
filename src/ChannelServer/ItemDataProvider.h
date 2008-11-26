/*
Copyright (C) 2008 Vana Development Team

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
#ifndef ITEMDATA_H
#define ITEMDATA_H

#include "Types.h"
#include <boost/unordered_map.hpp>
#include <vector>

using boost::unordered_map;
using std::vector;

class Equip;

struct EquipInfo {
	int8_t slots;
	int32_t price;
	int16_t istr;
	int16_t idex;
	int16_t iint;
	int16_t iluk;
	int16_t ihp;
	int16_t imp;
	int16_t iwatk;
	int16_t imatk;
	int16_t iwdef;
	int16_t imdef;
	int16_t iacc;
	int16_t iavo;
	int16_t ihand;
	int16_t ijump;
	int16_t ispeed;
	int16_t tamingmob;
	bool onlyone;
	bool notrade;
	bool quest;
};

struct SummonBag {
	int32_t mobid;
	uint32_t chance;
};

struct Skillbook {
	int32_t skillid;
	uint8_t reqlevel;
	uint8_t maxlevel;
};

struct ConsumeInfo {
	int16_t hp;
	int16_t mp;
	int16_t hpr;
	int16_t mpr;
	int32_t moveTo;
	int32_t time;
	int16_t watk;
	int16_t matk;
	int16_t avo;
	int16_t acc;
	int16_t wdef;
	int16_t mdef;
	int16_t speed;
	int16_t jump;
	int16_t morph;
	int16_t success;
	int16_t cursed;
	int16_t istr;
	int16_t idex;
	int16_t iint;
	int16_t iluk;
	int16_t ihp;
	int16_t imp;
	int16_t iwatk;
	int16_t imatk;
	int16_t iwdef;
	int16_t imdef;
	int16_t iacc;
	int16_t iavo;
	int16_t ihand;
	int16_t ijump;
	int16_t ispeed;
	uint8_t types[8];
	vector<int16_t> vals;
	vector<SummonBag> mobs;
	vector<Skillbook> skills;
};

struct ItemInfo {
	int16_t maxslot;
	int32_t price;
	bool notrade;
	bool quest;
	ConsumeInfo cons;
};

class ItemDataProvider {
public:
	static ItemDataProvider * Instance() {
		if (singleton == 0)
			singleton = new ItemDataProvider;
		return singleton;
	}
	void loadData();
	bool itemExists(int32_t id);
	int32_t getPrice(int32_t itemid);
	int16_t getMaxslot(int32_t itemid);
	EquipInfo const getEquipInfo(int32_t equipid) {
		return equips[equipid];
	}
	ItemInfo const getItemInfo(int32_t itemid) {
		return items[itemid];
	}

private:
	ItemDataProvider() { }
	static ItemDataProvider *singleton;

	unordered_map<int32_t, EquipInfo> equips;
	unordered_map<int32_t, ItemInfo> items;
	void addItemInfo(int32_t id, ItemInfo item);
};

#endif
