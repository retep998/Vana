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
#include "BuddyListHandler.hpp"
#include "Common/PacketReader.hpp"
#include "ChannelServer/BuddyListPacket.hpp"
#include "ChannelServer/Player.hpp"
#include <string>

namespace vana {
namespace channel_server {

namespace buddy_opcodes {
	enum opcodes : int8_t {
		add = 0x01,
		accept_invite = 0x02,
		remove = 0x03
	};
}

auto buddy_list_handler::handle_buddy_list(ref_ptr<player> player, packet_reader &reader) -> void {
	int8_t type = reader.get<int8_t>();
	switch (type) {
		case buddy_opcodes::add: {
			string name = reader.get<string>();
			string group = reader.get<string>();

			uint8_t error = player->get_buddy_list()->add_buddy(name, group);

			if (error) {
				player->send(packets::buddy::error(error));
			}
			break;
		}
		case buddy_opcodes::accept_invite: {
			game_player_id char_id = reader.get<game_player_id>();
			player->get_buddy_list()->remove_pending_buddy(char_id, true);
			break;
		}
		case buddy_opcodes::remove: {
			game_player_id char_id = reader.get<game_player_id>();
			player->get_buddy_list()->remove_buddy(char_id);
			break;
		}
	}
}

}
}