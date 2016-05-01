/*
Copyright (C) 2008-2015 Vana Development Team

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

namespace Vana {
namespace ChannelServer {

namespace BuddyOpcodes {
	enum Opcodes : int8_t {
		Add = 0x01,
		AcceptInvite = 0x02,
		Remove = 0x03
	};
}

auto BuddyListHandler::handleBuddyList(ref_ptr_t<Player> player, PacketReader &reader) -> void {
	int8_t type = reader.get<int8_t>();
	switch (type) {
		case BuddyOpcodes::Add: {
			string_t name = reader.get<string_t>();
			string_t group = reader.get<string_t>();

			uint8_t error = player->getBuddyList()->addBuddy(name, group);

			if (error) {
				player->send(Packets::Buddy::error(error));
			}
			break;
		}
		case BuddyOpcodes::AcceptInvite: {
			player_id_t charId = reader.get<player_id_t>();
			player->getBuddyList()->removePendingBuddy(charId, true);
			break;
		}
		case BuddyOpcodes::Remove: {
			player_id_t charId = reader.get<player_id_t>();
			player->getBuddyList()->removeBuddy(charId);
			break;
		}
	}
}

}
}