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

#include "common/ip.hpp"
#include "common/packet_builder.hpp"
#include "common/types.hpp"
#include <string>

namespace vana {
	class packet_builder;

	namespace channel_server {
		class key_maps;
		class player;
		class skill_macros;

		namespace packets {
			namespace player {
				namespace block_messages {
					enum messages : int8_t {
						cannot_go = 0x01,
						no_cash_shop = 0x02,
						mts_unavailable = 0x03,
						mts_user_limit = 0x04,
						level_too_low = 0x05
					};
				}
				namespace notice_types {
					enum types : int8_t {
						notice = 0x00,
						box = 0x01,
						red = 0x05,
						blue = 0x06
					};
				}

				PACKET(connect_data, ref_ptr<vana::channel_server::player> player);
				PACKET(show_keys, key_maps *keymaps);
				PACKET(show_skill_macros, skill_macros *macros);
				PACKET(update_stat, int32_t update_bits, int32_t value, bool item_response = false);
				PACKET(change_channel, const ip &ip, connection_port port);
				PACKET(show_message, const game_chat &msg, int8_t type);
				PACKET(group_chat, const string &name, const game_chat &msg, int8_t type);
				PACKET(instruction_bubble, const game_chat &msg, game_coord width = -1, int16_t time = 5, bool is_static = false, int32_t x = 0, int32_t y = 0);
				PACKET(show_hp_bar, game_player_id player_id, int32_t hp, int32_t max_hp);
				PACKET(send_blocked_message, int8_t type);
				PACKET(send_week_event_message, const game_chat &msg);
				PACKET(stalk_result, ref_ptr<vana::channel_server::player> player);
				PACKET(stalk_add_or_update_player, ref_ptr<vana::channel_server::player> player);
				PACKET(stalk_remove_player, vector<game_player_id> player_ids);
			}
		}
	}
}