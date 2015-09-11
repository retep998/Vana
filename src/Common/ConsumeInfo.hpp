/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "CardMapRangeInfo.hpp"
#include "MorphChanceInfo.hpp"
#include "Types.hpp"
#include <vector>

namespace Vana {
	struct ConsumeInfo {
		bool autoConsume = false;
		bool ignoreWdef = false;
		bool ignoreMdef = false;
		bool party = false;
		bool mouseCancel = true;
		bool ignoreContinent = false;
		bool ghost = false;
		bool barrier = false;
		bool overrideTraction = false;
		bool preventDrown = false;
		bool preventFreeze = false;
		bool mesoUp = false;
		bool dropUp = false;
		bool partyDropUp = false;
		uint8_t effect = 0;
		uint8_t decHunger = 0;
		uint8_t decFatigue = 0;
		uint8_t cp = 0;
		health_t hp = 0;
		health_t mp = 0;
		int16_t hpr = 0;
		int16_t mpr = 0;
		stat_t wAtk = 0;
		stat_t mAtk = 0;
		stat_t avo = 0;
		stat_t acc = 0;
		stat_t wDef = 0;
		stat_t mDef = 0;
		stat_t speed = 0;
		stat_t jump = 0;
		int16_t fireResist = 0;
		int16_t iceResist = 0;
		int16_t lightningResist = 0;
		int16_t poisonResist = 0;
		int16_t curseDef = 0;
		int16_t stunDef = 0;
		int16_t weaknessDef = 0;
		int16_t darknessDef = 0;
		int16_t sealDef = 0;
		int16_t dropUpItemRange = 0;
		uint16_t mcProb = 0;
		item_id_t dropUpItem = 0;
		map_id_t moveTo = 0;
		int32_t ailment = 0;
		seconds_t buffTime = seconds_t{0};
		vector_t<MorphChanceInfo> morphs;
		vector_t<CardMapRangeInfo> mapRanges;
	};
}