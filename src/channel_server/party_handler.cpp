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
#include "party_handler.hpp"
#include "common/constant/party.hpp"
#include "common/inter_helper.hpp"
#include "common/packet_reader.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/party.hpp"
#include "channel_server/party_packet.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/sync_packet.hpp"

namespace vana {
namespace channel_server {

auto party_handler::handle_request(ref_ptr<player> player, packet_reader &reader) -> void {
	int8_t type = reader.get<int8_t>();
	switch (type) {
		case party_actions::create:
		case party_actions::leave:
			channel_server::get_instance().send_world(packets::interserver::party::sync(type, player->get_id()));
			break;
		case party_actions::join: {
			game_party_id party_id = reader.get<game_party_id>();
			if (party *party = channel_server::get_instance().get_player_data_provider().get_party(party_id)) {
				if (party->get_members_count() == constant::party::max_members) {
					player->send(packets::party::error(packets::party::errors::party_full));
				}
				else {
					channel_server::get_instance().send_world(packets::interserver::party::sync(type, player->get_id(), party_id));
				}
			}
			break;
		}
		case party_actions::expel:
		case party_actions::set_leader: {
			channel_server::get_instance().send_world(packets::interserver::party::sync(type, player->get_id(), reader.get<int32_t>()));
			break;
		}
		case party_actions::invite: {
			string inv_name = reader.get<string>();
			if (player->get_party() == nullptr) {
				// ??
				return;
			}
			if (auto invitee = channel_server::get_instance().get_player_data_provider().get_player(inv_name)) {
				if (invitee->get_party() != nullptr) {
					player->send(packets::party::error(packets::party::errors::player_has_party));
				}
				else {
					invitee->send(packets::party::invite_player(player->get_party(), player->get_name()));
				}
			}
			else {
				player->send(packets::party::error(packets::party::errors::differing_channel));
			}
			break;
		}
	}
}

}
}