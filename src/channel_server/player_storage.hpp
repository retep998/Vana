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
#include "common/util/meso_inventory.hpp"
#include <vector>

namespace vana {
	class item;

	namespace channel_server {
		class player;

		class player_storage {
			NONCOPYABLE(player_storage);
			NO_DEFAULT_CONSTRUCTOR(player_storage);
		public:
			player_storage(ref_ptr<player> player);
			~player_storage();

			auto set_slots(game_storage_slot slots) -> void;
			auto add_item(item *item) -> void;
			auto take_item(game_storage_slot slot) -> void;
			auto set_mesos(game_mesos mesos) -> void;
			auto modify_mesos(game_mesos mod) -> vana::util::meso_modify_result;
			auto can_modify_mesos(game_mesos mesos) const -> stack_result;

			auto get_slots() const -> game_storage_slot { return m_slots; }
			auto get_num_items() const -> game_storage_slot { return static_cast<game_storage_slot>(m_items.size()); }
			auto get_num_items(game_inventory inv) -> game_storage_slot;
			auto get_mesos() const -> game_mesos { return m_mesos.get_mesos(); }
			auto is_full() const -> bool { return m_items.size() == m_slots; }
			auto get_item(game_storage_slot slot) const -> item * {
				if (slot < m_items.size()) {
					return m_items[slot];
				}
				return nullptr;
			}

			auto load() -> void;
			auto save() -> void;
		private:
			game_storage_slot m_slots = 0;
			int32_t m_char_slots = 0;
			vana::util::meso_inventory m_mesos;
			vector<item *> m_items;
			view_ptr<player> m_player;
		};
	}
}