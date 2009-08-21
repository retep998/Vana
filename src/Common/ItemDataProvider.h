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
#ifndef ITEMDATA_H
#define ITEMDATA_H

#include "Types.h"
#include <boost/bimap.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <vector>
#include <string>

using boost::bimap;
using std::tr1::unordered_map;
using std::vector;
using std::string;

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
	ConsumeInfo() :
		fireresist(0), lightningresist(0), poisonresist(0), iceresist(0),
		cursedef(0), stundef(0), weaknessdef(0), darknessdef(0), sealdef(0),
		ignorewdef(0), ignoremdef(0)
		{ }

	bool hasmapeffect;
	bool autoconsume;
	bool randstat;
	bool recover;
	int16_t hp;
	int16_t mp;
	int16_t hpr;
	int16_t mpr;
	int16_t watk;
	int16_t matk;
	int16_t avo;
	int16_t acc;
	int16_t wdef;
	int16_t mdef;
	int16_t speed;
	int16_t jump;
	int16_t morph;
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
	int16_t fireresist;
	int16_t iceresist;
	int16_t lightningresist;
	int16_t poisonresist;
	int16_t mesoup;
	int16_t dropup;
	int16_t cursedef;
	int16_t stundef;
	int16_t weaknessdef;
	int16_t darknessdef;
	int16_t sealdef;
	int16_t ignorewdef;
	int16_t ignoremdef;
	uint16_t success;
	uint16_t cursed;
	uint16_t mcprob;
	int32_t moveTo;
	int32_t time;
	int32_t ailment;
	vector<SummonBag> mobs;
	vector<Skillbook> skills;
};

struct ItemInfo {
	uint16_t maxslot;
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
	uint16_t getMaxSlot(int32_t itemid);
	int32_t getCardId(int32_t mobid);
	int32_t getMobId(int32_t cardid);
	string getItemName(int32_t itemid);
	EquipInfo const getEquipInfo(int32_t equipid) { return equips[equipid]; }
	ItemInfo const getItemInfo(int32_t itemid) { return items[itemid]; }

private:
	ItemDataProvider() { }
	static ItemDataProvider *singleton;

	typedef bimap<int32_t, int32_t> card_map;
	typedef card_map::value_type card_info;

	unordered_map<int32_t, EquipInfo> equips;
	unordered_map<int32_t, ItemInfo> items;
	unordered_map<int32_t, string> item_names;
	card_map cards; // Left, cardid; right, mobid

	void addItemInfo(int32_t id, const ItemInfo &item);
};

#endif
