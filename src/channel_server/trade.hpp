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
#include <array>
#include <memory>

namespace vana {
	class item;

	namespace channel_server {
		class player;

		struct trade_info {
			trade_info()
			{
				for (game_trade_slot i = 0; i < trade_size; i++) {
					items[i] = nullptr;
				}
			}

			const static game_trade_slot trade_size = 9;

			bool accepted = false;
			game_trade_slot count = 0;
			vana::util::meso_inventory mesos;
			array<item *, trade_size> items;
		};

		class active_trade {
			NONCOPYABLE(active_trade);
			NO_DEFAULT_CONSTRUCTOR(active_trade);
		public:
			active_trade(ref_ptr<player> starter, ref_ptr<player> receiver, game_trade_id id);

			auto get_id() const -> game_trade_id { return m_id; }
			auto get_sender_trade() const -> trade_info * { return m_sender.get(); }
			auto get_receiver_trade() const -> trade_info * { return m_receiver.get(); }

			auto get_sender() -> ref_ptr<player>;
			auto get_receiver() -> ref_ptr<player>;

			auto both_can_trade() -> bool;
			auto both_accepted() -> bool;
			auto return_trade() -> void;
			auto swap_trade() -> void;
			auto accept(trade_info *unit) -> void;
			auto add_mesos(ref_ptr<player> holder, trade_info *unit, game_mesos amount) -> game_mesos;
			auto add_item(ref_ptr<player> holder, trade_info *unit, item *value, game_trade_slot trade_slot, game_inventory_slot inventory_slot, game_inventory inventory, game_slot_qty amount) -> item *;
			auto is_item_in_slot(trade_info *unit, game_trade_slot trade_slot) -> bool {
				return trade_slot > trade_info::trade_size ? true : unit->items[trade_slot - 1] != nullptr;
			}
		private:
			auto can_trade(ref_ptr<player> target, trade_info *unit) -> bool;
			auto give_items(ref_ptr<player> target, trade_info *unit) -> void;
			auto give_mesos(ref_ptr<player> player, trade_info *info, bool traded = false) -> void;

			game_trade_id m_id = 0;
			game_player_id m_sender_id = 0;
			game_player_id m_receiver_id = 0;
			owned_ptr<trade_info> m_sender;
			owned_ptr<trade_info> m_receiver;
		};
	}
}