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

#include "common/Ip.hpp"
#include "common/InterHelper.hpp"
#include "common/PacketBuilder.hpp"
#include "common/PlayerData.hpp"
#include "common/Types.hpp"
#include <string>
#include <vector>

namespace vana {
	class packet_builder;
	class packet_reader;
	struct rates_config;

	namespace world_server {
		namespace packets {
			namespace interserver {
				PACKET(send_sync_data, function<void(packet_builder &)> build_sync_data);

				namespace config {
					PACKET(set_rates, const rates_config &rates);
					PACKET(scrolling_header, const string &message);
				}
				namespace party {
					PACKET(remove_party_member, game_party_id party_id, game_player_id player_id, bool kicked);
					PACKET(add_party_member, game_party_id party_id, game_player_id player_id);
					PACKET(new_party_leader, game_party_id party_id, game_player_id player_id);
					PACKET(create_party, game_party_id party_id, game_player_id player_id);
					PACKET(disband_party, game_party_id party_id);
				}
				namespace player {
					PACKET(player_change_channel, game_player_id player_id, game_channel_id channel_id, const ip &ip_value, connection_port port);
					PACKET(new_connectable, game_player_id player_id, const ip &ip_value, packet_reader &buffer);
					PACKET(delete_connectable, game_player_id player_id);
					PACKET(update_player, const player_data &data, protocol_update_bits flags);
					PACKET(character_created, const player_data &data);
					PACKET(character_deleted, game_player_id id);
				}
				namespace buddy {
					PACKET(send_buddy_invite, game_player_id invitee_id, game_player_id inviter_id, const string &name);
					PACKET(send_accept_buddy_invite, game_player_id invitee_id, game_player_id inviter_id);
					PACKET(send_buddy_removal, game_player_id list_owner_id, game_player_id removal_id);
					PACKET(send_readd_buddy, game_player_id list_owner_id, game_player_id buddy_id);
				}
			}
		}
	}
}