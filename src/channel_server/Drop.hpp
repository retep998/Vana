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

#include "common/Item.hpp"
#include "common/Point.hpp"
#include "common/Types.hpp"

namespace vana {
	class item;

	namespace channel_server {
		class map;
		class player;

		class drop {
			NO_DEFAULT_CONSTRUCTOR(drop);
			NONCOPYABLE(drop);
		public:
			drop(game_map_id map_id, game_mesos mesos, const point &pos, game_player_id owner, bool player_drop = false);
			drop(game_map_id map_id, const item &item, const point &pos, game_player_id owner, bool player_drop = false);

			auto set_quest(game_quest_id quest_id) -> void { m_quest_id = quest_id; }
			auto set_tradeable(bool is_trade) -> void { m_tradeable = is_trade; }
			auto set_item_amount(game_slot_qty amount) -> void { m_item.set_amount(amount); }
			auto set_id(game_map_object id) -> void { m_id = id; }
			auto set_time(int32_t time) -> void { m_time = time; }
			auto set_owner(int32_t owner) -> void { m_owner = owner; }
			auto set_pos(point pos) -> void { m_pos = pos; }
			auto set_type(int8_t t) -> void { m_type = t; }
			auto set_dropped_at_time(time_point time) -> void { m_dropped_at_time = time; }

			auto get_type() const -> int8_t { return m_type; }
			auto get_quest() const -> game_quest_id { return m_quest_id; }
			auto get_id() const -> game_map_object { return m_id; }
			auto get_time() const -> int32_t { return m_time; }
			auto get_owner() const -> int32_t { return m_owner; }
			auto get_map_id() const -> game_map_id { return m_map_id; }
			auto is_player_drop() const -> bool { return m_player_drop; }
			auto is_mesos() const -> bool { return m_mesos > 0; }
			auto is_quest() const -> bool { return m_quest_id > 0; }
			auto is_tradeable() const -> bool { return m_tradeable; }
			auto get_pos() const -> point { return m_pos; }
			auto get_item() const -> item { return m_item; }
			auto get_dropped_at_time() const -> time_point { return m_dropped_at_time; }

			auto get_amount() -> game_slot_qty;
			auto get_object_id() -> int32_t;

			auto do_drop(const point &origin) -> void;
			auto show_drop(ref_ptr<player> player) -> void;
			auto take_drop(ref_ptr<player> player, game_pet_id pet_id) -> void;
			auto remove_drop(bool show_packet = true) -> void;

			auto get_map() const -> map *;

			static const int8_t explosive = 3;
			static const int8_t free_for_all = 2;
			static const int8_t party = 1;
			static const int8_t normal = 0;
		private:
			bool m_player_drop = false;
			bool m_tradeable = true;
			int8_t m_type = drop::normal;
			game_quest_id m_quest_id = 0;
			int32_t m_owner = 0;
			game_map_id m_map_id = 0;
			game_map_object m_id = 0;
			game_mesos m_mesos = 0;
			int32_t m_time = 0;
			time_point m_dropped_at_time;
			point m_pos;
			item m_item;
		};
	}
}