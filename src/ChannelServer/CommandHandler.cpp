/*
Copyright (C) 2008 Vana Development Team

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
#include "Player.h"
#include "Players.h"
#include "PlayersPacket.h"
#include "ReadPacket.h"
#include "StringUtilities.h"
#include "WorldServerConnectPlayerPacket.h"

void CommandHandler::handleCommand(Player *player, ReadPacket *packet) {
	unsigned char type = packet->getByte();
	string name = packet->getString();

	string chat;
	if (type == 0x06) {
		chat = packet->getString();
	}

	hash_map<int, Player*>::iterator iter = Players::players.begin();
	for (iter = Players::players.begin(); iter != Players::players.end(); iter++) {
		if (StringUtilities::noCaseCompare(iter->second->getName(), name) == 0) {	
			if (type == 0x06) {
				PlayersPacket::whisperPlayer(iter->second, player->getName(), ChannelServer::Instance()->getChannel(), chat);
				PlayersPacket::findPlayer(player,iter->second->getName(),-1,1);
			}
			else if (type == 0x05) {
				PlayersPacket::findPlayer(player, iter->second->getName(), iter->second->getMap());
			}
			break;
		}
	}	
	if (iter == Players::players.end()) {
		if (type == 0x05)
			WorldServerConnectPlayerPacket::findPlayer(ChannelServer::Instance()->getWorldPlayer(), player->getPlayerid(), name); // Let's connect to the world server to see if the player is on any other channel
		else
			WorldServerConnectPlayerPacket::whisperPlayer(ChannelServer::Instance()->getWorldPlayer(), player->getPlayerid(), name, chat);
	}
}
