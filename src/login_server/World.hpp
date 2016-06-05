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

#include "common/types_temp.hpp"
#include "common/world_config.hpp"
#include "login_server/Channel.hpp"
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace vana {
	class packet_builder;

	namespace login_server {
		class channel;
		class login_server_accepted_session;

		class world {
			NONCOPYABLE(world);
		public:
			world() = default;

			auto set_connected(bool connected) -> void;
			auto set_id(game_world_id id) -> void;
			auto set_port(connection_port port) -> void;
			auto set_player_load(int32_t load) -> void;
			auto set_session(ref_ptr<login_server_accepted_session> session) -> void;
			auto set_configuration(const world_config &config) -> void;
			auto set_event_message(const string &message) -> void;
			auto run_channel_function(function<void (channel *)> func) -> void;
			auto clear_channels() -> void;
			auto remove_channel(game_channel_id id) -> void;
			auto add_channel(game_channel_id id, channel *chan) -> void;
			auto send(const packet_builder &builder) -> void;

			auto is_connected() const -> bool;
			auto get_id() const -> optional<game_world_id>;
			auto get_ribbon() const -> int8_t;
			auto get_port() const -> connection_port;
			auto get_random_channel() const -> game_channel_id;
			auto get_max_channels() const -> game_channel_id;
			auto get_player_load() const -> int32_t;
			auto get_max_player_load() const -> int32_t;
			auto match_subnet(const ip &test) -> ip;
			auto get_channel_count() const -> game_channel_id;
			auto get_name() const -> string;
			auto get_event_message() const -> string;
			auto get_channel(game_channel_id id) -> channel *;
			auto get_config() const -> const world_config &;
		private:
			bool m_connected = false;
			optional<game_world_id> m_id;
			connection_port m_port = 0;
			int32_t m_player_load = 0;
			ref_ptr<login_server_accepted_session> m_session;
			world_config m_config;
			hash_map<game_channel_id, ref_ptr<channel>> m_channels;
		};
	}
}