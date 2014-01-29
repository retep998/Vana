/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "ServerPacket.hpp"
#include "PacketCreator.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"

auto ServerPacket::showScrollingHeader(Player *player, const string_t &msg) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MESSAGE);
	packet.add<int8_t>(4);
	packet.add<int8_t>(1);
	packet.addString(msg);
	player->getSession()->send(packet);
}

auto ServerPacket::changeScrollingHeader(const string_t &msg) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MESSAGE);
	packet.add<int8_t>(4);
	packet.add<bool>(msg.size() > 0);
	if (msg.size() > 0) {
		packet.addString(msg);
	}
	PlayerDataProvider::getInstance().sendPacket(packet);
}