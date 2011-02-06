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

#include "Types.h"

class Player;

namespace FamePacket {
	namespace Errors {
		enum Errors {
			IncorrectUser = 0x01,
			LevelUnder15 = 0x02,
			AlreadyFamedToday = 0x03,
			FamedThisMonth = 0x04
		};
	}
	void sendFame(Player *player, Player *player2, uint8_t type, int32_t newFame);
	void sendError(Player *player, int32_t reason);
};

