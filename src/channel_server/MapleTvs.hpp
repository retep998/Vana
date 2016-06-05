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
#include "common/TimerContainerHolder.hpp"
#include "common/Types.hpp"
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

namespace vana {
	namespace channel_server {
		class map;
		class player;

		struct maple_tv_message {
			bool has_receiver = false;
			int32_t time = 0;
			game_item_id megaphone_id = 0;
			game_player_id sender_id = 0;
			uint32_t counter = 0;
			string msg1;
			string msg2;
			string msg3;
			string msg4;
			string msg5;
			string send_name;
			string recv_name;
			packet_builder recv_display;
			packet_builder send_display;
		};

		class maple_tvs : public timer_container_holder {
		public:
			auto add_map(map *map) -> void;

			auto add_message(ref_ptr<player> sender, ref_ptr<player> receiver, const string &msg, const string &msg2, const string &msg3, const string &msg4, const string &msg5, game_item_id megaphone_id, int32_t time) -> void;
			auto is_maple_tv_map(game_map_id id) const -> bool;
			auto has_message() const -> bool;
			auto get_counter() -> uint32_t;
			auto get_current_message() const -> const maple_tv_message &;
			auto get_message_time() const -> seconds;
		private:
			auto parse_buffer() -> void;
			auto send(const packet_builder &builder) -> void;

			bool m_has_message = false;
			uint32_t m_counter = 0;
			maple_tv_message m_current_message;
			queue<maple_tv_message> m_buffer;
			hash_map<game_map_id, map *> m_maps;
		};
	}
}