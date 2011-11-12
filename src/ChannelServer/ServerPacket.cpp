/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "ServerPacket.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "SendHeader.h"

void ServerPacket::showScrollingHeader(Player *player, const string &msg) {
	PacketCreator packet;
	packet.addHeader(SMSG_MESSAGE);
	packet.add<int8_t>(4);
	packet.addBool(true);
	packet.addString(msg);
	player->getSession()->send(packet);
}

void ServerPacket::changeScrollingHeader(const string &msg) {
	PacketCreator packet;
	packet.addHeader(SMSG_MESSAGE);
	packet.add<int8_t>(4);
	packet.addBool(true);
	packet.addString(msg);
	PlayerDataProvider::Instance()->sendPacket(packet);
}

void ServerPacket::scrollingHeaderOff() {
	PacketCreator packet;
	packet.addHeader(SMSG_MESSAGE);
	packet.add<int8_t>(4);
	packet.addBool(false);
	PlayerDataProvider::Instance()->sendPacket(packet);
}
