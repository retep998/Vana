/*
Copyright (C) 2008-2016 Vana Development Team

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

#include "Common/Types.hpp"
#include <vector>

namespace Vana {
	struct EquipInfo {
		bool tradeBlockOnEquip = false;
		int8_t slots = 0;
		int8_t attackSpeed = 0;
		int8_t healing = 0;
		uint8_t tamingMob = 0;
		uint8_t iceDamage = 0;
		uint8_t fireDamage = 0;
		uint8_t lightningDamage = 0;
		uint8_t poisonDamage = 0;
		uint8_t elementalDefault = 0;
		stat_t istr = 0;
		stat_t idex = 0;
		stat_t iint = 0;
		stat_t iluk = 0;
		health_t ihp = 0;
		health_t imp = 0;
		stat_t iwAtk = 0;
		stat_t imAtk = 0;
		stat_t iwDef = 0;
		stat_t imDef = 0;
		stat_t iacc = 0;
		stat_t iavo = 0;
		stat_t ihand = 0;
		stat_t ijump = 0;
		stat_t ispeed = 0;
		stat_t reqStr = 0;
		stat_t reqDex = 0;
		stat_t reqInt = 0;
		stat_t reqLuk = 0;
		fame_t reqFame = 0;
		uint64_t validSlots = 0;
		double traction = 0.;
		vector_t<int8_t> validJobs;
	};
}