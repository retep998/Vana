/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "BuddyListHandler.h"
#include "BuddyListPacket.h"
#include "Player.h"
#include "PacketReader.h"
#include <string>

using std::string;

void BuddyListHandler::handleBuddyList(Player *player, PacketReader &packet) {
	uint8_t type = packet.get<int8_t>();
	if (type == 1) { // Add
		string name = packet.getString();

		uint8_t error = player->getBuddyList()->add(name);

		if (error) {
			BuddyListPacket::error(player, error);
		}
	}
	else if (type == 3) { // Remove
		int32_t charid = packet.get<int32_t>();
		player->getBuddyList()->remove(charid);
	}
}
