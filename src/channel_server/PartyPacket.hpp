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

#include "common_temp/PacketBuilder.hpp"
#include "common_temp/Types.hpp"
#include <string>

namespace vana {
	namespace channel_server {
		class mystic_door;
		class party;
		class player;

		namespace packets {
			namespace party {
				namespace errors {
					enum : int8_t {
						player_has_party2 = 0x09,
						beginner_cannot_create_party = 0x0A,
						player_has_not_joined_party = 0x0D,
						player_has_party = 0x10,
						party_full = 0x11,
						differing_channel = 0x12,
						unable_to_find = 0x13,
						cannot_kick_user_in_map = 0x19,
						member_must_be_close_to_pass_leader = 0x1C,
						automatic_leader_pass_failed = 0x1D,
						member_must_be_on_same_channel_to_pass_leader = 0x1E,
						gm_cannot_create_party = 0x20,
						unable_to_find2 = 0x21,
					};
				}
				namespace invite_errors {
					enum : int8_t {
						target_blocking_invitations = 0x15,
						target_already_invited_to_some_party = 0x16,
						target_denied_invitation = 0x17,
					};
				}

				PACKET(error, int8_t error);
				PACKET(invite_error, int8_t error, const string &target);
				PACKET(custom_error, const string &error = "");
				PACKET(create_party, vana::channel_server::party *party, ref_ptr<vana::channel_server::player> leader);
				PACKET(join_party, game_map_id target_map_id, vana::channel_server::party *party, const string &player);
				PACKET(leave_party, game_map_id target_map_id, vana::channel_server::party *party, game_player_id player_id, const string &name, bool kicked);
				PACKET(invite_player, vana::channel_server::party *party, const string &inviter);
				PACKET(disband_party, vana::channel_server::party *party);
				PACKET(set_leader, vana::channel_server::party *party, game_player_id new_leader);
				PACKET(silent_update, game_map_id target_map_id, vana::channel_server::party *party);
				PACKET(update_party, game_map_id target_map_id, vana::channel_server::party *party);
				PACKET(update_door, uint8_t zero_based_player_index, ref_ptr<mystic_door> door);
			}
		}
	}
}