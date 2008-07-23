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
#include "ChannelServer.h"
#include "PlayersPacket.h"
#include "Connectable.h"
#include "Players.h"
#include "Player.h"
#include "ReadPacket.h"
#include "PacketCreator.h"
#include "Rates.h"
#include <iostream>

void WorldServerConnectHandler::connectLogin(WorldServerConnectPlayer *player, ReadPacket *packet) {
	char worldid = packet->getByte();
	if (worldid != 0xFF) {
		ChannelServer::Instance()->setWorld(worldid);
		ChannelServer::Instance()->setWorldIp(packet->getString());
		ChannelServer::Instance()->setWorldPort(packet->getShort());
		std::cout << "Connecting to world " << (int) worldid << std::endl;
		ChannelServer::Instance()->connectWorld();
	}
	else {
		std::cout << "Error: No world server to connect" << std::endl;
		ChannelServer::Instance()->shutdown();
	}
}

void WorldServerConnectHandler::connect(WorldServerConnectPlayer *player, ReadPacket *packet) {
	int channel = packet->getInt();
	if (channel != -1) {
		ChannelServer::Instance()->setChannel(channel);
		short port = packet->getShort();
		ChannelServer::Instance()->setPort(port);
		ChannelServer::Instance()->listen();
		std::cout << "Handling channel " << channel << " on port " << port << std::endl;
	}
	else {
		std::cout << "Error: No channel to handle" << std::endl;
		ChannelServer::Instance()->shutdown();
	}
}

void WorldServerConnectHandler::playerChangeChannel(WorldServerConnectPlayer *player, ReadPacket *packet) {
	int playerid = packet->getInt();
	string ip = packet->getString();
	short port = packet->getShort();
	hash_map<int, Player *>::iterator iter = Players::players.find(playerid);
	if (iter == Players::players.end())
		return;
	PlayerPacket::changeChannel(iter->second, ip, port);
	iter->second->save();
	iter->second->setSaveOnDC(false);
}

void WorldServerConnectHandler::findPlayer(ReadPacket *packet) {
	int finder = packet->getInt();
	int channel = packet->getInt();
	string name = packet->getString();
	char is = packet->getByte();
	if (channel == -1) {
		PlayersPacket::findPlayer(Players::players[finder], name, -1, is);
	}
	else {
		PlayersPacket::findPlayer(Players::players[finder], name, channel, is, 1);
	}
}

void WorldServerConnectHandler::whisperPlayer(ReadPacket *packet) {
	int whisperee = packet->getInt();
	string whisperer = packet->getString();
	int channel = packet->getInt();
	string message = packet->getString();

	PlayersPacket::whisperPlayer(Players::players[whisperee], whisperer, channel, message);
}

void WorldServerConnectHandler::scrollingHeader(ReadPacket *packet) {
	string message = packet->getString();
	ChannelServer::Instance()->setScrollingHeader(message);
}

void WorldServerConnectHandler::newConnectable(ReadPacket *packet) {
	Connectable::Instance()->newPlayer(packet->getInt());
}

void WorldServerConnectHandler::forwardPacket(ReadPacket *packet) {
	PacketCreator ppacket;
	int playerid = packet->getInt();
	ppacket.addBuffer(packet);
	ppacket.send(Players::players[playerid]);
}

void WorldServerConnectHandler::setRates(ReadPacket *packet) {
	int ratesSetBit = packet->getInt();
	if (ratesSetBit & Rates::SetBits::exp) {
		ChannelServer::Instance()->setExprate(packet->getInt());
	}
	if (ratesSetBit & Rates::SetBits::questExp) {
		ChannelServer::Instance()->setQuestExprate(packet->getInt());
	}
	if (ratesSetBit & Rates::SetBits::meso) {
		ChannelServer::Instance()->setMesorate(packet->getInt());
	}
	if (ratesSetBit & Rates::SetBits::drop) {
		ChannelServer::Instance()->setDroprate(packet->getInt());
	}
}
