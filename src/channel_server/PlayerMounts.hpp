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

#include "common/types.hpp"
#include <unordered_map>

namespace vana {
	class packet_builder;

	namespace channel_server {
		class player;

		struct mount_data {
			int16_t exp = 0;
			int8_t tiredness = 0;
			int8_t level = 0;
		};

		class player_mounts {
			NONCOPYABLE(player_mounts);
			NO_DEFAULT_CONSTRUCTOR(player_mounts);
		public:
			player_mounts(player *player);

			auto save() -> void;
			auto load() -> void;

			auto mount_info_packet(packet_builder &builder) -> void;
			auto mount_info_map_spawn_packet(packet_builder &builder) -> void;

			auto get_current_mount() const -> game_item_id { return m_current_mount; }
			auto get_current_exp() -> int16_t;
			auto get_current_level() -> int8_t;
			auto get_current_tiredness() -> int8_t;
			auto set_current_mount(game_item_id id) -> void { m_current_mount = id; }
			auto set_current_exp(int16_t exp) -> void;
			auto set_current_level(int8_t level) -> void;
			auto set_current_tiredness(int8_t tiredness) -> void;

			auto add_mount(game_item_id id) -> void;

			auto get_mount_exp(game_item_id id) -> int16_t;
			auto get_mount_level(game_item_id id) -> int8_t;
			auto get_mount_tiredness(game_item_id id) -> int8_t;
		private:
			game_item_id m_current_mount = 0;
			player *m_player = nullptr;
			hash_map<game_item_id, mount_data> m_mounts;
		};
	}
}