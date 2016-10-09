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

		struct monster_card {
			monster_card() = default;
			monster_card(game_item_id id, uint8_t level) : id{id}, level{level} { }

			game_item_id id = 0;
			uint8_t level = 0;
		};

		class player_monster_book {
			NONCOPYABLE(player_monster_book);
			NO_DEFAULT_CONSTRUCTOR(player_monster_book);
		public:
			player_monster_book(ref_ptr<player> player);

			auto load() -> void;
			auto save() -> void;
			auto connect_packet(packet_builder &builder) -> void;
			auto info_packet(packet_builder &builder) -> void;

			auto add_card(game_item_id item_id, uint8_t level = 1, bool initial_load = false) -> bool;
			auto calculate_level() -> void;
			auto set_cover(int32_t new_cover) -> void { m_cover = new_cover; }

			auto get_card(game_item_id card_id) -> monster_card *;
			auto get_card_level(game_item_id card_id) -> uint8_t;
			auto get_specials() const -> int32_t { return m_special_count; }
			auto get_normals() const -> int32_t { return m_normal_count; }
			auto get_size() const -> int32_t { return static_cast<int32_t>(m_cards.size()); }
			auto get_level() const -> int32_t { return m_level; }
			auto get_cover() const -> int32_t { return m_cover; }
			auto is_full(game_item_id card_id) -> bool;
		private:
			int32_t m_special_count = 0;
			int32_t m_normal_count = 0;
			int32_t m_level = 1;
			int32_t m_cover = 0;
			view_ptr<player> m_player;
			hash_map<game_item_id, monster_card> m_cards;
		};
	}
}