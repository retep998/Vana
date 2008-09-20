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
#include "ChannelServer.h"
#include "Connectable.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "PlayersPacket.h"
#include "Rates.h"
#include "ReadPacket.h"
#include "WorldServerConnectPlayer.h"
#include <iostream>

void WorldServerConnectHandler::connectLogin(WorldServerConnectPlayer *player, ReadPacket *packet) {
	int8_t worldid = packet->getByte();
	if (worldid != 0xFF) {
		ChannelServer::Instance()->setWorld(worldid);
		ChannelServer::Instance()->setWorldIp(packet->getString());
		ChannelServer::Instance()->setWorldPort(packet->getShort());
		std::cout << "Connecting to world " << (int32_t) worldid << std::endl;
		ChannelServer::Instance()->connectWorld();
	}
	else {
		std::cout << "Error: No world server to connect" << std::endl;
		ChannelServer::Instance()->shutdown();
	}
}

void WorldServerConnectHandler::connect(WorldServerConnectPlayer *player, ReadPacket *packet) {
	uint16_t channel = packet->getShort();
	if (channel != -1) {
		ChannelServer::Instance()->setChannel(channel);
		int16_t port = packet->getShort();
		ChannelServer::Instance()->setPort(port);
		ChannelServer::Instance()->setMaxMultiLevel(packet->getByte());
		ChannelServer::Instance()->setMaxStats(packet->getShort());
		ChannelServer::Instance()->listen();
		std::cout << "Handling channel " << channel << " on port " << port << std::endl;
	}
	else {
		std::cout << "Error: No channel to handle" << std::endl;
		ChannelServer::Instance()->shutdown();
	}
}

void WorldServerConnectHandler::playerChangeChannel(WorldServerConnectPlayer *player, ReadPacket *packet) {
	int32_t playerid = packet->getInt();
	string ip = packet->getString();
	int16_t port = packet->getShort();
	
	Player *ccPlayer = Players::Instance()->getPlayer(playerid);
	if (!ccPlayer) {
		return;
	}
	PlayerPacket::changeChannel(ccPlayer, ip, port);
	ccPlayer->saveAll();
	ccPlayer->setSaveOnDC(false);
}

void WorldServerConnectHandler::findPlayer(ReadPacket *packet) {
	int32_t finder = packet->getInt();
	uint16_t channel = packet->getShort();
	string name = packet->getString();
	int8_t is = packet->getByte();
	if (channel == 65535) {
		PlayersPacket::findPlayer(Players::Instance()->getPlayer(finder), name, -1, is);
	}
	else {
		PlayersPacket::findPlayer(Players::Instance()->getPlayer(finder), name, channel, is, 1);
	}
}

void WorldServerConnectHandler::whisperPlayer(ReadPacket *packet) {
	int32_t whisperee = packet->getInt();
	string whisperer = packet->getString();
	uint16_t channel = packet->getShort();
	string message = packet->getString();

	PlayersPacket::whisperPlayer(Players::Instance()->getPlayer(whisperee), whisperer, channel, message);
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
	int32_t playerid = packet->getInt();
	ppacket.addBuffer(packet);
	Players::Instance()->getPlayer(playerid)->getSession()->send(ppacket);
}

void WorldServerConnectHandler::setRates(ReadPacket *packet) {
	int32_t ratesSetBit = packet->getInt();
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
