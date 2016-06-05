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

#include "Common/FileTime.hpp"
#include "Common/PacketBuilder.hpp"
#include "Common/Types.hpp"
#include <vector>
#include <string>

namespace vana {
	class client_ip;

	namespace login_server {
		class user;
		class world;
		struct character;

		namespace packets {
			namespace check_name_errors {
				enum errors : uint8_t {
					none = 0x00,
					taken = 0x01,
					invalid = 0x04,
					unknown_reason = 0xFF,
				};
			}
			namespace errors {
				enum errors : uint8_t {
					invalid_pin = 0x02,
					invalid_password = 0x04,
					invalid_username = 0x05,
					already_logged_in = 0x07,
				};
			}
			namespace world_messages {
				enum messages : uint8_t {
					normal = 0x00,
					heavy_load = 0x01,
					max_load = 0x02,
				};
			}

			PACKET(login_error, int16_t error_id);
			PACKET(login_ban, int8_t reason, file_time expire);
			PACKET(login_process, int8_t id);
			PACKET(login_connect, ref_ptr<user> user_value, const string &username);
			PACKET(pin_assigned);
			PACKET(gender_done, game_gender_id gender);
			PACKET(show_world, world *world_value);
			PACKET(world_end);
			PACKET(show_channels, int8_t status);
			PACKET(channel_select);
			PACKET(channel_offline);
			PACKET(show_all_characters_info, game_world_id world_count, uint32_t unk);
			PACKET(show_view_all_characters, game_world_id world_id, const vector<character> &chars);
			PACKET(show_characters, const vector<character> &chars, int32_t max_chars);
			PACKET(show_character, const character &charc);
			PACKET(check_name, const string &name, uint8_t message);
			PACKET(delete_character, game_player_id id, uint8_t result);
			PACKET(connect_ip, const optional<client_ip> &ip_value, optional<connection_port> port, game_player_id char_id);
			PACKET(relog_response);
		}
	}
}