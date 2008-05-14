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
#include "WorldServerConnectHandler.h"
#include "WorldServerConnectPlayer.h"
#include "PlayerPacket.h"
#include "BufferUtilities.h"
#include "ChannelServer.h"
#include "PlayersPacket.h"
#include "Players.h"
#include "Player.h"
#include <iostream>

void WorldServerConnectHandler::connectLogin(WorldServerConnectPlayer *player, unsigned char *packet) {
	if (packet[0] != 0xFF) {
		ChannelServer::Instance()->setWorld(packet[0]);
		char worldip[255];
		int worldlen = BufferUtilities::getShort(packet+1);
		BufferUtilities::getString(packet+3, worldlen, worldip);
		ChannelServer::Instance()->setWorldIp(worldip);
		ChannelServer::Instance()->setWorldPort(BufferUtilities::getShort(packet+3+worldlen));
		std::cout << "Connecting to world " << (int) packet[0] << std::endl;
		ChannelServer::Instance()->connectWorld();
	}
	else {
		std::cout << "Error: No world server to connect" << std::endl;
		ChannelServer::Instance()->shutdown();
	}
}

void WorldServerConnectHandler::connect(WorldServerConnectPlayer *player, unsigned char *packet) {
	int channel = BufferUtilities::getInt(packet);
	if (channel != -1) {
		ChannelServer::Instance()->setChannel(channel);
		short port = BufferUtilities::getShort(packet+4);
		ChannelServer::Instance()->setPort(port);
		ChannelServer::Instance()->listen();
		std::cout << "Handling channel " << (int) packet[0] << " on port " << port <<  std::endl;
	}
	else {
		std::cout << "Error: No channel to handle" << std::endl;
		ChannelServer::Instance()->shutdown();
	}
}

void WorldServerConnectHandler::playerChangeChannel(WorldServerConnectPlayer *player, unsigned char *packet) {
	int playerid = BufferUtilities::getInt(packet);
	short iplen = BufferUtilities::getShort(packet+4);
	char ip[15];
	BufferUtilities::getString(packet+6, iplen, ip);
	short port = BufferUtilities::getShort(packet+6+iplen);
	hash_map <int, Player *>::iterator iter = Players::players.find(playerid);
	if (iter == Players::players.end())
		return;
	PlayerPacket::changeChannel(iter->second, ip, port);
	iter->second->save();
	iter->second->setSaveOnDC(false);
}

void WorldServerConnectHandler::findPlayer(unsigned char *packet) {
	int finder = BufferUtilities::getInt(packet);
	int channel = BufferUtilities::getInt(packet+4);
	short namelen = BufferUtilities::getShort(packet+8);
	char name[15];
	BufferUtilities::getString(packet+10, namelen, name);
	if (channel == -1) {
		PlayersPacket::findPlayer(Players::players[finder], name, -1, packet[10+namelen]);
	}
	else {
		PlayersPacket::findPlayer(Players::players[finder], name, channel, packet[10+namelen], 1);
	}
}

void WorldServerConnectHandler::whisperPlayer(unsigned char *packet) {
	int whisperee = BufferUtilities::getInt(packet);
	short whispererlen = BufferUtilities::getShort(packet+4);
	char whisperer[15];
	BufferUtilities::getString(packet+6, whispererlen, whisperer);
	int channel = BufferUtilities::getInt(packet+6+whispererlen);
	short messagelen = BufferUtilities::getShort(packet+10+whispererlen);
	char message[91];
	BufferUtilities::getString(packet+12+whispererlen, messagelen, message);

	PlayersPacket::whisperPlayer(Players::players[whisperee], whisperer, channel, message);
}

void WorldServerConnectHandler::scrollingHeader(unsigned char *packet) {
	short messagelen = BufferUtilities::getShort(packet);
	char message[100];
	BufferUtilities::getString(packet+2, messagelen, message);
	ChannelServer::Instance()->setScrollingHeader(message);
}
