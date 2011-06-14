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
#include "Configuration.h"
#include "DropDataProvider.h"
#include "ItemDataProvider.h"
#include "MapDataProvider.h"
#include "MiscUtilities.h"
#include "MobDataProvider.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayerDataProvider.h"
#include "PlayersPacket.h"
#include "QuestDataProvider.h"
#include "Rates.h"
#include "ReactorDataProvider.h"
#include "ScriptDataProvider.h"
#include "Session.h"
#include "ShopDataProvider.h"
#include "SkillDataProvider.h"
#include "WorldServerConnection.h"
#include <iostream>
#include <limits>

void WorldServerConnectHandler::connectLogin(WorldServerConnection *player, PacketReader &packet) {
	int8_t worldId = packet.get<int8_t>();
	if (worldId != -1) {
		ChannelServer::Instance()->setWorld(worldId);
		ChannelServer::Instance()->setWorldIp(packet.get<ip_t>());
		ChannelServer::Instance()->setWorldPort(packet.get<port_t>());
		std::cout << "Connecting to world " << (int16_t) worldId << std::endl;
		ChannelServer::Instance()->connectWorld();
	}
	else {
		std::cerr << "Error: No world server to connect" << std::endl;
		ChannelServer::Instance()->shutdown();
	}
}

void WorldServerConnectHandler::connect(WorldServerConnection *player, PacketReader &packet) {
	int16_t channel = packet.get<int16_t>();
	if (channel != -1) {
		int8_t chId = channel + 1;
		port_t port = packet.get<port_t>();
		ChannelServer::Instance()->setChannel(channel);
		ChannelServer::Instance()->setPort(port);

		Configuration conf = packet.getClass<Configuration>();

		ChannelServer::Instance()->setConfig(conf);
		ChannelServer::Instance()->setPianusChannel(MiscUtilities::isBossChannel(conf.pianus.channels, chId));
		ChannelServer::Instance()->setPapChannel(MiscUtilities::isBossChannel(conf.pap.channels, chId));
		ChannelServer::Instance()->setZakumChannel(MiscUtilities::isBossChannel(conf.zakum.channels, chId));
		ChannelServer::Instance()->setHorntailChannel(MiscUtilities::isBossChannel(conf.horntail.channels, chId));
		ChannelServer::Instance()->setPinkBeanChannel(MiscUtilities::isBossChannel(conf.pinkbean.channels, chId));
		ChannelServer::Instance()->listen();
		std::cout << "Handling channel " << channel << " on port " << port << std::endl;

		ChannelServer::Instance()->displayLaunchTime();
	}
	else {
		std::cerr << "Error: No channel to handle" << std::endl;
		ChannelServer::Instance()->shutdown();
	}
}

void WorldServerConnectHandler::findPlayer(PacketReader &packet) {
	int32_t finder = packet.get<int32_t>();
	int16_t channel = packet.get<int16_t>();
	string name = packet.getString();
	int8_t is = packet.get<int8_t>();
	if (channel == -1) {
		PlayersPacket::findPlayer(PlayerDataProvider::Instance()->getPlayer(finder), name, -1, is);
	}
	else {
		PlayersPacket::findPlayer(PlayerDataProvider::Instance()->getPlayer(finder), name, channel, is, 1);
	}
}

void WorldServerConnectHandler::whisperPlayer(PacketReader &packet) {
	int32_t whisperee = packet.get<int32_t>();
	string whisperer = packet.getString();
	uint16_t channel = packet.get<int16_t>();
	string message = packet.getString();

	PlayersPacket::whisperPlayer(PlayerDataProvider::Instance()->getPlayer(whisperee), whisperer, channel, message);
}

void WorldServerConnectHandler::scrollingHeader(PacketReader &packet) {
	string message = packet.getString();
	ChannelServer::Instance()->setScrollingHeader(message);
}

void WorldServerConnectHandler::forwardPacket(PacketReader &packet) {
	PacketCreator ppacket;
	int32_t playerId = packet.get<int32_t>();
	ppacket.addBuffer(packet);
	PlayerDataProvider::Instance()->getPlayer(playerId)->getSession()->send(ppacket);
}

void WorldServerConnectHandler::setRates(PacketReader &packet) {
	int32_t ratesSetBit = packet.get<int32_t>();
	if (ratesSetBit & Rates::SetBits::Exp) {
		ChannelServer::Instance()->setExpRate(packet.get<int32_t>());
	}
	if (ratesSetBit & Rates::SetBits::QuestExp) {
		ChannelServer::Instance()->setQuestExpRate(packet.get<int32_t>());
	}
	if (ratesSetBit & Rates::SetBits::Meso) {
		ChannelServer::Instance()->setMesoRate(packet.get<int32_t>());
	}
	if (ratesSetBit & Rates::SetBits::Drop) {
		ChannelServer::Instance()->setDropRate(packet.get<int32_t>());
	}
}

void WorldServerConnectHandler::sendToPlayers(PacketReader &packet) {
	PlayersPacket::sendToPlayers(packet.getBuffer(), packet.getBufferLength());
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