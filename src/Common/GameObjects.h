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
#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include "Types.h"
#include <string>

using std::string;

struct Item {
	Item() : id(0), amount(1), hammers(0), slots(0), scrolls(0), flags(0), istr(0), idex(0), iint(0), iluk(0), ihp(0),
		imp(0), iwatk(0), imatk(0), iwdef(0), imdef(0), iacc(0), iavo(0), ihand(0), ijump(0), ispeed(0), petid(0), name("") { }

	Item(int32_t itemid, int16_t amount) : id(itemid), amount(amount), hammers(0), slots(0), scrolls(0), flags(0), istr(0), idex(0), iint(0), iluk(0), ihp(0), imp(0),
		iwatk(0), imatk(0), iwdef(0), imdef(0), iacc(0), iavo(0), ihand(0), ijump(0), ispeed(0), petid(0), name("") { }

	Item(int32_t equipid, bool random);

	Item(Item *item) {
		id = item->id;
		amount = item->amount;
		hammers = item->hammers;
		slots = item->slots;
		scrolls = item->scrolls;
		istr = item->istr;
		idex = item->idex;
		iint = item->iint;
		iluk = item->iluk;
		ihp = item->ihp;
		imp = item->imp;
		iwatk = item->iwatk;
		imatk = item->imatk;
		iwdef = item->iwdef;
		imdef = item->imdef;
		iacc = item->iacc;
		iavo = item->iavo;
		ihand = item->ihand;
		ijump = item->ijump;
		ispeed = item->ispeed;
		petid = item->petid;
		name = item->name;
		flags = item->flags;
	}
	int32_t id;
	int32_t hammers;
	int16_t amount;
	int8_t slots;
	int8_t scrolls;
	int8_t flags;
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
	int32_t petid;
	string name;
};

#endif