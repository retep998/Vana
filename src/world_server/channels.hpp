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

#include "common/external_ip.hpp"
#include "common/ip.hpp"
#include "common/types.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace vana {
	class packet_builder;

	namespace world_server {
		class channel;
		class world_server_accepted_session;

		class channels {
		public:
			auto register_channel(ref_ptr<world_server_accepted_session> session, game_channel_id channel_id, const ip &channel_ip, const ip_matrix &ext_ip, connection_port port) -> void;
			auto remove_channel(game_channel_id channel_id) -> void;
			auto get_channel(game_channel_id num) -> channel *;
			auto increase_population(game_channel_id channel_id) -> void;
			auto decrease_population(game_channel_id channel_id) -> void;
			auto get_first_available_channel_id() -> game_channel_id;
			auto disconnect() -> void;
			auto send(game_channel_id channel_id, const packet_builder &builder) -> void;
			auto send(const vector<game_channel_id> &channels, const packet_builder &builder) -> void;
			auto send(const packet_builder &builder) -> void;
		private:
			hash_map<game_channel_id, ref_ptr<channel>> m_channels;
		};
	}
}