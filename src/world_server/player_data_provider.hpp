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
#include "common/ip.hpp"
#include "common/party_data.hpp"
#include "common/player_data.hpp"
#include "common/types.hpp"
#include "common/util/id_pool.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace vana {
	class packet_builder;
	class soci::row;

	namespace world_server {
		class login_server_session;
		class world_server_accepted_session;

		class player_data_provider {
		public:
			player_data_provider();

			auto load_data() -> void;
			auto get_channel_connect_packet(packet_builder &builder) -> void;
			auto channel_disconnect(game_channel_id channel) -> void;
			auto send(game_player_id player_id, const packet_builder &builder) -> void;
			auto send(const vector<game_player_id> &player_ids, const packet_builder &builder) -> void;
			auto send(const packet_builder &builder) -> void;

			// Handling
			auto handle_sync(ref_ptr<world_server_accepted_session> session, protocol_sync type, packet_reader &reader) -> void;
			auto handle_sync(ref_ptr<login_server_session> session, protocol_sync type, packet_reader &reader) -> void;
		private:
			auto load_players(game_world_id world_id) -> void;
			auto load_player(game_player_id player_id) -> void;
			auto add_player(const player_data &data) -> void;
			auto send_sync(const packet_builder &builder) const -> void;

			// Handling
			auto handle_player_sync(ref_ptr<world_server_accepted_session> session, packet_reader &reader) -> void;
			auto handle_player_sync(ref_ptr<login_server_session> session, packet_reader &reader) -> void;
			auto handle_party_sync(packet_reader &reader) -> void;
			auto handle_buddy_sync(packet_reader &reader) -> void;

			// Players
			auto remove_pending_player(game_player_id id) -> game_channel_id;
			auto handle_player_connect(game_channel_id channel, packet_reader &reader) -> void;
			auto handle_player_disconnect(game_channel_id channel, packet_reader &reader) -> void;
			auto handle_change_channel_request(ref_ptr<world_server_accepted_session> session, packet_reader &reader) -> void;
			auto handle_change_channel(packet_reader &reader) -> void;
			auto handle_player_update(packet_reader &reader) -> void;
			auto handle_character_created(packet_reader &reader) -> void;
			auto handle_character_deleted(packet_reader &reader) -> void;

			// Parties
			auto handle_create_party(game_player_id player_id) -> void;
			auto handle_party_add(game_player_id player_id, game_party_id party_id) -> void;
			auto handle_party_remove(game_player_id player_id, game_player_id target_id) -> void;
			auto handle_party_leave(game_player_id player_id) -> void;
			auto handle_party_transfer(game_player_id player_id, game_player_id new_leader_id) -> void;

			// Buddies
			auto buddy_invite(packet_reader &reader) -> void;
			auto accept_buddy_invite(packet_reader &reader) -> void;
			auto remove_buddy(packet_reader &reader) -> void;
			auto readd_buddy(packet_reader &reader) -> void;

			vana::util::id_pool<game_party_id> m_party_ids;
			hash_map<game_player_id, game_channel_id> m_channel_switches;
			hash_map<game_party_id, party_data> m_parties;
			hash_map<game_player_id, player_data> m_players;
			case_insensitive_hash_map<player_data *> m_players_by_name;
		};
	}
}