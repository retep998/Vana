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

#include "Common/PacketBuilder.hpp"
#include "Common/Types.hpp"
#include <string>

namespace Vana {
	namespace ChannelServer {
		class Player;
		struct BuddyInvite;

		namespace Packets {
			namespace Buddy {
				namespace Errors {
					enum Errors : int8_t {
						None = 0x00,
						BuddyListFull = 0x0B,
						TargetListFull = 0x0C,
						AlreadyInList = 0x0D,
						NoGms = 0x0E,
						UserDoesNotExist = 0x0F
					};
				}
				namespace ActionTypes {
					enum Types : int8_t {
						First = 0x07,
						Add = 0x0A,
						Remove = 0x12,
						Logon = 0x14
					};
				}
				namespace OppositeStatus {
					enum Statuses : int8_t {
						Registered = 0x00,
						Requested = 0x01,
						Unregistered = 0x02
					};
				}

				PACKET(error, uint8_t error);
				PACKET(update, ref_ptr_t<Player> player, uint8_t type);
				PACKET(showSize, ref_ptr_t<Player> player);
				PACKET(invitation, const BuddyInvite &invite);
				PACKET(online, player_id_t charId, channel_id_t channel, bool cashShop);
			}
		}
	}
}