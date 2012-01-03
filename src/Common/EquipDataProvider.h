/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "ItemDataObjects.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>

using std::tr1::unordered_map;

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