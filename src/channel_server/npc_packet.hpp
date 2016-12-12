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

#include "common/packet_builder.hpp"
#include "common/types.hpp"

namespace vana {
	class packet_reader;
	struct shop_data;
	namespace data {
		namespace type {
			struct npc_spawn_info;
		}
	}

	namespace channel_server {
		class player;
		class move_path;

		namespace packets {
			namespace npc {
				namespace bought_messages {
					enum {
						success = 0,
						not_enough_in_stock = 1,
						not_enough_mesos = 2,
						no_slots = 3
					};
				}
				namespace dialogs {
					enum dialog_options : int8_t {
						normal = 0x00,
						yes_no = 0x01,
						get_text = 0x02,
						get_number = 0x03,
						simple = 0x04,
						question = 0x05,
						quiz = 0x06,
						style = 0x07,
						accept_decline = 0x0C,
						accept_decline_no_exit = 0x0D
					};
				}

				PACKET(show_npc, const data::type::npc_spawn_info &npc, game_map_object id, bool show = true);
				PACKET(control_npc, const data::type::npc_spawn_info &npc, game_map_object id, bool show = true);
				PACKET(move_npc, game_map_object npc_id, uint8_t action1, uint8_t action2, const move_path &path);
				PACKET(animate_npc, game_map_object npc_id, uint8_t action1, uint8_t action2);
				PACKET(show_npc_effect, int32_t index, bool show = false);
				PACKET(bought, uint8_t msg);
				PACKET(show_shop, const shop_data &shop, game_slot_qty rechargeable_bonus);
				PACKET(npc_chat, int8_t type, game_npc_id npc_id, const string &text, bool exclude_text = false);
				auto npc_set_script(const hash_map<game_npc_id, string> scripts) -> vector<packet_builder>;
			}
		}
	}
}