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
#include "CommandHandler.h"
#include "ChannelServer.h"
#include "Player.h"
#include "Players.h"
#include "PlayersPacket.h"
#include "PacketReader.h"
#include "WorldServerConnectPacket.h"
#include <string>

using std::string;

void CommandHandler::handleCommand(Player *player, PacketReader &packet) {
	uint8_t type = packet.get<int8_t>();
	string name = packet.getString();

	string chat;
	if (type == 0x06) {
		chat = packet.getString();
	}

	Player *receiver = Players::Instance()->getPlayer(name);
	if (receiver) {
		if (type == 0x05) {
			PlayersPacket::findPlayer(player, receiver->getName(), receiver->getMap());
		}
		else if (type == 0x06) {
			PlayersPacket::whisperPlayer(receiver, player->getName(), ChannelServer::Instance()->getChannel(), chat);
			PlayersPacket::findPlayer(player, receiver->getName(), -1, 1);
		}
	}	
	else { // Let's connect to the world server to see if the player is on any other channel
		if (type == 0x05) {
			WorldServerConnectPacket::findPlayer(ChannelServer::Instance()->getWorldPlayer(), player->getId(), name);
		}
		else if (type == 0x06) {
			WorldServerConnectPacket::whisperPlayer(ChannelServer::Instance()->getWorldPlayer(), player->getId(), name, chat);
		}
	}
}
