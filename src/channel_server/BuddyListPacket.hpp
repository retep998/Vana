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

#include "common/PacketBuilder.hpp"
#include "common/Types.hpp"
#include <string>

namespace vana {
	namespace channel_server {
		class player;
		struct buddy_invite;

		namespace packets {
			namespace buddy {
				namespace errors {
					enum errors : int8_t {
						none = 0x00,
						buddy_list_full = 0x0B,
						target_list_full = 0x0C,
						already_in_list = 0x0D,
						no_gms = 0x0E,
						user_does_not_exist = 0x0F
					};
				}
				namespace action_types {
					enum types : int8_t {
						first = 0x07,
						add = 0x0A,
						remove = 0x12,
						logon = 0x14
					};
				}
				namespace opposite_status {
					enum statuses : int8_t {
						registered = 0x00,
						requested = 0x01,
						unregistered = 0x02
					};
				}

				PACKET(error, uint8_t error);
				PACKET(update, ref_ptr<player> player, uint8_t type);
				PACKET(show_size, ref_ptr<player> player);
				PACKET(invitation, const buddy_invite &invite);
				PACKET(online, game_player_id char_id, game_channel_id channel, bool cash_shop);
			}
		}
	}
}