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

namespace vana {
	class item;
	class packet_reader;

	namespace channel_server {
		class player;

		namespace inventory_handler {
			auto move_item(ref_ptr<player> player, packet_reader &reader) -> void;
			auto drop_item(ref_ptr<player> player, packet_reader &reader, item *item_value, game_inventory_slot slot, game_inventory inv) -> void;
			auto use_item(ref_ptr<player> player, packet_reader &reader) -> void;
			auto cancel_item(ref_ptr<player> player, packet_reader &reader) -> void;
			auto use_skillbook(ref_ptr<player> player, packet_reader &reader) -> void;
			auto use_chair(ref_ptr<player> player, packet_reader &reader) -> void;
			auto use_item_effect(ref_ptr<player> player, packet_reader &reader) -> void;
			auto upgrade_tomb_effect(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_chair(ref_ptr<player> player, packet_reader &reader) -> void;
			auto use_summon_bag(ref_ptr<player> player, packet_reader &reader) -> void;
			auto use_return_scroll(ref_ptr<player> player, packet_reader &reader) -> void;
			auto use_scroll(ref_ptr<player> player, packet_reader &reader) -> void;
			auto use_buff_item(ref_ptr<player> player, packet_reader &reader) -> void;
			auto use_cash_item(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_rock_functions(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_rock_teleport(ref_ptr<player> player, game_item_id item_id, packet_reader &reader) -> bool;
			auto handle_hammer_time(ref_ptr<player> player) -> void;
			auto handle_reward_item(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_script_item(ref_ptr<player> player, packet_reader &reader) -> void;
		}
	}
}