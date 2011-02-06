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
#include "WorldServerConnectHandler.h"
#include "BeautyDataProvider.h"
#include "ChannelServer.h"
#include "Connectable.h"
#include "DropDataProvider.h"
#include "ItemDataProvider.h"
#include "MapDataProvider.h"
#include "MapleSession.h"
#include "MiscUtilities.h"
#include "MobDataProvider.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "PlayersPacket.h"
#include "QuestDataProvider.h"
#include "Rates.h"
#include "ReactorDataProvider.h"
#include "ScriptDataProvider.h"
#include "ShopDataProvider.h"
#include "SkillDataProvider.h"
#include "WorldServerConnection.h"
#include <iostream>
#include <limits>

void WorldServerConnectHandler::connectLogin(WorldServerConnection *player, PacketReader &packet) {
	int8_t worldid = packet.get<int8_t>();
	if (worldid != -1) {
		ChannelServer::Instance()->setWorld(worldid);
		ChannelServer::Instance()->setWorldIp(packet.get<uint32_t>());
		ChannelServer::Instance()->setWorldPort(packet.get<int16_t>());
		std::cout << "Connecting to world " << (int16_t) worldid << std::endl;
		ChannelServer::Instance()->connectWorld();
	}
	else {
		std::cout << "Error: No world server to connect" << std::endl;
		ChannelServer::Instance()->shutdown();
	}
}

void WorldServerConnectHandler::connect(WorldServerConnection *player, PacketReader &packet) {
	int16_t channel = packet.get<int16_t>();
	if (channel != -1) {
		ChannelServer::Instance()->setChannel(channel);
		uint16_t port = packet.get<uint16_t>();
		vector<int8_t> bosschannels;
		int8_t chid = channel + 1;
		ChannelServer::Instance()->setPort(port);
		ChannelServer::Instance()->setMaxMultiLevel(packet.get<int8_t>());
		ChannelServer::Instance()->setMaxStats(packet.get<int16_t>());
		ChannelServer::Instance()->setMaxChars(packet.get<int32_t>());

		ChannelServer::Instance()->setPianusAttempts(packet.get<int16_t>());
		ChannelServer::Instance()->setPapAttempts(packet.get<int16_t>());
		ChannelServer::Instance()->setZakumAttempts(packet.get<int16_t>());
		ChannelServer::Instance()->setHorntailAttempts(packet.get<int16_t>());
		ChannelServer::Instance()->setPinkBeanAttempts(packet.get<int16_t>());

		bosschannels = packet.getVector<int8_t>();
		ChannelServer::Instance()->setPianusChannel(MiscUtilities::isBossChannel(bosschannels, chid));
		ChannelServer::Instance()->setPianusChannels(bosschannels);

		bosschannels = packet.getVector<int8_t>();
		ChannelServer::Instance()->setPapChannel(MiscUtilities::isBossChannel(bosschannels, chid));
		ChannelServer::Instance()->setPapChannels(bosschannels);

		bosschannels = packet.getVector<int8_t>();
		ChannelServer::Instance()->setZakumChannel(MiscUtilities::isBossChannel(bosschannels, chid));
		ChannelServer::Instance()->setZakumChannels(bosschannels);

		bosschannels = packet.getVector<int8_t>();
		ChannelServer::Instance()->setHorntailChannel(MiscUtilities::isBossChannel(bosschannels, chid));
		ChannelServer::Instance()->setHorntailChannels(bosschannels);

		bosschannels = packet.getVector<int8_t>();
		ChannelServer::Instance()->setPinkBeanChannel(MiscUtilities::isBossChannel(bosschannels, chid));
		ChannelServer::Instance()->setPinkBeanChannels(bosschannels);

		ChannelServer::Instance()->listen();
		std::cout << "Handling channel " << channel << " on port " << port << std::endl;
	}
	else {
		std::cout << "Error: No channel to handle" << std::endl;
		ChannelServer::Instance()->shutdown();
	}
}

