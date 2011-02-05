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
#include "PlayerBuddyList.h"
#include <string>

using std::string;

class Player;
struct PlayerBuddyList::BuddyInvite;

namespace BuddyListPacket {
	namespace Errors {
		enum Errors {
			None = 0x00,
			BuddyListFull = 0x0B,
			TargetListFull = 0x0C,
			AlreadyInList = 0x0D,
			NoGms = 0x0E,
			UserDoesNotExist = 0x0F
		};
	}
	namespace ActionTypes {
		enum Types {
			First = 0x07,
			Add = 0x0A,
			Remove = 0x12,
			Logon = 0x14
		};
	}
	namespace OppositeStatus {
		enum Statusses {
			Registered = 0x00,
			Requested = 0x01,
			Unregistered = 0x02
		};
	}

	void error(Player *player, uint8_t error);
	void update(Player *player, uint8_t type);
	void showSize(Player *player);
	void invitation(Player *player, PlayerBuddyList::BuddyInvite &invite);
	void online(Player *player, int32_t charid, int32_t channel);
}
