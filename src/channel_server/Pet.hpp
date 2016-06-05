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

#include "common/point_temp.hpp"
#include "common/types_temp.hpp"
#include "channel_server/MovableLife.hpp"
#include <soci.h>
#include <string>

namespace vana {
	class item;

	namespace channel_server {
		class player;

		class pet : public movable_life {
			NONCOPYABLE(pet);
			NO_DEFAULT_CONSTRUCTOR(pet);
		public:
			pet(player *player, item *item);
			pet(player *player, item *item, const soci::row &row);

			auto summon(int8_t index) -> void { m_index = index; }
			auto desummon() -> void { m_index.reset(); }
			auto set_inventory_slot(int8_t slot) -> void { m_inventory_slot = slot; }
			auto set_name(const string &name) -> void;
			auto modify_fullness(int8_t offset, bool send_packet = true) -> void;
			auto add_closeness(int16_t amount) -> void;

			auto get_index() const -> opt_int8_t { return m_index; }
			auto get_level() const -> int8_t { return m_level; }
			auto get_inventory_slot() const -> int8_t { return m_inventory_slot; }
			auto get_fullness() const -> int8_t { return m_fullness; }
			auto get_closeness() const -> int16_t { return m_closeness; }
			auto get_item_id() const -> game_item_id { return m_item_id; }
			auto get_id() const -> game_pet_id { return m_id; }
			auto is_summoned() const -> bool { return m_index.is_initialized(); }
			auto get_name() -> string { return m_name; }
			auto get_pos() const -> point override { return point{m_pos.x, m_pos.y - 1}; }
			auto has_name_tag() const -> bool;
			auto has_quote_item() const -> bool;

			auto start_timer() -> void;
		private:
			auto initialize_pet(const soci::row &row) -> void;
			auto level_up() -> void;

			opt_int8_t m_index;
			int8_t m_level = 1;
			int8_t m_fullness = stats::max_fullness;
			int8_t m_inventory_slot = 0;
			int16_t m_closeness = 0;
			game_item_id m_item_id = 0;
			game_pet_id m_id = 0;
			item *m_item = nullptr;
			player *m_player = nullptr;
			string m_name;
		};
	}
}