void WorldServerConnectHandler::playerChangeChannel(WorldServerConnection *player, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	uint32_t ip = packet.get<uint32_t>();
	int16_t port = packet.get<int16_t>();

	Player *ccPlayer = Players::Instance()->getPlayer(playerid);
	if (!ccPlayer) {
		return;
	}
	if (ip == 0) {
		PlayerPacket::sendBlockedMessage(ccPlayer, 0x01);
	}
	else {
		ccPlayer->setOnline(0); // Set online to 0 BEFORE CC packet is sent to player
		PlayerPacket::changeChannel(ccPlayer, ip, port);
		ccPlayer->saveAll(true);
		ccPlayer->setSaveOnDc(false);
	}
}

void WorldServerConnectHandler::findPlayer(PacketReader &packet) {
	int32_t finder = packet.get<int32_t>();
	int16_t channel = packet.get<int16_t>();
	string name = packet.getString();
	int8_t is = packet.get<int8_t>();
	if (channel == -1) {
		PlayersPacket::findPlayer(Players::Instance()->getPlayer(finder), name, -1, is);
	}
	else {
		PlayersPacket::findPlayer(Players::Instance()->getPlayer(finder), name, channel, is, 1);
	}
}

void WorldServerConnectHandler::whisperPlayer(PacketReader &packet) {
	int32_t whisperee = packet.get<int32_t>();
	string whisperer = packet.getString();
	uint16_t channel = packet.get<int16_t>();
	string message = packet.getString();

	PlayersPacket::whisperPlayer(Players::Instance()->getPlayer(whisperee), whisperer, channel, message);
}

void WorldServerConnectHandler::scrollingHeader(PacketReader &packet) {
	string message = packet.getString();
	ChannelServer::Instance()->setScrollingHeader(message);
}

void WorldServerConnectHandler::newConnectable(PacketReader &packet) {
	Connectable::Instance()->newPlayer(packet.get<int32_t>());
}

void WorldServerConnectHandler::forwardPacket(PacketReader &packet) {
	PacketCreator ppacket;
	int32_t playerid = packet.get<int32_t>();
	ppacket.addBuffer(packet);
	Players::Instance()->getPlayer(playerid)->getSession()->send(ppacket);
}

void WorldServerConnectHandler::setRates(PacketReader &packet) {
	int32_t ratesSetBit = packet.get<int32_t>();
	if (ratesSetBit & Rates::SetBits::exp) {
		ChannelServer::Instance()->setExprate(packet.get<int32_t>());
	}
	if (ratesSetBit & Rates::SetBits::questExp) {
		ChannelServer::Instance()->setQuestExprate(packet.get<int32_t>());
	}
	if (ratesSetBit & Rates::SetBits::meso) {
		ChannelServer::Instance()->setMesorate(packet.get<int32_t>());
	}
	if (ratesSetBit & Rates::SetBits::drop) {
		ChannelServer::Instance()->setDroprate(packet.get<int32_t>());
	}
}

void WorldServerConnectHandler::reloadMcdb(PacketReader &packet) {
	string args = packet.getString();
	if (args == "all") {
		ItemDataProvider::Instance()->loadData();
		DropDataProvider::Instance()->loadData();
		ShopDataProvider::Instance()->loadData();
		MobDataProvider::Instance()->loadData();
		BeautyDataProvider::Instance()->loadData();
		ScriptDataProvider::Instance()->loadData();
		SkillDataProvider::Instance()->loadData();
		ReactorDataProvider::Instance()->loadData();
		QuestDataProvider::Instance()->loadData();
	}
	else if (args == "items") ItemDataProvider::Instance()->loadData();
	else if (args == "drops") DropDataProvider::Instance()->loadData();
	else if (args == "shops") ShopDataProvider::Instance()->loadData();
	else if (args == "mobs") MobDataProvider::Instance()->loadData();
	else if (args == "beauty") BeautyDataProvider::Instance()->loadData();
	else if (args == "scripts") ScriptDataProvider::Instance()->loadData();
	else if (args == "skills") SkillDataProvider::Instance()->loadData();
	else if (args == "reactors") ReactorDataProvider::Instance()->loadData();
	else if (args == "quest") QuestDataProvider::Instance()->loadData();
}