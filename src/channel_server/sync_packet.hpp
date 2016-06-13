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

#include "common/inter_helper.hpp"
#include "common/packet_builder.hpp"
#include "common/player_data.hpp"
#include "common/types.hpp"
#include <string>
#include <vector>

namespace vana {
	namespace config {
		struct rates;
	}

	namespace channel_server {
		class player;

		namespace packets {
			namespace interserver {
				namespace config {
					PACKET(scrolling_header, const string &message);
					PACKET(reset_rates, int32_t flags);
					PACKET(modify_rates, const vana::config::rates &rates);
				}
				namespace player {
					PACKET(change_channel, ref_ptr<vana::channel_server::player> info, game_channel_id channel);
					PACKET(connectable_established, game_player_id player_id);
					PACKET(connect, const player_data &player, bool first_connect);
					PACKET(disconnect, game_player_id player_id);
					PACKET(update_player, const player_data &player, protocol_update_bits flags);
				}
				namespace party {
					PACKET(sync, int8_t type, game_player_id player_id, int32_t target = 0);
				}
				namespace buddy {
					PACKET(buddy_invite, game_player_id inviter_id, game_player_id invitee_id);
					PACKET(accept_buddy_invite, game_player_id invitee_id, game_player_id inviter_id);
					PACKET(remove_buddy, game_player_id list_owner_id, game_player_id removal_id);
					PACKET(readd_buddy, game_player_id list_owner_id, game_player_id buddy_id);
				}
			}
		}
	}
}