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
#ifndef EQUIPDATAPROVIDER_H
#define EQUIPDATAPROVIDER_H

#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <vector>

using std::tr1::unordered_map;
using std::vector;

struct EquipInfo {
	EquipInfo() : tradeblockonequip(false), validslots(0) { }

	bool tradeblockonequip;
	int8_t slots;
	int8_t attackspeed;
	int8_t healing;
	uint8_t tamingmob;
	uint8_t icedamage;
	uint8_t firedamage;
	uint8_t lightningdamage;
	uint8_t poisondamage;
	uint8_t elementaldefault;
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
	int16_t reqstr;
	int16_t reqdex;
	int16_t reqint;
	int16_t reqluk;
	int16_t reqfame;
	int64_t validslots;
	vector<int8_t> validjobs;
	double traction;
};

class EquipDataProvider {
public:
	static EquipDataProvider * Instance() {
		if (singleton == 0)
			singleton = new EquipDataProvider;
		return singleton;
	}
	void loadData();

	EquipInfo * getEquipInfo(int32_t equipid) { return &equips[equipid]; }
private:
	EquipDataProvider() { }
	static EquipDataProvider *singleton;

	void loadEquips();

	unordered_map<int32_t, EquipInfo> equips;
};

#endif
