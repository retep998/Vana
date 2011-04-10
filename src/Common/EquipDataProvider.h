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
#pragma once

#include "Item.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <vector>

using std::tr1::unordered_map;
using std::vector;

struct EquipInfo {
	EquipInfo() : tradeBlockOnEquip(false), validSlots(0) { }

	bool tradeBlockOnEquip;
	int8_t slots;
	int8_t attackSpeed;
	int8_t healing;
	uint8_t tamingMob;
	uint8_t iceDamage;
	uint8_t fireDamage;
	uint8_t lightningDamage;
	uint8_t poisonDamage;
	uint8_t elementalDefault;
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
	int16_t reqStr;
	int16_t reqDex;
	int16_t reqInt;
	int16_t reqLuk;
	int16_t reqFame;
	int64_t validSlots;
	vector<int8_t> validJobs;
	double traction;
};

class EquipDataProvider {
public:
	static EquipDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new EquipDataProvider;
		return singleton;
	}
	void loadData();

	void setEquipStats(Item *equip, bool random);
	bool canEquip(int32_t itemId, int16_t job, int16_t str, int16_t dex, int16_t intt, int16_t luk, int16_t fame);
	bool validSlot(int32_t equipId, int16_t target);
	int8_t getSlots(int32_t equipId) { return getEquipInfo(equipId)->slots; }
private:
	EquipDataProvider() { }
	static EquipDataProvider *singleton;

	void loadEquips();
	int16_t getStatVariance(uint16_t amount);
	int16_t getRandomStat(int16_t equipAmount, uint16_t variance);
	EquipInfo * getEquipInfo(int32_t equipId) { return &m_equipInfo[equipId]; }

	unordered_map<int32_t, EquipInfo> m_equipInfo;
};