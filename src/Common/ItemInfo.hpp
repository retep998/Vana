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

#include "Types.hpp"
#include <string>

namespace Vana {
	struct ItemInfo {
		bool timeLimited = false;
		bool noSale = false;
		bool karmaScissors = false;
		bool expireOnLogout = false;
		bool blockPickup = false;
		bool noTrade = false;
		bool quest = false;
		bool cash = false;
		player_level_t minLevel = 0;
		player_level_t maxLevel = 0;
		skill_level_t makerLevel = 0;
		slot_qty_t maxSlot = 0;
		mesos_t price = 0;
		int32_t maxObtainable = 0;
		experience_t exp = 0;
		npc_id_t npc = 0;
		mesos_t mesos = 0;
		string_t name;
	};
}