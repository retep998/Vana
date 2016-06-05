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
#include <functional>
#include <map>
#include <string>

namespace vana {
	class packet_builder;
	class packet_reader;

	namespace login_server {
		class channel;
		class login_server_accepted_session;
		class user;
		class world;

		class worlds {
		public:
			auto channel_select(ref_ptr<user> user_value, packet_reader &reader) -> void;
			auto select_world(ref_ptr<user> user_value, packet_reader &reader) -> void;
			auto show_world(ref_ptr<user> user_value) -> void;
			auto send(game_world_id id, const packet_builder &builder) -> void;
			auto send(const vector<game_world_id> &worlds, const packet_builder &builder) -> void;
			auto send(const packet_builder &builder) -> void;

			auto add_world(world *world_value) -> void;
			auto calculate_player_load(world *world_value) -> void;
			auto run_function(function<bool (world *)> func) -> void;
			auto set_event_messages(const string &message) -> void;

			auto get_world(game_world_id id) -> world *;

			// Inter-server
			auto add_world_server(ref_ptr<login_server_accepted_session> session) -> optional<game_world_id>;
			auto add_channel_server(ref_ptr<login_server_accepted_session> session) -> optional<game_world_id>;
		private:
			ord_map<game_world_id, world *> m_worlds;
		};
	}
}