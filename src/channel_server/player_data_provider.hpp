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
#include "common/player_data.hpp"
#include "common/types.hpp"
#include "common/util/shared_array.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace vana {
	class packet_builder;
	class packet_reader;

	namespace channel_server {
		class party;
		class player;

		struct connecting_player {
			connecting_player() : connect_ip{0} { }

			ip connect_ip;
			time_point connect_time;
			game_map_id map_id = -1;
			string portal;
			uint16_t packet_size;
			vana::util::shared_array<unsigned char> held_packet;
		};

		class player_data_provider {
		public:
			auto handle_sync(protocol_sync type, packet_reader &reader) -> void;

			// Online players
			auto add_player(ref_ptr<player> player) -> void;
			auto remove_player(ref_ptr<player> player) -> void;
			auto update_player_level(ref_ptr<player> player) -> void;
			auto update_player_map(ref_ptr<player> player) -> void;
			auto update_player_job(ref_ptr<player> player) -> void;
			auto get_player(game_player_id id) -> ref_ptr<player>;
			auto get_player(const string &name) -> ref_ptr<player>;
			auto run(function<void(ref_ptr<player>)> func) -> void;
			auto send(game_player_id player_id, const packet_builder &builder) -> void;
			auto send(const vector<game_player_id> &player_ids, const packet_builder &builder) -> void;
			auto send(const packet_builder &builder) -> void;
			auto add_follower(ref_ptr<player> follower, ref_ptr<player> target) -> void;
			auto stop_following(ref_ptr<player> follower) -> void;
			auto disconnect() -> void;

			// Player data
			auto get_player_data(game_player_id id) const -> const player_data * const;
			auto get_player_data_by_name(const string &name) const -> const player_data * const;

			// Parties
			auto get_party(game_party_id id) -> party *;

			// Chat
			auto handle_group_chat(int8_t chat_type, game_player_id player_id, const vector<game_player_id> &receivers, const game_chat &chat) -> void;
			auto handle_gm_chat(ref_ptr<player> player, const game_chat &chat) -> void;

			// Connections
			auto check_player(game_player_id id, const ip &ip, bool &has_packet) const -> result;
			auto get_packet(game_player_id id) const -> packet_reader;
			auto player_established(game_player_id id) -> void;
		private:
			auto parse_channel_connect_packet(packet_reader &reader) -> void;

			auto handle_player_sync(packet_reader &reader) -> void;
			auto handle_party_sync(packet_reader &reader) -> void;
			auto handle_buddy_sync(packet_reader &reader) -> void;

			auto send_sync(const packet_builder &builder) const -> void;
			auto add_player_data(const player_data &data) -> void;
			auto handle_character_created(packet_reader &reader) -> void;
			auto handle_character_deleted(packet_reader &reader) -> void;
			auto handle_change_channel(packet_reader &reader) -> void;
			auto handle_new_connectable(packet_reader &reader) -> void;
			auto handle_delete_connectable(packet_reader &reader) -> void;
			auto handle_update_player(packet_reader &reader) -> void;

			auto handle_create_party(game_party_id id, game_player_id leader_id) -> void;
			auto handle_disband_party(game_party_id id) -> void;
			auto handle_party_transfer(game_party_id id, game_player_id leader_id) -> void;
			auto handle_party_remove(game_party_id id, game_player_id player_id, bool kicked) -> void;
			auto handle_party_add(game_party_id id, game_player_id player_id) -> void;

			auto buddy_invite(packet_reader &reader) -> void;
			auto accept_buddy_invite(packet_reader &reader) -> void;
			auto remove_buddy(packet_reader &reader) -> void;
			auto readd_buddy(packet_reader &reader) -> void;

			auto new_player(game_player_id id, const ip &ip, packet_reader &reader) -> void;

			const static uint32_t max_connection_milliseconds = 5000;

			hash_set<game_player_id> m_gm_list;
			hash_map<game_player_id, player_data> m_player_data;
			hash_map<game_player_id, vector<ref_ptr<player>>> m_followers;
			case_insensitive_hash_map<player_data *> m_player_data_by_name;
			hash_map<game_party_id, ref_ptr<party>> m_parties;
			hash_map<game_player_id, ref_ptr<player>> m_players;
			case_insensitive_hash_map<ref_ptr<player>> m_players_by_name;
			hash_map<game_player_id, connecting_player> m_connections;
		};
	}
}